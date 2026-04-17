#![no_std]
#![no_main]

mod config;
mod input;
mod logging;

use embassy_executor::Spawner;
use embassy_time::{Duration, Instant};
use esp_backtrace as _;
use esp_hal::timer::timg::TimerGroup;

use input::Input;
esp_bootloader_esp_idf::esp_app_desc!();

enum Event {
    Press,
    Release,
    Timeout,
}

#[derive(Default)]
struct EventGenerator {
    last_button_is_pressed: bool,
    timer_alarm_at: Option<Instant>,
}

impl EventGenerator {
    fn generate_event<'d>(&mut self, input: &Input<'d>) -> Option<Event> {
        let current_button_is_pressed = input.is_pressed();
        if !self.last_button_is_pressed && current_button_is_pressed {
            self.last_button_is_pressed = true;
            return Some(Event::Press);
        }

        if self.last_button_is_pressed && !current_button_is_pressed {
            self.last_button_is_pressed = false;
            return Some(Event::Release);
        }
        if self
            .timer_alarm_at
            .is_some_and(|alarm_at| alarm_at <= Instant::now())
        {
            self.timer_alarm_at = None;
            return Some(Event::Timeout);
        }
        None
    }

    fn start_timer(&mut self, duration: Duration) {
        self.timer_alarm_at = Some(Instant::now() + duration)
    }
}

enum State {
    Idle,
    Pressed,
    HeldShort,
    HeldLong,
    Released,
}

#[esp_rtos::main]
async fn main(_spawner: Spawner) -> ! {
    logging::init_logging(log::LevelFilter::Info);
    log::info!("Init...");

    let peripherals = esp_hal::init(esp_hal::Config::default());

    let timg0 = TimerGroup::new(peripherals.TIMG0);
    esp_rtos::start(timg0.timer0);

    let input = Input::new(peripherals.GPIO4, esp_hal::gpio::Pull::Up);
    let mut event_generator = EventGenerator::default();

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
        let mut current_state = State::Idle;

        while Instant::now() - waiting_start_time < config::EXPERIMENT_DURATION {
            let event = event_generator.generate_event(&input);
            if event.is_none() {
                continue;
            }
            let event = event.unwrap();
            match (&current_state, event) {
                (State::Idle, Event::Press) => {
                    current_state = State::Pressed;
                    event_generator.start_timer(config::DEBOUNCE_DURATION);
                }
                (State::Idle, Event::Release) => { /*empty*/ }
                (State::Idle, Event::Timeout) => { /*empty*/ }

                (State::Pressed, Event::Press) => { /*empty*/ }
                (State::Pressed, Event::Release) => { /*empty*/ }
                (State::Pressed, Event::Timeout) => {
                    current_state = State::HeldShort;

                    event_generator.start_timer(config::HELD_LONG_MIN_DURATION);
                }

                (State::HeldShort, Event::Press) => { /*empty*/ }
                (State::HeldShort, Event::Release) => {
                    current_state = State::Released;
                    short_press_num += 1;
                    event_generator.start_timer(config::DEBOUNCE_DURATION);
                }
                (State::HeldShort, Event::Timeout) => {
                    current_state = State::HeldLong;
                }

                (State::HeldLong, Event::Press) => { /*empty*/ }
                (State::HeldLong, Event::Release) => {
                    current_state = State::Released;
                    long_press_num += 1;
                    event_generator.start_timer(config::DEBOUNCE_DURATION);
                }
                (State::HeldLong, Event::Timeout) => { /*empty*/ }

                (State::Released, Event::Press) => { /*empty*/ }
                (State::Released, Event::Release) => { /*empty*/ }
                (State::Released, Event::Timeout) => {
                    current_state = State::Idle;
                }
            }
        }
        log::info!("Number of short presses {}", short_press_num);
        log::info!("Number of long presses {}", long_press_num);
        log::info!(
            "Restarting in {} seconds \n",
            config::DELAY_BETWEEN_MEASUREMENTS_BATCH.as_secs()
        );

        embassy_time::Timer::after(config::DELAY_BETWEEN_MEASUREMENTS_BATCH).await;
    }
}
