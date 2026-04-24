use embassy_stm32::gpio::Output;
use embassy_time::{Duration, Instant};

pub struct Led<'d> {
    output: Output<'d>,
    is_blinking: bool,
}

impl<'d> Led<'d> {
    pub fn new(output: Output<'d>) -> Self {
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
    pub fn on_timer_event(&mut self, instant: Instant) {
        if !self.is_blinking {
            return;
        }
        let blinking_period_duration = Duration::from_millis(500);
        let duration_from_start_of_period =
            instant.as_millis() % blinking_period_duration.as_millis();
        let on_part_of_period = blinking_period_duration.as_millis() / 2;
        if duration_from_start_of_period < on_part_of_period {
            self.output.set_high();
        } else {
            self.output.set_low();
        }
    }
}
