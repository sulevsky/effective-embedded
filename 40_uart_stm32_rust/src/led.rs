use stm32f4xx_hal::gpio::{AnyPin, Output, Pin};

pub struct Led {
    output: AnyPin<Output>,
}

impl Led {
    pub fn new(output: AnyPin<Output>) -> Self {
        Self { output }
    }
    pub fn on(&mut self) {
        self.output.set_high();
    }
    pub fn off(&mut self) {
        self.output.set_low();
    }
    pub fn toggle(&mut self) {
        self.output.toggle();
    }
}
