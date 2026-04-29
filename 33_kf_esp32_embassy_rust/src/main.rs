#![no_std]
#![no_main]

mod logging;

use embassy_executor::Spawner;
use embassy_time::Timer;
use esp_backtrace as _;
use esp_hal::{i2c, peripherals, timer::timg::TimerGroup};

esp_bootloader_esp_idf::esp_app_desc!();

// IMPORTANT temporary blocking, async does not work (halts)
const BARO_DEVICE_ADDR: u8 = 0x77;
#[esp_rtos::main]
async fn main(_spawner: Spawner) {
    logging::init_logging(log::LevelFilter::Info);
    log::info!("Init...");

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
        let calibration_data = to_calibration_data(&buf);
        log::info!("Calibration {:?}", calibration_data);

        let data = read_sensor(&mut baro, &calibration_data, over_sampling_setting).await;
        log::info!("Temperature {:?}", data.temperature as f32 / 10.0);
        log::info!("Pressure {:?}", data.pressure);

        Timer::after_millis(2000).await;
    }
}

#[derive(Debug)]
struct CalibrationData {
    ac1: i16,
    ac2: i16,
    ac3: i16,
    ac4: u16,
    ac5: u16,
    ac6: u16,
    b1: i16,
    b2: i16,
    mb: i16,
    mc: i16,
    md: i16,
}
struct SensorData {
    temperature: i32,
    pressure: i32,
}

fn to_calibration_data(arr: &[u8; 22]) -> CalibrationData {
    CalibrationData {
        ac1: i16::from_be_bytes(arr[0..2].try_into().unwrap()),
        ac2: i16::from_be_bytes(arr[2..4].try_into().unwrap()),
        ac3: i16::from_be_bytes(arr[4..6].try_into().unwrap()),
        ac4: u16::from_be_bytes(arr[6..8].try_into().unwrap()),
        ac5: u16::from_be_bytes(arr[8..10].try_into().unwrap()),
        ac6: u16::from_be_bytes(arr[10..12].try_into().unwrap()),
        b1: i16::from_be_bytes(arr[12..14].try_into().unwrap()),
        b2: i16::from_be_bytes(arr[14..16].try_into().unwrap()),
        mb: i16::from_be_bytes(arr[16..18].try_into().unwrap()),
        mc: i16::from_be_bytes(arr[18..20].try_into().unwrap()),
        md: i16::from_be_bytes(arr[20..22].try_into().unwrap()),
    }
}

async fn read_sensor(
    baro: &mut i2c::master::I2c<'_, esp_hal::Blocking>,
    calibration_data: &CalibrationData,
    over_sampling_setting: u8,
) -> SensorData {
    let mut buf = [0u8; 3];
    // uncompensated temperature
    // 1 write
    baro.write(BARO_DEVICE_ADDR, &[0xF4, 0x2E]).unwrap();
    // 2 wait 4.5 millis
    Timer::after_micros(4500).await;
    // 3 read
    baro.write_read(BARO_DEVICE_ADDR, &[0xF6], &mut buf[..2])
        .unwrap();
    let ut = i32::from_be_bytes([0, 0, buf[0], buf[1]]);

    // uncompensated pressure
    // 1 write
    baro.write(
        BARO_DEVICE_ADDR,
        &[0xF4, 0x34 + (over_sampling_setting << 6)],
    )
    .unwrap();
    // 2 wait 4.5 millis
    Timer::after_micros(4500).await;
    // 3 read
    baro.write_read(BARO_DEVICE_ADDR, &[0xF6], &mut buf)
        .unwrap();
    let up = i32::from_be_bytes([0, buf[0], buf[1], buf[2]]) >> (8 - over_sampling_setting);

    // true temperature
    let x1 = (ut - calibration_data.ac6 as i32) * calibration_data.ac5 as i32 / 2i32.pow(15);
    let x2 = (calibration_data.mc as i32 * 2i32.pow(11)) / (x1 as i32 + calibration_data.md as i32);
    let b5 = x1 + x2;
    let temperature = (b5 + 8) / 2i32.pow(4);

    // true pressure
    let b6 = b5 - 4000;
    let x1 = (calibration_data.b2 as i32 * (b6 * b6 / 2i32.pow(12))) / 2i32.pow(11);
    let x2 = calibration_data.ac2 as i32 * b6 / 2i32.pow(11);
    let x3 = x1 + x2;
    let b3 = (((calibration_data.ac1 as i32 * 4 + x3) << over_sampling_setting) + 2) / 4;
    let x1 = calibration_data.ac3 as i32 * b6 / 2i32.pow(13);
    let x2 = ((calibration_data.b1 as i32) * b6 * b6 / 2i32.pow(12)) / 2i32.pow(16);
    let x3 = (x1 + x2 + 2) / 2i32.pow(2);
    let b4 = calibration_data.ac4 as i32 * (x3 + 32768) / 2i32.pow(15);
    let b7 = (up - b3) * (50000 >> over_sampling_setting);
    let p = if (b7 as u32) < 0x80000000 {
        ((b7 as i64 * 2) as i64 / b4 as i64) as i32
    } else {
        (b7 / b4) * 2
    };
    let x1 = (p / 2i32.pow(8)).pow(2);
    let x1 = (x1 * 3038) / 2i32.pow(16);
    let x2 = (-7357 * p) / 2i32.pow(16);

    let pressure = p + (x1 + x2 + 3791) / 2i32.pow(4);

    SensorData {
        temperature,
        pressure,
    }
}
