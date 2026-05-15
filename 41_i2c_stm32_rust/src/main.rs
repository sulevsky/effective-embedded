#![no_std]
#![no_main]

// mod led;
mod bme280;
mod ssd1306;

use core::cell::RefCell;

use cortex_m::delay::Delay;
use defmt::info;

use defmt_rtt as _;
use panic_probe as _;
use stm32f4xx_hal::{
    gpio::GpioExt,
    i2c::{I2cExt, Mode},
    prelude::*,
    serial,
};

use crate::{bme280::UncalibratedSensor, ssd1306::UninitializedDisplay};

// use ssd1306::mode::DisplayConfig;
// use ssd1306::rotation::DisplayRotation;
// use ssd1306::size::DisplaySize128x64;
// use ssd1306::{I2CDisplayInterface, Ssd1306};
// use stm32f1xx_hal::gpio::GpioExt;
// use stm32f1xx_hal::pac;
// use stm32f1xx_hal::rcc::RccExt;

// use crate::led::Led;

#[cortex_m_rt::entry]
fn main() -> ! {
    info!("Starting initialization");
    let cp = cortex_m::Peripherals::take().unwrap();
    let dp = stm32f4xx_hal::pac::Peripherals::take().unwrap();
    let mut rcc = dp.RCC.constrain();
    let gpioa = dp.GPIOA.split(&mut rcc);
    let gpiob = dp.GPIOB.split(&mut rcc);
    let mut internal_led = gpioa.pa5.into_push_pull_output();

    let mut delay = Delay::new(cp.SYST, rcc.clocks.sysclk().raw());

    let i2c = dp
        .I2C1
        .i2c((gpiob.pb6, gpiob.pb7), Mode::standard(100.kHz()), &mut rcc);
    let i2c_cell = &RefCell::new(i2c);
    let mut baro_sensor = UncalibratedSensor::new(i2c_cell).calibrate(0);
    let mut display = UninitializedDisplay::new(i2c_cell).init();
    display.clear();

    info!("Finished initialization");
    info!("Running");
    loop {
        info!("---------------");
        let sensor_data = baro_sensor.read_sensor();
        info!("temperature {}", sensor_data.temperature as f64 / 100.0);
        display.print(sensor_data.temperature as f64 / 100.0);
        info!("---------------");

        internal_led.set_high();
        delay.delay_ms(500);
        internal_led.set_low();
        delay.delay_ms(500);
    }
}
