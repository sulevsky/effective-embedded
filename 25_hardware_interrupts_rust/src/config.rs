use embassy_time::Duration;

pub const EXPERIMENT_DURATION: Duration = Duration::from_secs(10);
pub const DEBOUNCE_DURATION: Duration = Duration::from_millis(50);
pub const HELD_LONG_MIN_DURATION: Duration = Duration::from_secs(2);
pub const DELAY_BETWEEN_MEASUREMENTS_BATCH: Duration = Duration::from_secs(10);
