#![no_std]
#![no_main]

mod led;
// mod ssd1306;
// mod bm280;

use cortex_m::delay::Delay;
use defmt::info;

use defmt_rtt as _;
use panic_probe as _;
use ssd1306::mode::DisplayConfig;
use ssd1306::rotation::DisplayRotation;
use ssd1306::size::DisplaySize128x64;
use ssd1306::{I2CDisplayInterface, Ssd1306};
use stm32f1xx_hal::gpio::GpioExt;
use stm32f1xx_hal::pac;
use stm32f1xx_hal::rcc::RccExt;

use crate::led::Led;

#[cortex_m_rt::entry]
fn main() -> ! {
    info!("Init...");
    let cp = cortex_m::Peripherals::take().unwrap();
    let dp = pac::Peripherals::take().unwrap();

    // let mut rcc = dp.RCC.constrain();
    // let gpioa = dp.GPIOA.split(&mut rcc);
    // let gpiob = dp.GPIOB.split(&mut rcc);

    // let mut delay = Delay::new(cp.SYST, rcc.clocks.sysclk().raw());
    // let mut remote_led = Led::new(gpioa.pa5.into_push_pull_output().erase());
    // let mut local_led = Led::new(gpioa.pa8.into_push_pull_output().erase());

    // let mut i2c = dp
    //     .I2C1
    //     .i2c((gpiob.pb6, gpiob.pb7), Mode::from(100.kHz()), &mut rcc);

    // let mut display = Display::new(i2c);
    // display.exec();
    // let interface = I2CDisplayInterface::new(i2c);
    // let mut display =
    //     Ssd1306::new(interface, DisplaySize128x64, DisplayRotation::Rotate0).into_terminal_mode();
    // // todo vova - panics
    // display.init().unwrap();
    // display.clear().unwrap();
    // for i in 0..255u8 {
    //     display.print_char(i as char).unwrap();
    //     delay.delay_ms(50);
    // }

    // let init_commands = [
    //     0xAE, // DISPLAY OFF
    //     0xD5, 0x80, // SET DISPLAY CLOCK DIV RATIO
    //     0xA8, 0x3F, // SET MULTIPLEX RATIO
    //     0xD3, 0x00, // SET DISPLAY OFFSET
    //     0x40, // SET START LINE
    //     0x8D, 0x14, // ENABLE CHARGE PUMP
    //     0x20, 0x00, // SET MEMORY ADDRESSING MODE (horizontal)
    //     0xA1, // SET COM OUTPUT SCAN DIRECTION
    //     0xC8, // SET COM PINS HARDWARE CONFIGURATION
    //     0xDA, 0x12, // SET COM OUTPUT SCAN DIRECTION
    //     0x81, 0xCF, // SET CONTRAST CONTROL
    //     0xD9, 0xF1, // SET PRE-CHARGE PERIOD
    //     0xDB, 0x40, // SET VCOMH DESELECT LEVEL
    //     0xA4, // ENTIRE DISPLAY ON
    //     0xA6, // NORMAL DISPLAY (not inverted)
    //     0xAF, // DISPLAY ON
    // ];
    // for c in init_commands {
    //     i2c.write(display_addr, &[c]).unwrap();
    //     delay.delay_ms(10);
    // }

    // i2c.write(display_addr, &[control_register_addr, 0xA4])
    //     .unwrap();
    loop {
        // i2c.write_read(display_addr, &register_addr, &mut buf)
        //     .unwrap();
        info!("Read");
        // delay.delay_ms(500);
    }
}
