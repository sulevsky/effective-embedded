#![no_std]
#![no_main]

use cortex_m::delay::Delay;
use defmt::info;

use defmt_rtt as _;
use panic_probe as _;
use stm32f4xx_hal::{
    adc::{Adc, config::AdcConfig},
    gpio::GpioExt,
    prelude::*,
};

#[cortex_m_rt::entry]
fn main() -> ! {
    info!("Starting initialization");
    let cp = cortex_m::Peripherals::take().unwrap();
    let dp = stm32f4xx_hal::pac::Peripherals::take().unwrap();
    let mut rcc = dp.RCC.constrain();
    let gpioa = dp.GPIOA.split(&mut rcc);
    let mut internal_led = gpioa.pa5.into_push_pull_output();

    let adc_config = AdcConfig::default()
        .clock(stm32f4xx_hal::adc::config::Clock::Pclk2_div_4)
        .continuous(stm32f4xx_hal::adc::config::Continuous::Continuous);
    let mut adc = Adc::new(dp.ADC1, true, adc_config, &mut rcc);
    let adc_pin = gpioa.pa0.into_analog();
    adc.configure_channel(
        &adc_pin,
        stm32f4xx_hal::adc::config::Sequence::One,
        stm32f4xx_hal::adc::config::SampleTime::Cycles_480,
    );
    adc.start_conversion();
    let mut delay = Delay::new(cp.SYST, rcc.clocks.sysclk().raw());

    info!("Finished initialization");
    info!("Running");
    loop {
        info!("---------------");
        let raw_adc_value = adc.current_sample();
        let coverted_adc_value = adc.sample_to_millivolts(raw_adc_value);
        info!("raw: {} bits, converted: {} mv", raw_adc_value, coverted_adc_value);
        internal_led.set_high();
        delay.delay_ms(50);
        internal_led.set_low();
        delay.delay_ms(50);
    }
}
