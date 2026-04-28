use stm32f4xx_hal::gpio::{AnyPin, Input};

pub struct Switch {
    input: AnyPin<Input>,
}

impl Switch {
    pub fn new(input: AnyPin<Input>) -> Self {
        Self { input }
    }
    pub fn is_on(&self) -> bool {
        self.input.is_low()
    }
}
