#![no_std]
#![no_main]

mod baro;
mod logging;
use embassy_executor::Spawner;
use embassy_time::{Delay, Timer};
use esp_backtrace as _;
use esp_hal::{i2c, timer::timg::TimerGroup};

esp_bootloader_esp_idf::esp_app_desc!();

// IMPORTANT temporary blocking, async does not work (halts)
const BARO_DEVICE_ADDR: u8 = 0x77;

use embedded_hal::delay::DelayNs;
#[esp_rtos::main]
async fn main(_spawner: Spawner) {
    logging::init_logging(log::LevelFilter::Info);
    log::info!("Init...");

    log::error!("Init...");
    let peripherals = esp_hal::init(esp_hal::Config::default());

    let timg0 = TimerGroup::new(peripherals.TIMG0);
    esp_rtos::start(timg0.timer0);

    let mut mpu_i2c = i2c::master::I2c::new(peripherals.I2C0, i2c::master::Config::default())
        .unwrap()
        .with_scl(peripherals.GPIO9)
        .with_sda(peripherals.GPIO10);
    // .into_async();

    let mut mpu = mpu6050_dmp::sensor::Mpu6050::new(mpu_i2c, mpu6050_dmp::address::Address::default()).unwrap();
    // let delay = Timer::
    // mpu.init(&mut Delay).await.unwrap();

    loop {
        // get roll and pitch estimate
        let acc = mpu.accel().unwrap();
        log::info!("r/p: {:?}", acc);

        Timer::after_millis(2000).await;
    }
}

async fn custom_read() {
    let peripherals = esp_hal::init(esp_hal::Config::default());

    let timg0 = TimerGroup::new(peripherals.TIMG0);
    esp_rtos::start(timg0.timer0);

    let mut baro = i2c::master::I2c::new(peripherals.I2C0, i2c::master::Config::default())
        .unwrap()
        .with_scl(peripherals.GPIO5)
        .with_sda(peripherals.GPIO6);
    let over_sampling_setting = 0u8;
    loop {
        // id
        let mut buf = [0u8; 1];
        baro.write_read(BARO_DEVICE_ADDR, &[0xD0], &mut buf)
            .unwrap();
        log::info!("id {:?}", buf);

        // calibration data
        let mut buf = [0u8; 22];
        baro.write(BARO_DEVICE_ADDR, &[0xAA]).unwrap();
        // max buf size is 16
        baro.read(BARO_DEVICE_ADDR, &mut buf[..16]).unwrap();
        baro.read(BARO_DEVICE_ADDR, &mut buf[16..]).unwrap();
        let calibration_data = baro::to_calibration_data(&buf);
        // info!("Calibration {:?}", calibration_data);

        let data = baro::read_sensor(&mut baro, &calibration_data, over_sampling_setting).await;
        log::info!("Temperature {:?}", data.temperature as f32 / 10.0);
        log::info!("Pressure {:?}", data.pressure);

        Timer::after_millis(2000).await;
    }
}
