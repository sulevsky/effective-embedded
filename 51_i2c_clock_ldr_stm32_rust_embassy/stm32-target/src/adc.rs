use crate::Irqs;
use cortex_m::singleton;
use defmt::{info, warn};
use embassy_stm32::Peri;
use embassy_stm32::adc::{AdcChannel, CONTINUOUS, Exten, RingBufferedAdc, SampleTime};
use embassy_stm32::{Peripherals, bind_interrupts};

const ADC_BUF_SIZE: usize = 1024;
const READ_BUF_SIZE: usize = ADC_BUF_SIZE / 2;

pub struct Adc<'d> {
    adc: RingBufferedAdc<'d, embassy_stm32::peripherals::ADC1>,
}

impl<'d> Adc<'d> {
    pub fn new(
        dma: Peri<'d, embassy_stm32::peripherals::DMA2_CH0>,
        dp_adc: Peri<'d, embassy_stm32::peripherals::ADC1>,
        adc_pin: Peri<'d, embassy_stm32::peripherals::PA0>,
    ) -> Self {
        let adc_data: &mut [u16; ADC_BUF_SIZE] =
            singleton!(ADCDAT : [u16; ADC_BUF_SIZE] = [0u16; ADC_BUF_SIZE]).unwrap();
        let mut adc = embassy_stm32::adc::Adc::new_with_config(dp_adc, Default::default());
        let mut adc = adc.into_ring_buffered(
            dma,
            adc_data,
            Irqs,
            [(adc_pin.degrade_adc(), SampleTime::CYCLES480)].into_iter(),
            CONTINUOUS,
            Exten::DISABLED,
        );
        let _ = adc.start();
        Self { adc }
    }
    pub async fn measure(&mut self) -> u16 {
        self.adc.clear();
        let mut buffer = [0u16; READ_BUF_SIZE];
        match self.adc.read(&mut buffer).await {
            Ok(_data) => {
                (buffer.iter().map(|&x| x as u32).sum::<u32>() / READ_BUF_SIZE as u32) as u16
            }
            Err(e) => {
                warn!("ADC Error: {:?}", e);
                0
            }
        }
    }
}
