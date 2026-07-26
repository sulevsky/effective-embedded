use crate::entry::LogEntry;

pub struct SpiSender;

impl SpiSender {
    pub fn new() -> Self {
        Self
    }

    pub async fn send<T: embedded_hal_async::spi::SpiBus, P: embedded_hal::digital::OutputPin>(
        &self,
        spi_bus: &mut T,
        cs: &mut P,
        entry: &LogEntry,
    ) {
        let serialized_entry = entry.serialize();
        cs.set_low().unwrap();
        spi_bus.write(&serialized_entry).await.unwrap();
        cs.set_high().unwrap();
    }
}
