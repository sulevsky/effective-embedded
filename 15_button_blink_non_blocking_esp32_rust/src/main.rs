#![no_std]
#![no_main]

use esp_backtrace as _;
use esp_hal::delay::Delay;
use esp_hal::gpio::{Input, InputConfig, Output, OutputConfig};
use esp_hal::main;
use esp_println::println;
esp_bootloader_esp_idf::esp_app_desc!();

const FAST_PERIOD: u32 = 100;
const SLOW_PERIOD: u32 = 2000;

const DEBOUNCING_TIME: u32 = 200;

const LOOP_STEP: u32 = 10;
#[main]
fn main() -> ! {
    println!("Init...");
    let peripherals = esp_hal::init(esp_hal::Config::default());

    let delay = Delay::new();
    let output_config = OutputConfig::default();
    let mut blue_led = Output::new(peripherals.GPIO4, esp_hal::gpio::Level::High, output_config);
    let mut red_led = Output::new(peripherals.GPIO5, esp_hal::gpio::Level::Low, output_config);

    let input_config = InputConfig::default().with_pull(esp_hal::gpio::Pull::Up);
    let external_button = Input::new(peripherals.GPIO19, input_config);
    let boot_button = Input::new(peripherals.GPIO0, input_config);

    let mut period = SLOW_PERIOD;
    let mut current_is_first = true;
    let mut counter = 1u32;

    let mut external_button_unavailable_until = 0u32;
    let mut boot_button_unavailable_until = 0u32;

    loop {
        if external_button_unavailable_until < counter && external_button.is_low() {
            period = FAST_PERIOD;
            external_button_unavailable_until = counter + DEBOUNCING_TIME;
            println!("External button is pressed")
        }
        if boot_button_unavailable_until < counter && boot_button.is_low() {
            period = SLOW_PERIOD;
            boot_button_unavailable_until = counter + DEBOUNCING_TIME;
            println!("Boot button is pressed")
        }
        let next_is_first = is_first(counter, period * 2, period);
        if current_is_first != next_is_first {
            blue_led.toggle();
            red_led.toggle();
            current_is_first = next_is_first;
        }
        delay.delay_millis(LOOP_STEP);
        counter += LOOP_STEP;
    }
}

fn is_first(counter: u32, full_period: u32, first_period: u32) -> bool {
    first_period >= (counter % full_period)
}
