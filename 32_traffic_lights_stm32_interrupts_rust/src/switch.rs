use embassy_stm32::gpio::Input;

pub struct Switch<'d> {
    input: Input<'d>,
}

impl<'d> Switch<'d> {
    pub fn new(input: Input<'d>) -> Self {
        Self { input }
    }
    pub fn is_on(&self) -> bool {
        self.input.is_low()
    }
}
