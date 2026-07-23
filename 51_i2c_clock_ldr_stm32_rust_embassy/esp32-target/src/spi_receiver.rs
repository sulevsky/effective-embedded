use esp_hal::dma::{DmaRxBuf, DmaTxBuf};
use esp_hal::spi::slave::dma::SpiDma;
use esp_hal::{Blocking, dma_buffers};
use log::{debug, info};
use messages::entry;
use messages::entry::LogEntry;

pub struct SpiReceiver;

impl SpiReceiver {
    pub fn new() -> Self {
        Self
    }

    pub fn loop_receiving(&self, mut spi_device: SpiDma<Blocking>) {
        let (rx_buffer, rx_descriptors, _, _) = dma_buffers!(32000, 0);
        let mut dma_rx_buf = DmaRxBuf::new(rx_descriptors, rx_buffer).unwrap();
        loop {
            let (returned_spi_device, returned_dma_rx_buf) =
                spi_device.read(10, dma_rx_buf).unwrap().wait();
            let chunks = returned_dma_rx_buf.received_data();

            for chunk in chunks {
                match parsed(chunk) {
                    Ok(entry) => {
                        info!("Received an event: {:?}", entry);
                    }
                    Err(msg) => {
                        debug!("Could not process event: {}, event: {:?}", msg, chunk);
                    }
                }
            }
            spi_device = returned_spi_device;
            dma_rx_buf = returned_dma_rx_buf;
        }
    }
}

fn parsed(chunk: &[u8]) -> Result<LogEntry, &str> {
    if chunk.len() != entry::SERIALIZED_LOG_ENTRY_SIZE {
        return Err("Received message size is not correct");
    }
    let array: Result<[u8; 10], _> = chunk.try_into();
    if array.is_err() {
        return Err("Could not transform to an array");
    }
    Ok(LogEntry::deserialize(&array.unwrap()))
}
