use embedded_hal::digital::StatefulOutputPin;

pub struct Led<PIN> {
    output: PIN,
}

impl<PIN: StatefulOutputPin> Led<PIN> {
    pub fn new(output: PIN) -> Self {
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
