use defmt::warn;

pub fn log_warn(message: &str, log_entry: &messages::entry::LogEntry) {
    warn!(
        "{} {} {}",
        message,
        defmt::Display2Format(&log_entry.datetime()),
        log_entry.value()
    );
}
pub const fn parse_bool(input: Option<&str>) -> bool {
    if let Some(inp) = input {
        return matches!(inp.as_bytes(), b"true");
    }
    false
}
pub fn parse_u32(input: Option<&str>) -> u32 {
    input.map(|s| s.parse::<u32>().unwrap_or(0)).unwrap_or(0)
}
