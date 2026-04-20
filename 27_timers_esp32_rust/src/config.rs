// for experiment minutes replaces with seconds
#[cfg(feature = "debug-timing")]
pub mod values {
    pub const TICK_DURATION: esp_hal::time::Duration = esp_hal::time::Duration::from_secs(1);
    pub const IS_ON_DURATION: esp_hal::time::Duration = esp_hal::time::Duration::from_secs(15);
    pub const CYCLE_DURATION: esp_hal::time::Duration = esp_hal::time::Duration::from_secs(59);
}

#[cfg(not(feature = "debug-timing"))]
pub mod values {
    pub const TICK_DURATION: esp_hal::time::Duration = esp_hal::time::Duration::from_minutes(1);
    pub const IS_ON_DURATION: esp_hal::time::Duration = esp_hal::time::Duration::from_minutes(15);
    pub const CYCLE_DURATION: esp_hal::time::Duration = esp_hal::time::Duration::from_minutes(59);
}
