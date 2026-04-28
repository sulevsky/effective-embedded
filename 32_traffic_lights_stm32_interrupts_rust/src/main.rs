#![no_std]
#![no_main]

mod led;
mod switch;

use core::cell::RefCell;
use core::time::Duration;

use critical_section::Mutex;
use defmt::info;
use stm32f4xx_hal::timer::CounterMs;

use defmt_rtt as _;
use led::Led;
use panic_probe as _;
use stm32f4xx_hal::pac::{TIM2, interrupt};
use stm32f4xx_hal::prelude::*;
use stm32f4xx_hal::{
    gpio::GpioExt,
    pac,
    rcc::{Config, RccExt},
    time::Hertz,
    timer::{Event, TimerExt},
};
use switch::Switch;

#[derive(Clone, Copy)]
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

static TIMER: Mutex<RefCell<Option<CounterMs<TIM2>>>> = Mutex::new(RefCell::new(None));
static COUNTER: Mutex<RefCell<Duration>> = Mutex::new(RefCell::new(Duration::ZERO));

static SWITCHER: Mutex<RefCell<Option<Switch>>> = Mutex::new(RefCell::new(None));

fn blinker(now: Duration) {
    critical_section::with(|cs| {
        LEDS.iter().for_each(|l| {
            l.borrow_ref_mut(cs)
                .as_mut()
                .expect("Not initialized")
                .on_timer_event(now);
        });
    });
}

static CURRENT_STATE: Mutex<RefCell<State>> =
    Mutex::new(RefCell::new(State::Red(Duration::from_secs(10))));
fn state_manager(now: Duration, switch_is_on: bool) {
    //initial - start
    let mut current_state = critical_section::with(|cs| *CURRENT_STATE.borrow_ref(cs));
    match current_state {
        State::Green(expires_at)
        | State::GreenBlinking(expires_at)
        | State::Yellow(expires_at)
        | State::YellowBlinking(expires_at)
        | State::Red(expires_at)
        | State::RedYellow(expires_at) => {
            if now < expires_at {
                return;
            }
        }
    };
    match current_state {
        State::Green(_) => {
            off(&RED_LED);
            off(&YELLOW_LED);
            on(&GREEN_LED);
            current_state = State::GreenBlinking(now + Duration::from_secs(10))
        }
        State::GreenBlinking(_) => {
            off(&RED_LED);
            off(&YELLOW_LED);
            blinking(&GREEN_LED);

            current_state = State::Yellow(now + Duration::from_secs(10))
        }
        State::Yellow(_) => {
            off(&RED_LED);
            on(&YELLOW_LED);
            off(&GREEN_LED);
            current_state = State::Red(now + Duration::from_secs(30))
        }
        State::YellowBlinking(_) => {
            off(&RED_LED);
            blinking(&YELLOW_LED);
            off(&GREEN_LED);
            if switch_is_on {
                current_state = State::YellowBlinking(now + Duration::from_secs(5))
            } else {
                current_state = State::Red(now + Duration::from_secs(5))
            }
        }
        State::Red(_) => {
            on(&RED_LED);
            off(&YELLOW_LED);
            off(&GREEN_LED);

            if switch_is_on {
                current_state = State::YellowBlinking(now + Duration::from_secs(5))
            } else {
                current_state = State::RedYellow(now + Duration::from_secs(10))
            }
        }
        State::RedYellow(_) => {
            on(&RED_LED);
            on(&YELLOW_LED);
            off(&GREEN_LED);

            current_state = State::Green(now + Duration::from_secs(30))
        }
    }
    critical_section::with(|cs| {
        *CURRENT_STATE.borrow(cs).borrow_mut() = current_state;
    });
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

#[interrupt]
fn TIM2() {
    let (now, switcher_is_on) = critical_section::with(|cs| {
        TIMER.borrow_ref_mut(cs).as_mut().unwrap().clear_all_flags();
        let now = *COUNTER.borrow_ref_mut(cs);
        let new_now = now + Duration::from_millis(10);
        *COUNTER.borrow_ref_mut(cs) = new_now;
        let switcher_is_on = SWITCHER
            .borrow_ref(cs)
            .as_ref()
            .expect("Not initialized")
            .is_on();
        (new_now, switcher_is_on)
    });
    blinker(now);
    state_manager(now, switcher_is_on);
}

#[cortex_m_rt::entry]
fn main() -> ! {
    info!("Init...");
    let _cp = cortex_m::Peripherals::take().unwrap();
    let dp = pac::Peripherals::take().unwrap();

    let mut rcc = dp
        .RCC
        .freeze(Config::hsi().sysclk(Hertz::from_raw(48_000_000)));
    let parts = dp.GPIOA.split(&mut rcc);
    let pa3 = parts.pa3.into_push_pull_output().erase();
    let pa4 = parts.pa4.into_push_pull_output().erase();
    let pa5 = parts.pa5.into_push_pull_output().erase();

    let red = Led::new(pa3);
    let yellow = Led::new(pa4);
    let green = Led::new(pa5);

    let mut timer = dp.TIM2.counter_ms(&mut rcc);

    timer.start(10.millis()).unwrap();
    timer.listen(Event::Update);
    let pa5 = parts.pa6.into_pull_up_input().erase();
    let switcher = switch::Switch::new(pa5);

    critical_section::with(|cs| {
        TIMER.borrow(cs).borrow_mut().replace(timer);
        *RED_LED.borrow_ref_mut(cs) = Some(red);
        *YELLOW_LED.borrow_ref_mut(cs) = Some(yellow);
        *GREEN_LED.borrow_ref_mut(cs) = Some(green);
        SWITCHER.borrow(cs).borrow_mut().replace(switcher);
    });

    unsafe {
        cortex_m::peripheral::NVIC::unmask(interrupt::TIM2);
    }

    loop {
        cortex_m::asm::wfi();
    }
}
