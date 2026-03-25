#![no_std]
#![no_main]

use esp_backtrace as _;
use esp_hal::delay::Delay;
use esp_hal::gpio::{Output, OutputConfig};
use esp_hal::main;
use esp_println::println;
esp_bootloader_esp_idf::esp_app_desc!();

#[main]
fn main() -> ! {
    println!("Init...");
    let peripherals = esp_hal::init(esp_hal::Config::default());

    let delay = Delay::new();
    let output_config = OutputConfig::default();
    let mut blue_led = Output::new(peripherals.GPIO4, esp_hal::gpio::Level::High, output_config);
    let mut red_led = Output::new(peripherals.GPIO5, esp_hal::gpio::Level::Low, output_config);

    loop {
        blue_led.toggle();
        red_led.toggle();
        delay.delay_millis(500);
    }
}
