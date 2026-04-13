#![no_std]
#![no_main]

mod config;
mod ldr;
mod led;
mod logging;
mod sensor;

use embassy_executor::Spawner;
use esp_backtrace as _;
use esp_hal::analog::adc::AdcCalLine;
use esp_hal::ledc::timer::TimerIFace;
use esp_hal::ledc::{LSGlobalClkSource, Ledc};
use esp_hal::peripherals::GPIO9;
use esp_hal::peripherals::{ADC1, GPIO8};
use esp_hal::timer::timg::TimerGroup;
use led::Led;
use static_cell::StaticCell;

use crate::ldr::Sensor;
use crate::sensor::current_value;
esp_bootloader_esp_idf::esp_app_desc!();

static SENSOR: StaticCell<
    Sensor<'static, ADC1<'static>, GPIO8<'static>, GPIO9<'static>, AdcCalLine<ADC1<'static>>>,
> = StaticCell::new();

#[esp_rtos::main]
async fn main(spawner: Spawner) -> ! {
    logging::init_logging(log::LevelFilter::Info);
    log::info!("Init...");
    let peripherals = esp_hal::init(esp_hal::Config::default());

    let timg0 = TimerGroup::new(peripherals.TIMG0);
    esp_rtos::start(timg0.timer0);

    let sensor_ref = SENSOR.init(Sensor::new(
        peripherals.ADC1,
        peripherals.GPIO8,
        peripherals.GPIO9,
    ));
    sensor_ref.calibrate().await;
    spawner
        .spawn(sensor::sensor_collection(sensor_ref))
        .unwrap();

    let mut ledc = Ledc::new(peripherals.LEDC);
    ledc.set_global_slow_clock(LSGlobalClkSource::APBClk);
    let mut timer0 = ledc.timer(esp_hal::ledc::timer::Number::Timer0);
    let timer_config = esp_hal::ledc::timer::config::Config {
        duty: esp_hal::ledc::timer::config::Duty::Duty10Bit,
        clock_source: esp_hal::ledc::timer::LSClockSource::APBClk,
        frequency: config::LED_TIMER_FREQUENCY,
    };
    timer0.configure(timer_config).unwrap();

    let mut led = Led::new(peripherals.GPIO5, ledc, &timer0);

    loop {
        let sensor_value_pct = current_value().await;
        led.update(sensor_value_pct);

        embassy_time::Timer::after(config::LED_UPDATE_PERIOD).await;
    }
}
