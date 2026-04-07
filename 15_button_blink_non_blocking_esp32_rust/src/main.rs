#![no_std]
#![no_main]

mod button;
mod config;
mod led;

use button::Button;
use esp_backtrace as _;
use esp_hal::main;
use esp_hal::time::{Duration, Instant};
use esp_println::println;
use led::Led;

esp_bootloader_esp_idf::esp_app_desc!();

#[main]
fn main() -> ! {
    println!("Init...");
    let peripherals = esp_hal::init(esp_hal::Config::default());

    let mut blue_led = Led::new(peripherals.GPIO4);
    blue_led.toggle();
    let mut red_led = Led::new(peripherals.GPIO5);

    let external_button = Button::new(peripherals.GPIO19, esp_hal::gpio::Pull::Up);
    let boot_button = Button::new(peripherals.GPIO0, esp_hal::gpio::Pull::Up);

    let mut period = config::SLOW_PERIOD;
    let mut last_toggle_at = Instant::now();

    let mut external_button_pressed_at =
        Instant::EPOCH - config::DEBOUNCING_TIME - Duration::from_millis(1);
    let mut boot_button_pressed_at =
        Instant::EPOCH - config::DEBOUNCING_TIME - Duration::from_millis(1);

    loop {
        let now = Instant::now();
        if now - external_button_pressed_at >= config::DEBOUNCING_TIME && external_button.is_low() {
            period = config::FAST_PERIOD;
            external_button_pressed_at = now;
            println!("External button is pressed")
        }
        if now - boot_button_pressed_at >= config::DEBOUNCING_TIME && boot_button.is_low() {
            period = config::SLOW_PERIOD;
            boot_button_pressed_at = now;
            println!("Boot button is pressed")
        }
        if now - last_toggle_at >= period {
            blue_led.toggle();
            red_led.toggle();
            last_toggle_at = now;
        }
    }
}
