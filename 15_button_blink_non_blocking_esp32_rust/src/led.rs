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
    pub fn toggle(&mut self) {
        self.output.toggle();
    }
}
