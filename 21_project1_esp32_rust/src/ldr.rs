use embassy_time::Instant;
use esp_backtrace as _;
use esp_hal::Blocking;
use esp_hal::analog::adc::CalibrationAccess;
use esp_hal::analog::adc::{Adc, AdcCalScheme, AdcChannel, AdcConfig, AdcPin, RegisterAccess};
use esp_hal::gpio::AnalogPin;

use crate::config::{LDR_CALIBRATION_DURATION, SENSOR_READ_PERIOD_MS};

const ATTENUATION: esp_hal::analog::adc::Attenuation = esp_hal::analog::adc::Attenuation::_11dB;

// for ADC_ATTEN_DB_12, see ESP-IDF doc for conversion table
const DEFAULT_VOLTAGE_MIN_MV: u16 = 0;
const DEFAULT_VOLTAGE_MAX_MV: u16 = 3100;

const PCT_MAX: u8 = 100;

pub struct Sensor<'d, ADCI, PIN, CALIBRATEDPIN, CS>
where
    ADCI: RegisterAccess + CalibrationAccess + 'd,
    CS: AdcCalScheme<ADCI>,
{
    adc: Adc<'d, ADCI, Blocking>,
    adc_pin: AdcPin<PIN, ADCI, ()>,
    calibrated_adc_pin: AdcPin<CALIBRATEDPIN, ADCI, CS>,
    calibrated_voltage_min: u16,
    calibrated_voltage_max: u16,
}

impl<'d, ADCI, PIN, CALIBRATEDPIN, CS> Sensor<'d, ADCI, PIN, CALIBRATEDPIN, CS>
where
    ADCI: RegisterAccess + CalibrationAccess + 'd,
    PIN: AnalogPin + AdcChannel,
    CALIBRATEDPIN: AnalogPin + AdcChannel,
    CS: AdcCalScheme<ADCI>,
{
    pub fn new(adc: ADCI, pin: PIN, calibrated_pin: CALIBRATEDPIN) -> Self {
        let mut adc_config: AdcConfig<ADCI> = AdcConfig::new();
        let adc_pin = adc_config.enable_pin(pin, ATTENUATION);

        let calibrated_adc_pin = adc_config.enable_pin_with_cal::<CALIBRATEDPIN, CS>(
            calibrated_pin,
            esp_hal::analog::adc::Attenuation::_11dB,
        );

        Self {
            adc: Adc::new(adc, adc_config),
            adc_pin,
            calibrated_adc_pin,
            calibrated_voltage_min: DEFAULT_VOLTAGE_MIN_MV,
            calibrated_voltage_max: DEFAULT_VOLTAGE_MAX_MV,
        }
    }
    pub async fn calibrate(&mut self) {
        log::info!("Calibration started");
        self.calibrated_voltage_min = u16::MAX;
        self.calibrated_voltage_max = u16::MIN;
        let calibration_end = Instant::now() + LDR_CALIBRATION_DURATION;
        while Instant::now() < calibration_end {
            let calibrated_value = self.adc.read_blocking(&mut self.calibrated_adc_pin);
            if calibrated_value < self.calibrated_voltage_min {
                self.calibrated_voltage_min = calibrated_value;
            }
            if calibrated_value > self.calibrated_voltage_max {
                self.calibrated_voltage_max = calibrated_value;
            }
            embassy_time::Timer::after(SENSOR_READ_PERIOD_MS).await;
        }

        log::info!(
            "Calibration finished, min: {}, max: {}",
            self.calibrated_voltage_min,
            self.calibrated_voltage_max
        );
    }
    fn mv_to_pct(&self, milli_volts: u16) -> u8 {
        if milli_volts <= self.calibrated_voltage_min {
            return 0;
        }
        if milli_volts >= self.calibrated_voltage_max {
            return PCT_MAX;
        }
        ((milli_volts - self.calibrated_voltage_min) as u32 * 100
            / (self.calibrated_voltage_max - self.calibrated_voltage_min) as u32) as u8
    }
}

pub trait ReadableSensor {
    fn read_raw(&mut self) -> u16;
    fn read_calibrated_pct(&mut self) -> u8;
    fn log(&mut self);
}

impl<'d, ADCI, PIN, CALIBRATEDPIN, CS> ReadableSensor for Sensor<'d, ADCI, PIN, CALIBRATEDPIN, CS>
where
    ADCI: RegisterAccess + CalibrationAccess + 'd,
    PIN: AnalogPin + AdcChannel,
    CALIBRATEDPIN: AnalogPin + AdcChannel,
    CS: AdcCalScheme<ADCI>,
{
    fn read_raw(&mut self) -> u16 {
        self.adc.read_blocking(&mut self.adc_pin)
    }

    fn read_calibrated_pct(&mut self) -> u8 {
        let calibrated_value = self.adc.read_blocking(&mut self.calibrated_adc_pin);
        self.mv_to_pct(calibrated_value)
    }
    fn log(&mut self) {
        log::info!("Raw: {}, bytes", self.read_raw());
        log::info!("Calibrated: {}, %", self.read_calibrated_pct());
    }
}
