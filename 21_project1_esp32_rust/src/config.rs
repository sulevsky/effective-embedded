use embassy_time::Duration;
use esp_hal::time::Rate;

pub const LED_TIMER_FREQUENCY: Rate = Rate::from_hz(5000);
pub const LED_UPDATE_PERIOD: Duration = Duration::from_millis(50);

pub const LDR_CALIBRATION_DURATION: Duration = Duration::from_secs(5);
pub const SENSOR_READ_PERIOD_MS: Duration = Duration::from_millis(20);
