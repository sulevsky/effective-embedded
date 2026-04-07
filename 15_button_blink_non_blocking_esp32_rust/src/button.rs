use esp_hal::gpio::{Input, InputConfig, InputPin, Pull};

pub struct Button<'d> {
    input: Input<'d>,
}

impl<'d> Button<'d> {
    pub fn new(pin: impl InputPin + 'd, pull: Pull) -> Self {
        let input_config = InputConfig::default().with_pull(pull);
        Self {
            input: Input::new(pin, input_config),
        }
    }
    pub fn is_low(&self) -> bool {
        self.input.is_low()
    }
}
