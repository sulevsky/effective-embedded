#![no_std]
#![no_main]

mod led;

use cortex_m::delay::Delay;
use defmt::info;

use defmt_rtt as _;
use panic_probe as _;
use ssd1306::mode::DisplayConfig;
use ssd1306::rotation::DisplayRotation;
use ssd1306::size::DisplaySize128x64;
use ssd1306::{I2CDisplayInterface, Ssd1306};
use stm32f4xx_hal::dma::traits::PeriAddress;
use stm32f4xx_hal::i2c::Mode;
use stm32f4xx_hal::serial::{self, SerialExt};
use stm32f4xx_hal::{gpio::GpioExt, pac, rcc::RccExt};
use stm32f4xx_hal::{i2c, prelude::*};

use crate::led::Led;

#[cortex_m_rt::entry]
fn main() -> ! {
    info!("Init...");
    let cp = cortex_m::Peripherals::take().unwrap();
    let dp = pac::Peripherals::take().unwrap();

    let mut rcc = dp.RCC.constrain();
    let gpioa = dp.GPIOA.split(&mut rcc);
    let gpiob = dp.GPIOB.split(&mut rcc);

    let mut delay = Delay::new(cp.SYST, rcc.clocks.sysclk().raw());
    let mut remote_led = Led::new(gpioa.pa5.into_push_pull_output().erase());
    let mut local_led = Led::new(gpioa.pa8.into_push_pull_output().erase());

    let mut i2c = dp
        .I2C1
        .i2c((gpiob.pb6, gpiob.pb7), Mode::from(100.kHz()), &mut rcc);

    let interface = I2CDisplayInterface::new(i2c);
    let mut display = Ssd1306::new(interface, DisplaySize128x64, DisplayRotation::Rotate0)
        .into_terminal_mode();
    // todo vova - panics
    display.init().unwrap();
    // display.draw(&[120]).unwrap();


    // let display_addr = 0x22u8;
    // let register_addr = [0x22; 1];
    // let mut buf = [0u8; 1];
    loop {
        // i2c.write_read(display_addr, &register_addr, &mut buf)
        //     .unwrap();
        // info!("Read {:b}", buf[0]);
        delay.delay_ms(500);
    }
}
