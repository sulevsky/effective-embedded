use core::time::Duration;

use stm32f4xx_hal::gpio::{AnyPin, Output};

pub struct Led {
    output: AnyPin<Output>,
    is_blinking: bool,
}

impl Led {
    pub fn new(output: AnyPin<Output>) -> Self {
        Self {
            output,
            is_blinking: false,
        }
    }
    pub fn on(&mut self) {
        self.is_blinking = false;
        self.output.set_high();
    }
    pub fn off(&mut self) {
        self.is_blinking = false;
        self.output.set_low();
    }
    pub fn blinking(&mut self) {
        self.is_blinking = true;
    }
    pub fn on_timer_event(&mut self, now: Duration) {
        if !self.is_blinking {
            return;
        }
        let blinking_period_duration_ms = 500;
        let duration_from_start_of_period = now.as_millis() % blinking_period_duration_ms;
        let on_part_of_period = blinking_period_duration_ms / 2;
        if duration_from_start_of_period < on_part_of_period {
            self.output.set_high();
        } else {
            self.output.set_low();
        }
    }
}
