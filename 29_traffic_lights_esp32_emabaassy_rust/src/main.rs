#![no_std]
#![no_main]

mod led;
mod logging;
mod switch;

use core::cell::RefCell;

use critical_section::Mutex;
use embassy_executor::Spawner;
use embassy_time::{Duration, Instant, Timer};
use esp_backtrace as _;
use esp_hal::timer::timg::TimerGroup;

use crate::{led::Led, switch::Switch};

esp_bootloader_esp_idf::esp_app_desc!();

enum State {
    Green(Duration),
    GreenBlinking(Duration),
    Yellow(Duration),
    YellowBlinking(Duration),
    Red(Duration),
    RedYellow(Duration),
}

static RED_LED: Mutex<RefCell<Option<Led>>> = Mutex::new(RefCell::new(None));
static YELLOW_LED: Mutex<RefCell<Option<Led>>> = Mutex::new(RefCell::new(None));
static GREEEN_LED: Mutex<RefCell<Option<Led>>> = Mutex::new(RefCell::new(None));
static LEDS: [&Mutex<RefCell<Option<Led>>>; 3] = [&RED_LED, &YELLOW_LED, &GREEEN_LED];

#[embassy_executor::task]
async fn blinker() {
    loop {
        critical_section::with(|cs| {
            let now = Instant::now();
            LEDS.iter().for_each(|m| {
                if let Some(led) = m.borrow_ref_mut(cs).as_mut() {
                    led.on_timer_event(now)
                };
            });
        });
        Timer::after_millis(10).await;
    }
}
#[embassy_executor::task]
async fn state_manager(switch: Switch<'static>) {
    //initial - start
    let mut current_state = State::Red(Duration::MIN);
    on(&RED_LED);
    Timer::after(Duration::from_secs(10)).await;
    //initial - end

    loop {
        match current_state {
            State::Green(duration) => {
                off(&RED_LED);
                off(&YELLOW_LED);
                on(&GREEEN_LED);
                Timer::after(duration).await;
                current_state = State::GreenBlinking(Duration::from_secs(10))
            }
            State::GreenBlinking(duration) => {
                off(&RED_LED);
                off(&YELLOW_LED);
                blinking(&GREEEN_LED);

                Timer::after(duration).await;
                current_state = State::Yellow(Duration::from_secs(10))
            }
            State::Yellow(duration) => {
                off(&RED_LED);
                on(&YELLOW_LED);
                off(&GREEEN_LED);
                Timer::after(duration).await;
                current_state = State::Red(Duration::from_secs(30))
            }
            State::YellowBlinking(duration) => {
                off(&RED_LED);
                blinking(&YELLOW_LED);
                off(&GREEEN_LED);
                Timer::after(duration).await;
                if switch.is_on() {
                    current_state = State::YellowBlinking(Duration::from_secs(5))
                } else {
                    current_state = State::Red(Duration::from_secs(5))
                }
            }
            State::Red(duration) => {
                on(&RED_LED);
                off(&YELLOW_LED);
                off(&GREEEN_LED);

                Timer::after(duration).await;
                if switch.is_on() {
                    current_state = State::YellowBlinking(Duration::from_secs(5))
                } else {
                    current_state = State::RedYellow(Duration::from_secs(10))
                }
            }
            State::RedYellow(duration) => {
                on(&RED_LED);
                on(&YELLOW_LED);
                off(&GREEEN_LED);

                Timer::after(duration).await;
                current_state = State::Green(Duration::from_secs(30))
            }
        }
    }
}

fn on(wrapped_led: &Mutex<RefCell<Option<Led>>>) {
    critical_section::with(|cs| {
        wrapped_led
            .borrow_ref_mut(cs)
            .as_mut()
            .expect("Mutex is not initialized")
            .on();
    });
}
fn off(wrapped_led: &Mutex<RefCell<Option<Led>>>) {
    critical_section::with(|cs| {
        wrapped_led
            .borrow_ref_mut(cs)
            .as_mut()
            .expect("Mutex is not initialized")
            .off();
    });
}
fn blinking(wrapped_led: &Mutex<RefCell<Option<Led>>>) {
    critical_section::with(|cs| {
        wrapped_led
            .borrow_ref_mut(cs)
            .as_mut()
            .expect("Mutex is not initialized")
            .blinking();
    });
}

#[esp_rtos::main]
async fn main(spawner: Spawner) {
    logging::init_logging(log::LevelFilter::Info);
    log::info!("Init...");

    let peripherals = esp_hal::init(esp_hal::Config::default());

    let timg0 = TimerGroup::new(peripherals.TIMG0);
    esp_rtos::start(timg0.timer0);

    let red = led::Led::new(peripherals.GPIO6);
    let yellow = led::Led::new(peripherals.GPIO5);
    let green = led::Led::new(peripherals.GPIO4);
    let switch = switch::Switch::new(peripherals.GPIO7, esp_hal::gpio::Pull::Up);

    critical_section::with(|cs| {
        *RED_LED.borrow_ref_mut(cs) = Some(red);
        *YELLOW_LED.borrow_ref_mut(cs) = Some(yellow);
        *GREEEN_LED.borrow_ref_mut(cs) = Some(green);
    });
    spawner
        .spawn(blinker())
        .expect("Could not spawn blinker task");
    spawner
        .spawn(state_manager(switch))
        .expect("Could not spawn state_manager task");
}
