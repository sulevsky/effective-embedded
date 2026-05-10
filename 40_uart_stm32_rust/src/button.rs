use debouncr::{Debouncer, DebouncerStateful, Repeat8, debounce_8};
use stm32f4xx_hal::gpio::{AnyPin, Input};

pub struct Button {
    input: AnyPin<Input>,
    button: Debouncer<u8, Repeat8>,
}

impl Button {
    pub fn new(input: AnyPin<Input>) -> Self {
        Self {
            input,
            button: debounce_8(false),
        }
    }

    pub fn is_pressed(&mut self) -> bool {
        let raw = *&self.input.is_low(); // read raw GPIO
        if let Some(edge) = &self.button.update(raw) {
            match edge {
                debouncr::Edge::Rising => false,
                debouncr::Edge::Falling => true,
            }
        }else {
            false
        }
    }
}
