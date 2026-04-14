use embassy_time::Duration;

pub const MEASUREMENTS_NUM: u32 = 10;
pub const DELAY_BETWEEN_MEASUREMENTS: Duration = Duration::from_millis(500);
pub const DELAY_BETWEEN_MEASUREMENTS_BATCH: Duration = Duration::from_secs(10);
