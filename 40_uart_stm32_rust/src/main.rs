#![no_std]
#![no_main]

mod button;
mod led;

use cortex_m::delay::Delay;
use defmt::info;

use defmt_rtt as _;
use panic_probe as _;
use stm32f4xx_hal::prelude::{_embedded_hal_serial_nb_Read, _embedded_hal_serial_nb_Write};
use stm32f4xx_hal::serial::{self, SerialExt};
use stm32f4xx_hal::{gpio::GpioExt, pac, rcc::RccExt};

use crate::button::Button;
use crate::led::Led;

#[cortex_m_rt::entry]
fn main() -> ! {
    info!("Init...");
    let cp = cortex_m::Peripherals::take().unwrap();
    let dp = pac::Peripherals::take().unwrap();

    let mut rcc = dp.RCC.constrain();
    let gpioa = dp.GPIOA.split(&mut rcc);

    let mut delay = Delay::new(cp.SYST, rcc.clocks.sysclk().raw());
    let mut remote_led = Led::new(gpioa.pa5.into_push_pull_output().erase());
    let mut local_led = Led::new(gpioa.pa8.into_push_pull_output().erase());

    let mut button = Button::new(gpioa.pa6.into_pull_up_input().erase());

    let (mut tx, mut rx) = dp
        .USART1
        .serial::<u8>((gpioa.pa9, gpioa.pa10), serial::Config::default(), &mut rcc)
        .unwrap()
        .split();

    loop {
        if button.is_pressed() {
            info!("Button is pressed");
            tx.write('x' as u8).unwrap();
            local_led.toggle();
        }

        if let Some(read) = rx.read().ok() {
            info!("Read {}", read as char);
            remote_led.toggle();
        }
        delay.delay_ms(5);
    }
}
