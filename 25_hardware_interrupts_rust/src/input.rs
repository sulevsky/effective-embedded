use esp_hal::gpio::{InputConfig, InputPin, Pull};

pub struct Input<'d> {
    input: esp_hal::gpio::Input<'d>,
}

impl<'d> Input<'d> {
    pub fn new(pin: impl InputPin + 'd, pull: Pull) -> Self {
        let input_config = InputConfig::default().with_pull(pull);
        Self {
            input: esp_hal::gpio::Input::new(pin, input_config),
        }
    }

    pub fn is_pressed(&self) -> bool {
        self.input.is_low()
    }

    #[allow(dead_code)]
    pub async fn wait_for_on(&mut self) {
        self.input.wait_for_low().await
    }
}
