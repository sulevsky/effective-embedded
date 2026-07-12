use crate::entry::LogEntry;

pub struct SpiSender;

impl SpiSender {
    pub fn new() -> Self {
        Self
    }

    pub fn send<T: embedded_hal::spi::SpiBus>(&self, spi_bus: &mut T, entry: &LogEntry) {
        let serialized_entry = entry.serialize();
        spi_bus.write(&serialized_entry).unwrap();
    }
}
