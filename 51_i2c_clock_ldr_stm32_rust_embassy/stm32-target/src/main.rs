#![no_std]
#![no_main]
mod adc;
mod clock;
mod memory;
mod spi_sender;
mod utils;

use crate::utils::{parse_bool, parse_u32};
use chrono::NaiveDateTime;
use defmt::{debug, info};
use defmt_rtt as _;
use embassy_executor::Spawner;
use embassy_stm32::gpio::{Level, Output, Speed};
use embassy_stm32::i2c::{Config, I2c};
use embassy_stm32::{bind_interrupts, dma, i2c, peripherals};
use embassy_time::Timer;
use messages::entry;
use panic_probe as _;

const INIT_DATE_TIME: Option<&str> = option_env!("INIT_DATE_TIME");
const CLEAR_MEMORY: bool = parse_bool(option_env!("CLEAR_MEMORY"));
const PRINT_REPORT: bool = parse_bool(option_env!("PRINT_REPORT"));
const CREATE_DEMO_DATA: bool = parse_bool(option_env!("CREATE_DEMO_DATA"));
const COLLECT_NEW_DATA: bool = parse_bool(option_env!("COLLECT_NEW_DATA"));
const SEND_DEMO_BATCHES_NUM: Option<&str> = option_env!("SEND_DEMO_BATCHES_NUM");
const SHOULD_SEND_MEASUREMENTS: bool = parse_bool(option_env!("SHOULD_SEND_MEASUREMENTS"));

bind_interrupts!(
    struct Irqs {
    I2C1_EV => i2c::EventInterruptHandler<peripherals::I2C1>;
    I2C1_ER => i2c::ErrorInterruptHandler<peripherals::I2C1>;
    DMA1_STREAM0 => dma::InterruptHandler<peripherals::DMA1_CH0>;
    DMA1_STREAM6 => dma::InterruptHandler<peripherals::DMA1_CH6>;
    DMA1_STREAM4 => dma::InterruptHandler<peripherals::DMA1_CH4>;
    DMA1_STREAM3 => dma::InterruptHandler<peripherals::DMA1_CH3>;
    }
);



#[embassy_executor::main]
async fn main1(spawner: Spawner) {
    info!("Starting initialization");
    let p = embassy_stm32::init(Default::default());
    let mut internal_led = Output::new(p.PA5, Level::High, Speed::High);

    info!("Initializing ADC");
    let mut adc = adc::Adc::new(p.ADC1, p.PA0);

    info!("Initializing I2C");
    let mut i2c = I2c::new(
        p.I2C1,
        p.PB6,
        p.PB7,
        p.DMA1_CH6,
        p.DMA1_CH0,
        Irqs,
        Config::default(),
    );
    info!("Initializing Clock");
    let mut clock = clock::Clock;
    if let Some(serialized_date_time) = INIT_DATE_TIME {
        info!("Setting Clock to {}", serialized_date_time);
        let configured_datetime = serialized_date_time
            .parse::<NaiveDateTime>()
            .unwrap_or_else(|_| panic!("Could not parse date time {}", serialized_date_time));
        clock.write(&mut i2c, &configured_datetime).await;
    }
    let clock_time = clock.read(&mut i2c).await;
    info!(
        "Clock is set to date time {:?}",
        defmt::Display2Format(&clock_time)
    );

    info!("Initializing Memory");
    let mut memory = memory::Memory::new(&mut i2c).await;

    info!("Should clear memory set to {}", CLEAR_MEMORY);
    if CLEAR_MEMORY {
        info!("Clearing memory");
        memory.clear(&mut i2c).await;
    } else {
        info!("Memory is preserved");
    }
    info!("Memory state is: {:?}", memory);

    info!("Initializing SPI");
    let spi_sender = spi_sender::SpiSender::new();
    let sck = p.PB10;
    let miso = p.PC2;
    let mosi = p.PC1;
    let mut cs = Output::new(p.PB4, Level::High, Speed::VeryHigh);
    let spi_config = embassy_stm32::spi::Config::default();
    let mut spi_bus = embassy_stm32::spi::Spi::new(
        p.SPI2, sck, mosi, miso, p.DMA1_CH4, p.DMA1_CH3, Irqs, spi_config,
    );
    info!("Finished initializing SPI");

    info!("Create demo data set to {}", CREATE_DEMO_DATA);
    if CREATE_DEMO_DATA {
        info!("Creating demo data");
        let entries = create_demo_entries();
        for e in entries {
            memory.write_next(&mut i2c, &e).await;
        }
    } else {
        info!("Creating demo data is disabled");
    }
    info!("Memory state is: {:?}", memory);

    let num_send_demo_batches = parse_u32(SEND_DEMO_BATCHES_NUM);
    info!(
        "Sending demo data to consumer, num batches {}",
        num_send_demo_batches
    );
    for i in 1..=num_send_demo_batches {
        info!(
            "Sending demo data, batch {} of {}",
            i, num_send_demo_batches
        );
        for entry in &create_demo_entries() {
            spi_sender.send(&mut spi_bus, &mut cs, entry).await;
        }
        info!("Finished sending batch, wait 1 sec");
        internal_led.toggle();
        Timer::after_millis(1000).await;
    }
    info!("Sending demo data is finished");

    info!("Processing report");
    info!("Should print report set to {}", PRINT_REPORT);
    if PRINT_REPORT {
        info!("-----------");
        info!("Report");
        info!("Date time, Value");
        let mut buf: [Option<entry::LogEntry>; 100] = core::array::from_fn(|_| None);
        memory.read_all(&mut i2c, &mut buf).await.unwrap();
        for el in buf {
            if let Some(entry) = el {
                info!(
                    "{:?} {:?}",
                    defmt::Display2Format(entry.datetime()),
                    &entry.value()
                );
            }
        }
        info!("-----------");
    } else {
        info!("Printing report is disabled");
    }

    info!("Collect new data");
    info!("COLLECT_NEW_DATA is set to {}", COLLECT_NEW_DATA);
    info!(
        "SHOULD_SEND_MEASUREMENTS is set to {}",
        SHOULD_SEND_MEASUREMENTS
    );
    if COLLECT_NEW_DATA {
        info!("Starting collecting new data");
        loop {
            let adc_value = adc.measure();
            let now = clock.read(&mut i2c).await;
            debug!(
                "Adding new entry: {:?} {:?}",
                defmt::Display2Format(&now),
                adc_value
            );
            let entry = entry::LogEntry::new(now, adc_value);
            // memory.write_next(&mut i2c, &entry).await;
            if SHOULD_SEND_MEASUREMENTS {
                info!(
                    "Sending entry: {:?} {:?}",
                    defmt::Display2Format(entry.datetime()),
                    &entry.value()
                );
                spi_sender.send(&mut spi_bus, &mut cs, &entry).await;
            }
            internal_led.toggle();
            Timer::after_millis(1000).await;
        }
    } else {
        info!("Collecting new data is disabled");
        info!("Finished");
        loop {
            internal_led.toggle();
            Timer::after_millis(5000).await;
        }
    }
}
fn create_demo_entries() -> [entry::LogEntry; 3] {
    let datetime = "2015-09-18T23:56:04".parse::<NaiveDateTime>().unwrap();
    let e1 = entry::LogEntry::new(datetime, 42);
    let datetime = "2040-10-22T13:56:04".parse::<NaiveDateTime>().unwrap();
    let e2 = entry::LogEntry::new(datetime, 4095);
    let datetime = "2015-09-18T23:56:10".parse::<NaiveDateTime>().unwrap();
    let e3 = entry::LogEntry::new(datetime, 1);
    [e1, e2, e3]
}
