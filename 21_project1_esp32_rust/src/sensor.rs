use embassy_sync::{blocking_mutex::raw::CriticalSectionRawMutex, watch::Watch};

use crate::{config::SENSOR_READ_PERIOD_MS, ldr::ReadableSensor};

static CURRENT_SENSOR_VALUE_PCT: Watch<CriticalSectionRawMutex, u8, 1> = Watch::new();
const BUFFER_SIZE: usize = 8;

#[derive(Default)]
struct Buffer {
    next_element: usize,
    is_initialized: bool,
    inner_buffer: [u8; BUFFER_SIZE],
}

impl Buffer {
    fn add(&mut self, value: u8) {
        if !self.is_initialized {
            self.inner_buffer = [value; BUFFER_SIZE];
            self.is_initialized = true;
        } else {
            self.inner_buffer[self.next_element] = value;
        }

        self.next_element = (self.next_element + 1) % BUFFER_SIZE;
    }
    fn mean(&self) -> u8 {
        (self.inner_buffer.iter().map(|&e| e as u32).sum::<u32>() / self.inner_buffer.len() as u32)
            as u8
    }
}

#[embassy_executor::task]
pub async fn sensor_collection(sensor: &'static mut dyn ReadableSensor) {
    log::info!("Started sensor collection");
    let mut buffer = Buffer::default();
    loop {
        buffer.add(sensor.read_calibrated_pct());
        sensor.log();
        CURRENT_SENSOR_VALUE_PCT.sender().send(buffer.mean());
        embassy_time::Timer::after(SENSOR_READ_PERIOD_MS).await;
    }
}
pub async fn current_value() -> u8 {
    CURRENT_SENSOR_VALUE_PCT.receiver().unwrap().get().await
}
