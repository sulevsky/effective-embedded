use embassy_time::Instant;

use crate::{config, input::Input};

#[derive(Debug)]
pub enum Event {
    Press,
    Release,
    Timeout,
}

#[derive(Default)]
pub struct EventGenerator {
    last_button_is_pressed: bool,
}

impl EventGenerator {
    pub fn generate_event<'d>(&mut self, current_state: &State, input: &Input<'d>) -> Option<Event> {
        let current_button_is_pressed = input.is_pressed();
        if !self.last_button_is_pressed && current_button_is_pressed {
            self.last_button_is_pressed = true;
            return Some(Event::Press);
        }

        if self.last_button_is_pressed && !current_button_is_pressed {
            self.last_button_is_pressed = false;
            return Some(Event::Release);
        }
        match current_state {
            State::Pressed(alarm_at) | State::HeldShort(alarm_at) | State::Released(alarm_at) => {
                if alarm_at <= &Instant::now() {
                    return Some(Event::Timeout);
                }
            }
            _ => {}
        }

        None
    }
}

#[derive(Debug)]
pub enum State {
    Idle,
    Pressed(Instant),
    HeldShort(Instant),
    HeldLong,
    Released(Instant),
}

impl State {
    pub fn next_state(&self, event: Event) -> Option<State> {
        match (self, event) {
            (State::Idle, Event::Press) => {
                Some(State::Pressed(Instant::now() + config::DEBOUNCE_DURATION))
            }
            (State::Pressed(_), Event::Timeout) => Some(State::HeldShort(
                Instant::now() + config::HELD_LONG_MIN_DURATION,
            )),
            (State::HeldShort(_), Event::Release) => {
                Some(State::Released(Instant::now() + config::DEBOUNCE_DURATION))
            }
            (State::HeldShort(_), Event::Timeout) => Some(State::HeldLong),

            (State::HeldLong, Event::Release) => {
                Some(State::Released(Instant::now() + config::DEBOUNCE_DURATION))
            }
            (State::Released(_), Event::Timeout) => Some(State::Idle),

            _ => None,
        }
    }
}
