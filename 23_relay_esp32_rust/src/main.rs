#![no_std]
#![no_main]

mod actuator;
mod config;
mod input;
mod logging;

use actuator::Actuator;
use embassy_executor::Spawner;
use embassy_time::{Duration, Instant, Timer};
use esp_backtrace as _;
use esp_hal::timer::timg::TimerGroup;

use input::Input;
esp_bootloader_esp_idf::esp_app_desc!();

#[esp_rtos::main]
async fn main(_spawner: Spawner) -> ! {
    logging::init_logging(log::LevelFilter::Info);
    log::info!("Init...");

    let peripherals = esp_hal::init(esp_hal::Config::default());

    let timg0 = TimerGroup::new(peripherals.TIMG0);
    esp_rtos::start(timg0.timer0);

    let mut input = Input::new(peripherals.GPIO5, esp_hal::gpio::Pull::Up);
    let mut actuator = Actuator::new(peripherals.GPIO4);

    loop {
        log::info!("Starting measurement");
        let mut time_measurement_sum: Duration = Duration::from_secs(0);
        for i in 0..config::MEASUREMENTS_NUM {
            log::debug!("Measurement {i}");
            time_measurement_sum += measure_delay(&mut actuator, &mut input).await;
            Timer::after(config::DELAY_BETWEEN_MEASUREMENTS).await;
        }
        log::info!(
            "Measurement finished, made {} measurements",
            config::MEASUREMENTS_NUM
        );
        let mean = time_measurement_sum / config::MEASUREMENTS_NUM;
        log::info!("Mean delay is {} ms", mean.as_millis());
        log::info!(
            "Next measurement starts in {} seconds",
            config::DELAY_BETWEEN_MEASUREMENTS_BATCH.as_secs()
        );

        embassy_time::Timer::after(config::DELAY_BETWEEN_MEASUREMENTS_BATCH).await;
    }
}

async fn measure_delay<'d>(actuator: &mut Actuator<'d>, input: &mut Input<'d>) -> Duration {
    actuator.set_off();
    let start = Instant::now();
    actuator.set_on();

    // input.wait_for_on().await;
    // afaics busy loop is allowed here
    while !input.is_on() {}
    let end = Instant::now();
    actuator.set_off();
    end - start
}
