use chrono::{DateTime, NaiveDateTime};
use defmt::warn;

pub const SERIALIZED_LOG_ENTRY_SIZE: usize = 10;

#[derive(PartialEq)]
pub struct LogEntry {
    datetime: NaiveDateTime,
    value: u16,
}

impl LogEntry {
    pub fn new(datetime: NaiveDateTime, value: u16) -> Self {
        Self { datetime, value }
    }
    pub fn datetime(&self) -> &NaiveDateTime {
        &self.datetime
    }
    pub fn value(&self) -> u16 {
        self.value
    }
    pub fn deserialize(serialized: &[u8; SERIALIZED_LOG_ENTRY_SIZE]) -> LogEntry {
        let time_part: [u8; 8] = serialized[0..8].try_into().unwrap();
        let datetime = DateTime::from_timestamp_secs(i64::from_le_bytes(time_part))
            .unwrap_or_default()
            .naive_utc();

        let value_part: [u8; 2] = serialized[8..].try_into().unwrap();
        let value: u16 = u16::from_le_bytes(value_part);
        LogEntry { datetime, value }
    }

    pub fn serialize(&self) -> [u8; SERIALIZED_LOG_ENTRY_SIZE] {
        let timestamp_seconds: [u8; 8] = self.datetime.and_utc().timestamp().to_le_bytes();
        let value: [u8; 2] = self.value.to_le_bytes();
        let mut packed = [0u8; SERIALIZED_LOG_ENTRY_SIZE];
        packed[0..8].copy_from_slice(&timestamp_seconds);
        packed[8..].copy_from_slice(&value);
        packed
    }
}
pub fn log_warn(message: &str, log_entry: &LogEntry) {
    warn!(
        "{} {} {}",
        message,
        defmt::Display2Format(&log_entry.datetime()),
        log_entry.value()
    );
}
