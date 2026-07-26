#![no_std]
#![no_main]

mod logging;
mod spi_receiver;

use crate::spi_receiver::SpiReceiver;
use embassy_executor::Spawner;
#[allow(unused_imports)]
use esp_backtrace as _;
use esp_hal::spi::Mode;

esp_bootloader_esp_idf::esp_app_desc!();

#[esp_rtos::main]
async fn main(_spawner: Spawner) -> ! {
    logging::init_logging(log::LevelFilter::Info);
    log::info!("Initializing receiver...");
    let peripherals = esp_hal::init(esp_hal::Config::default());

    log::info!("Initializing SPI");

    let receiver = SpiReceiver::new();
    let spi_device = esp_hal::spi::slave::Spi::new(peripherals.SPI2, Mode::_0)
        .with_sck(peripherals.GPIO4)
        .with_mosi(peripherals.GPIO5)
        .with_miso(peripherals.GPIO6)
        .with_cs(peripherals.GPIO7)
        .with_dma(peripherals.DMA_CH0);
    log::info!("Listening for the events");

    receiver.loop_receiving(spi_device);
    loop {
        // won't reach here
    }
}
