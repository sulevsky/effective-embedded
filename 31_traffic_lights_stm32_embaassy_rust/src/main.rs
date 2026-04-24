#![no_std]
#![no_main]

mod led;
mod switch;

use core::cell::RefCell;

use critical_section::Mutex;
use defmt::info;
use defmt_rtt as _;
use embassy_executor::Spawner;
use embassy_stm32::{
    Config,
    gpio::{Input, Output},
};

use crate::{led::Led, switch::Switch};

use embassy_time::{Duration, Instant, Timer};
use panic_probe as _;

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
static GREEN_LED: Mutex<RefCell<Option<Led>>> = Mutex::new(RefCell::new(None));
static LEDS: [&Mutex<RefCell<Option<Led>>>; 3] = [&RED_LED, &YELLOW_LED, &GREEN_LED];

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
                on(&GREEN_LED);
                Timer::after(duration).await;
                current_state = State::GreenBlinking(Duration::from_secs(10))
            }
            State::GreenBlinking(duration) => {
                off(&RED_LED);
                off(&YELLOW_LED);
                blinking(&GREEN_LED);

                Timer::after(duration).await;
                current_state = State::Yellow(Duration::from_secs(10))
            }
            State::Yellow(duration) => {
                off(&RED_LED);
                on(&YELLOW_LED);
                off(&GREEN_LED);
                Timer::after(duration).await;
                current_state = State::Red(Duration::from_secs(30))
            }
            State::YellowBlinking(duration) => {
                off(&RED_LED);
                blinking(&YELLOW_LED);
                off(&GREEN_LED);
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
                off(&GREEN_LED);

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
                off(&GREEN_LED);

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

#[embassy_executor::main]
async fn main(spawner: Spawner) {
    info!("Init...");
    let peripherals = embassy_stm32::init(Config::default());

    let red = Led::new(Output::new(
        peripherals.PA3,
        embassy_stm32::gpio::Level::Low,
        embassy_stm32::gpio::Speed::Low,
    ));
    let yellow = Led::new(Output::new(
        peripherals.PA4,
        embassy_stm32::gpio::Level::Low,
        embassy_stm32::gpio::Speed::Low,
    ));
    let green = Led::new(Output::new(
        peripherals.PA5,
        embassy_stm32::gpio::Level::Low,
        embassy_stm32::gpio::Speed::Low,
    ));

    let switcher = switch::Switch::new(Input::new(peripherals.PA6, embassy_stm32::gpio::Pull::Up));

    critical_section::with(|cs| {
        *RED_LED.borrow_ref_mut(cs) = Some(red);
        *YELLOW_LED.borrow_ref_mut(cs) = Some(yellow);
        *GREEN_LED.borrow_ref_mut(cs) = Some(green);
    });
    spawner.spawn(blinker().unwrap());
    spawner.spawn(state_manager(switcher).unwrap());
}
