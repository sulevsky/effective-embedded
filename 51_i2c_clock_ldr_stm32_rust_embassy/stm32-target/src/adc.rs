use embassy_stm32::Peri;
use embassy_stm32::adc::SampleTime;

pub struct Adc<'d> {
    adc: embassy_stm32::adc::Adc<'d, embassy_stm32::peripherals::ADC1>,
    adc_pin: Peri<'d, embassy_stm32::peripherals::PA0>,
}

impl<'d> Adc<'d> {
    pub fn new(
        dp_adc: Peri<'d, embassy_stm32::peripherals::ADC1>,
        adc_pin: Peri<'d, embassy_stm32::peripherals::PA0>,
    ) -> Self {
        let mut adc = embassy_stm32::adc::Adc::new(dp_adc);
        let _ = adc.enable_vrefint();

        Self { adc, adc_pin }
    }
    pub fn measure(&mut self) -> u16 {
        self.adc
            .blocking_read(&mut self.adc_pin, SampleTime::CYCLES112)
    }
}
