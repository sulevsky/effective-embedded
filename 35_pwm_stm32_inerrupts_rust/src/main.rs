#![no_std]
#![no_main]

use cortex_m::delay::Delay;
use defmt::info;
use embedded_hal::pwm::SetDutyCycle;
use stm32f4xx_hal::adc::config::AdcConfig;

use defmt_rtt as _;
use panic_probe as _;
use stm32f4xx_hal::{adc, prelude::*};
use stm32f4xx_hal::{gpio::GpioExt, pac, rcc::RccExt};

#[cortex_m_rt::entry]
fn main() -> ! {
    info!("Init...");
    let cp = cortex_m::Peripherals::take().unwrap();
    let dp = pac::Peripherals::take().unwrap();

    let mut rcc = dp.RCC.constrain();
    let gpioa = dp.GPIOA.split(&mut rcc);

    let mut d = Delay::new(cp.SYST, rcc.clocks.sysclk().raw());
    let (_, (ch1, ch2, _, _)) = dp.TIM1.pwm_us(1.millis(), &mut rcc);
    let mut ch1_led = ch1.with(gpioa.pa8);
    let mut ch2_motor = ch2.with(gpioa.pa9);
    ch1_led.enable();
    ch2_motor.enable();

    let mut adc1_pin_pa0 = gpioa.pa0.into_analog();
    let mut adc1_pin_pa1 = gpioa.pa1.into_analog();
    let adc_config = AdcConfig::default();
    let mut adc1 = adc::Adc::new(dp.ADC1, false, adc_config, &mut rcc);
    adc1.enable();

    loop {
        let led_adc = adc1.convert(&mut adc1_pin_pa0, adc::config::SampleTime::Cycles_112);
        let motor_adc = adc1.convert(&mut adc1_pin_pa1, adc::config::SampleTime::Cycles_112);
        // info!("led: {}, motor: {}", led_adc, motor_adc);
        let led_duty = scale_into_percent(led_adc, 4096);
        ch1_led.set_duty_cycle_percent(led_duty).unwrap();
        let motor_duty = scale_into_percent(motor_adc, 4096);
        ch2_motor.set_duty_cycle_percent(motor_duty).unwrap();
        d.delay_ms(50);
    }
}

fn scale_into_percent(input: u16, max_input: u16) -> u8 {
    (input as u32 * 100 as u32 / max_input as u32) as u8
}
