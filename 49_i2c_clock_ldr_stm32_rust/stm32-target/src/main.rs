#![no_std]
#![no_main]

mod adc;
mod clock;
mod memory;
mod spi_sender;
mod utils;

use crate::utils::{parse_bool, parse_u32};
use chrono::NaiveDateTime;
use cortex_m::delay::Delay;
use defmt::{debug, info};
use defmt_rtt as _;
use messages::entry;
use panic_probe as _;
use stm32f4xx_hal::gpio::PinState;
use stm32f4xx_hal::{
    gpio::GpioExt,
    i2c::{I2cExt, Mode},
    prelude::*,
};

const INIT_DATE_TIME: Option<&str> = option_env!("INIT_DATE_TIME");
const CLEAR_MEMORY: bool = parse_bool(option_env!("CLEAR_MEMORY"));
const PRINT_REPORT: bool = parse_bool(option_env!("PRINT_REPORT"));
const CREATE_DEMO_DATA: bool = parse_bool(option_env!("CREATE_DEMO_DATA"));
const COLLECT_NEW_DATA: bool = parse_bool(option_env!("COLLECT_NEW_DATA"));
const SEND_DEMO_BATCHES_NUM: Option<&str> = option_env!("SEND_DEMO_BATCHES_NUM");
const SHOULD_SEND_MEASUREMENTS: bool = parse_bool(option_env!("SHOULD_SEND_MEASUREMENTS"));

#[cortex_m_rt::entry]
fn main() -> ! {
    info!("Starting initialization");
    let cp = cortex_m::Peripherals::take().unwrap();
    let dp = stm32f4xx_hal::pac::Peripherals::take().unwrap();
    let mut rcc = dp.RCC.constrain();
    let gpioa = dp.GPIOA.split(&mut rcc);
    let gpiob = dp.GPIOB.split(&mut rcc);
    let gpioc = dp.GPIOC.split(&mut rcc);
    let mut internal_led = gpioa.pa5.into_push_pull_output();
    let mut delay = Delay::new(cp.SYST, rcc.clocks.sysclk().raw());

    info!("Initializing ADC");
    let adc = adc::Adc::new(dp.ADC1, &mut rcc, &gpioa.pa0.into_analog());
    info!("Initializing I2C");
    let mut i2c = dp
        .I2C1
        .i2c((gpiob.pb6, gpiob.pb7), Mode::standard(100.kHz()), &mut rcc);
    info!("Initializing Clock");
    let clock = clock::Clock;
    if let Some(serialized_date_time) = INIT_DATE_TIME {
        info!("Setting Clock to {}", serialized_date_time);
        let configured_datetime = serialized_date_time
            .parse::<NaiveDateTime>()
            .unwrap_or_else(|_| panic!("Could not parse date time {}", serialized_date_time));
        clock.write(&mut i2c, &configured_datetime);
        let clock_time = clock.read(&mut i2c);
        info!(
            "Clock is updated, current date time  {:?}",
            defmt::Display2Format(&clock_time)
        );
    }

    info!("Initializing Memory");
    let mut memory = memory::Memory::new(&mut i2c);

    info!("Should clear memory set to {}", CLEAR_MEMORY);
    if CLEAR_MEMORY {
        info!("Clearing memory");
        memory.clear(&mut i2c);
    } else {
        info!("Memory is preserved");
    }
    info!("Memory state is: {:?}", memory);

    info!("Initializing SPI");
    let spi_sender = spi_sender::SpiSender::new();
    let sck = gpiob.pb10;
    let miso = gpioc.pc2;
    let mosi = gpioc.pc1;
    let mut cs = gpiob.pb4.into_push_pull_output_in_state(PinState::High);
    let mut spi_bus = stm32f4xx_hal::spi::Spi::new(
        dp.SPI2,
        (Some(sck), Some(miso), Some(mosi)),
        embedded_hal::spi::MODE_0,
        1.MHz(),
        &mut rcc,
    );
    info!("Finished initializing SPI");

    info!("Create demo data set to {}", CREATE_DEMO_DATA);
    if CREATE_DEMO_DATA {
        info!("Creating demo data");
        create_demo_entries()
            .iter()
            .for_each(|entry| memory.write_next(&mut i2c, entry));
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
        create_demo_entries()
            .iter()
            .for_each(|entry| spi_sender.send(&mut spi_bus, &mut cs, entry));
        info!("Finished sending batch, wait 1 sec");
        internal_led.toggle();
        delay.delay_ms(1000);

    }
    info!("Sending demo data is finished");

    info!("Processing report");
    info!("Should print report set to {}", PRINT_REPORT);
    if PRINT_REPORT {
        info!("-----------");
        info!("Report");
        info!("Date time, Value");
        let mut buf: [Option<entry::LogEntry>; 1000] = core::array::from_fn(|_| None);
        memory.read_all(&mut i2c, &mut buf).unwrap();
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
    info!("Collect new data is set to {}", COLLECT_NEW_DATA);
    if COLLECT_NEW_DATA {
        info!("Starting collecting new data");
        loop {
            let adc_value = adc.measure();
            let now = clock.read(&mut i2c);
            debug!(
                "Adding new entry: {:?} {:?}",
                defmt::Display2Format(&now),
                adc_value
            );
            memory.write_next(&mut i2c, &entry::LogEntry::new(now, adc_value));
            internal_led.toggle();
            delay.delay_ms(1000);
        }
    } else {
        info!("Collecting new data is disabled");
        info!("Finished");
        loop {
            internal_led.toggle();
            delay.delay_ms(5000);
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
