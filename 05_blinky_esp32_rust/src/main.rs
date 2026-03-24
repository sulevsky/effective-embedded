#![no_std]
#![no_main]

use core::u16;
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
    let mut output = Output::new(
        peripherals.GPIO17,
        esp_hal::gpio::Level::High,
        output_config,
    );

    loop {
        output.set_high();
        delay.delay_millis(2000);
        output.set_low();
        delay.delay_millis(1000);
    }
}
