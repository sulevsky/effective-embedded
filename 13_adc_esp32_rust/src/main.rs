#![no_std]
#![no_main]

use esp_backtrace as _;
use esp_hal::analog::adc::{Adc, AdcCalLine, AdcConfig};
use esp_hal::delay::Delay;
use esp_hal::main;
use esp_println::println;

esp_bootloader_esp_idf::esp_app_desc!();

const VOLTAGE_MAX: u32 = 3300;
const RESOLUTION_MAX: u32 = 4095;

#[main]
fn main() -> ! {
    println!("Init...\r");
    let delay = Delay::new();
    let peripherals = esp_hal::init(esp_hal::Config::default());
    let mut adc_config = AdcConfig::new();

    let mut raw_adc_pin =
        adc_config.enable_pin(peripherals.GPIO7, esp_hal::analog::adc::Attenuation::_11dB);

    let mut calibrated_adc_pin = adc_config.enable_pin_with_cal::<_, AdcCalLine<_>>(
        peripherals.GPIO8,
        esp_hal::analog::adc::Attenuation::_11dB,
    );

    let mut adc = Adc::new(peripherals.ADC1, adc_config);

    loop {
        let raw = adc.read_blocking(&mut raw_adc_pin);
        let calculated = (raw as u32) * VOLTAGE_MAX / RESOLUTION_MAX;
        let calibrated = adc.read_blocking(&mut calibrated_adc_pin);

        println!(
            "Raw: {} of {}, Calculated: {} mV, Calibrated: {} mV, Error: {} mV\r",
            raw,
            RESOLUTION_MAX,
            calculated,
            calibrated,
            calculated as i32 - calibrated as i32
        );
        delay.delay_millis(1000);
    }
}
