#![no_std]
#![no_main]

mod config;
mod input;
mod logging;
mod states;

use embassy_executor::Spawner;
use embassy_time::Instant;
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

    let input = Input::new(peripherals.GPIO4, esp_hal::gpio::Pull::Up);
    let mut event_generator = states::EventGenerator::default();

    loop {
        let mut short_press_num: u32 = 0;
        let mut long_press_num: u32 = 0;
        let waiting_start_time = Instant::now();
        log::info!("Starting measurement");
        log::info!(
            "Press buttons, for {} seconds",
            config::EXPERIMENT_DURATION.as_secs()
        );
        log::info!("Waiting...");
        let mut current_state = states::State::Idle;

        while waiting_start_time.elapsed() < config::EXPERIMENT_DURATION {
            let event = event_generator.generate_event(&current_state, &input);
            if event.is_none() {
                continue;
            }
            let event = event.unwrap();
            let next_state = current_state.next_state(event);
            if next_state.is_none() {
                continue;
            };
            let next_state = next_state.unwrap();

            if matches!(current_state, states::State::HeldShort(_))
                && matches!(next_state, states::State::Released(_))
            {
                short_press_num += 1;
            }
            if let states::State::HeldLong = current_state
                && let states::State::Released(_) = next_state
            {
                long_press_num += 1;
            }
            current_state = next_state;
        }
        log::info!("Number of short presses {}", short_press_num);
        log::info!("Number of long presses {}", long_press_num);
        log::info!(
            "Restarting in {} seconds",
            config::DELAY_BETWEEN_MEASUREMENTS_BATCH.as_secs()
        );

        embassy_time::Timer::after(config::DELAY_BETWEEN_MEASUREMENTS_BATCH).await;
    }
}
