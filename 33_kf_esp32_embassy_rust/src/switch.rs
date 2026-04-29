use esp_hal::gpio::{Input, InputConfig, InputPin, Pull};

pub struct Switch<'d> {
    input: Input<'d>,
}

impl<'d> Switch<'d> {
    pub fn new(pin: impl InputPin + 'd, pull: Pull) -> Self {
        let input_config = InputConfig::default().with_pull(pull);
        Self {
            input: Input::new(pin, input_config),
        }
    }
    pub fn is_on(&self) -> bool {
        self.input.is_low()
    }
}
