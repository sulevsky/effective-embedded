use esp_hal::time::Duration;

pub const FAST_PERIOD: Duration = Duration::from_millis(100);
pub const SLOW_PERIOD: Duration = Duration::from_millis(2000);

pub const DEBOUNCING_TIME: Duration = Duration::from_millis(200);
