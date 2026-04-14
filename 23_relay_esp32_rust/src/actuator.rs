use esp_hal::gpio::{Output, OutputConfig, OutputPin};

pub struct Actuator<'d> {
    output: Output<'d>,
}

impl<'d> Actuator<'d> {
    pub fn new(pin: impl OutputPin + 'd) -> Self {
        Self {
            output: Output::new(pin, esp_hal::gpio::Level::Low, OutputConfig::default()),
        }
    }
    pub fn set_on(&mut self) {
        self.output.set_low();
    }
    pub fn set_off(&mut self) {
        self.output.set_high();
    }
}
