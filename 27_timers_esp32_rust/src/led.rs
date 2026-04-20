use esp_hal::gpio::{Output, OutputConfig, OutputPin};

pub struct Led<'d> {
    output: Output<'d>,
}

impl<'d> Led<'d> {
    pub fn new(pin: impl OutputPin + 'd) -> Self {
        Self {
            output: Output::new(pin, esp_hal::gpio::Level::Low, OutputConfig::default()),
        }
    }
    pub fn set_on(&mut self, is_on: bool) {
        if is_on {
            self.output.set_high();
        } else {
            self.output.set_low();
        }
    }
}
