use esp_hal::{
    gpio::OutputPin,
    ledc::{
        Ledc, LowSpeed,
        channel::{Channel, ChannelIFace, config::Config},
        timer::Timer,
    },
};

const INITIAL_DUTY: u8 = 0;
const FADE_DURATION_MS: u16 = 2;
pub struct Led<'d> {
    current_duty_pct: u8,
    channel: Channel<'d, LowSpeed>,
}

impl<'d> Led<'d> {
    pub fn new(pin: impl OutputPin + 'd, ledc: Ledc<'d>, timer: &'d Timer<'d, LowSpeed>) -> Self {
        let mut channel = ledc.channel::<LowSpeed>(esp_hal::ledc::channel::Number::Channel0, pin);

        let ch_config = Config {
            timer,
            duty_pct: INITIAL_DUTY,
            drive_mode: esp_hal::gpio::DriveMode::PushPull,
        };
        channel.configure(ch_config).unwrap();
        Self {
            channel,
            current_duty_pct: INITIAL_DUTY,
        }
    }
    pub fn update(&mut self, duty_pct: u8) {
        if duty_pct != self.current_duty_pct {
            self.channel
                .start_duty_fade(self.current_duty_pct, duty_pct, FADE_DURATION_MS)
                .unwrap();
            self.channel.set_duty(duty_pct).unwrap();
            self.current_duty_pct = duty_pct;
        }
    }
}
