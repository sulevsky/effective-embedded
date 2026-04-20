#![no_std]
#![no_main]

mod config;
mod led;
mod logging;
mod non_volatile_storage;

use core::cell::RefCell;

use embassy_executor::Spawner;
use embassy_sync::{
    blocking_mutex::{Mutex, raw::CriticalSectionRawMutex},
    signal::Signal,
};
use esp_backtrace as _;
use esp_hal::{
    Blocking, handler,
    time::Duration,
    timer::{PeriodicTimer, timg::TimerGroup},
};

use crate::non_volatile_storage::NonVolatileStorage;

esp_bootloader_esp_idf::esp_app_desc!();

static TIMER: Mutex<CriticalSectionRawMutex, RefCell<Option<PeriodicTimer<'static, Blocking>>>> =
    Mutex::new(RefCell::new(None));

static INTERRUPT_TRIGGERED: Signal<CriticalSectionRawMutex, ()> = Signal::new();

#[handler]
fn interrupt_handler() {
    critical_section::with(|cs| {
        TIMER
            .borrow(cs)
            .borrow_mut()
            .as_mut()
            .unwrap()
            .clear_interrupt();
        INTERRUPT_TRIGGERED.signal(());
    });
    if cfg!(feature = "debug-timing") {
        // not allowed in ISR
        log::info!("In interrupt");
    }
}

fn is_on(passed_duration: esp_hal::time::Duration) -> bool {
    passed_duration < config::values::IS_ON_DURATION
}

#[esp_rtos::main]
async fn main(_spawner: Spawner) -> ! {
    logging::init_logging(log::LevelFilter::Info);
    log::info!("Init...");

    let peripherals = esp_hal::init(esp_hal::Config::default());

    let timg0 = TimerGroup::new(peripherals.TIMG0);
    esp_rtos::start(timg0.timer0);

    let mut led = led::Led::new(peripherals.GPIO5);

    let timg1 = TimerGroup::new(peripherals.TIMG1);
    let mut timer = PeriodicTimer::new(timg1.timer0);
    timer.set_interrupt_handler(interrupt_handler);
    timer.start(config::values::TICK_DURATION).unwrap();

    critical_section::with(|cs| {
        timer.listen();
        TIMER.borrow(cs).borrow_mut().replace(timer);
    });
    let mut storage = NonVolatileStorage::new(peripherals.FLASH);
    let mut passed_time = deserialize_duration(storage.load().await);
    loop {
        let is_on = is_on(passed_time);
        led.set_on(is_on);
        if cfg!(feature = "debug-timing") {
            log::info!("Seconds: {}, is on: {}", passed_time.as_secs(), is_on);
        } else {
            log::info!("Minutes: {}, is on: {}", passed_time.as_minutes(), is_on);
        }

        INTERRUPT_TRIGGERED.wait().await;
        passed_time = passed_time + config::values::TICK_DURATION;
        if passed_time > config::values::CYCLE_DURATION {
            passed_time = Duration::ZERO;
        }
        storage.save(serialize_duration(&passed_time)).await;
    }
}

fn serialize_duration(duration: &Duration) -> u32 {
    (if cfg!(feature = "debug-timing") {
        duration.as_secs()
    } else {
        duration.as_minutes()
    }) as u32
}

fn deserialize_duration(duration: Option<u32>) -> Duration {
    if let Some(duration) = duration {
        let duration = duration.max(0) as u64;

        if cfg!(feature = "debug-timing") {
            let duration = duration.min(config::values::CYCLE_DURATION.as_secs());
            Duration::from_secs(duration)
        } else {
            let duration = duration.min(config::values::CYCLE_DURATION.as_minutes());
            Duration::from_minutes(duration)
        }
    } else {
        Duration::ZERO
    }
}
