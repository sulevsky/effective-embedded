use core::cell::RefCell;

use defmt::warn;
use embedded_hal::i2c::I2c;
use stm32f4xx_hal::pac::adc1::ltr;

// IMPORTANT temporary blocking, async does not work (halts)
const BARO_DEVICE_ADDR: u8 = 0x77;

#[derive(Debug)]
pub struct CalibrationData {
    t1: u16,
    t2: i16,
    t3: i16,

    p1: u16,
    p2: i16,
    p3: i16,
    p4: i16,
    p5: i16,
    p6: i16,
    p7: i16,
    p8: i16,
    p9: i16,

    h1: u8,
    h2: i16,
    h3: u8,
    h4: i16,
    h5: i16,
    h6: u8,
}
pub struct UncalibratedSensor<'a, I2C> {
    i2c: &'a RefCell<I2C>,
}
impl<'a, I2C: I2c> UncalibratedSensor<'a, I2C> {
    pub fn new(i2c: &'a RefCell<I2C>) -> Self {
        Self { i2c }
    }
    pub fn calibrate(self, over_sampling_setting: u8) -> CalibratedSensor<'a, I2C> {
        let mut i2c = self.i2c.borrow_mut();
        // id
        let mut buf = [0u8; 1];
        i2c.write_read(BARO_DEVICE_ADDR, &[0xD0], &mut buf).unwrap();
        let id = buf[0];
        warn!("id is {}", id);
        // calibration data
        let mut buf_1 = [0u8; 24];
        i2c.write_read(BARO_DEVICE_ADDR, &[0x88], &mut buf_1)
            .unwrap();
        warn!("read buf {}", buf_1);
        let mut buf_2 = [0u8; 1];
        i2c.write_read(BARO_DEVICE_ADDR, &[0xA1], &mut buf_2)
            .unwrap();
        warn!("read buf {}", buf_2);
        let mut buf_3 = [0u8; 7];
        i2c.write_read(BARO_DEVICE_ADDR, &[0xE1], &mut buf_3)
            .unwrap();
        warn!("read buf {}", buf_3);

        let calibration_data = Self::decode_to_calibration_data(&buf_1, &buf_2, &buf_3);

        CalibratedSensor {
            i2c: self.i2c,
            id,
            calibration_data,
            over_sampling_setting,
        }
    }
    fn decode_to_calibration_data(
        buf_1: &[u8; 24],
        buf_2: &[u8; 1],
        buf_3: &[u8; 7],
    ) -> CalibrationData {
        let u16_le = |a: &[u8], idx: usize| u16::from_le_bytes([a[idx], a[idx + 1]]);
        let i16_le = |a: &[u8], idx: usize| i16::from_le_bytes([a[idx], a[idx + 1]]);

        CalibrationData {
            t1: u16_le(buf_1, 0),
            t2: i16_le(buf_1, 2),
            t3: i16_le(buf_1, 4),

            p1: u16_le(buf_1, 6),
            p2: i16_le(buf_1, 8),
            p3: i16_le(buf_1, 10),
            p4: i16_le(buf_1, 12),
            p5: i16_le(buf_1, 14),
            p6: i16_le(buf_1, 16),
            p7: i16_le(buf_1, 18),
            p8: i16_le(buf_1, 20),
            p9: i16_le(buf_1, 22),

            h1: buf_2[0],
            h2: i16_le(buf_3, 0),
            h3: buf_3[2],
            h4: {
                let e4 = (buf_3[3] as i8 as i16) << 4;
                let e5 = (buf_3[4] as i16) & 0xF;
                e4 | e5
            },
            h5: {
                let e5 = (buf_3[4] as i16) >> 4;
                let e6 = (buf_3[5] as i16) << 4;
                e5 | e6
            },
            h6: buf_3[6],
        }
    }
}

pub struct CalibratedSensor<'a, I2C> {
    i2c: &'a RefCell<I2C>,
    id: u8,
    calibration_data: CalibrationData,
    over_sampling_setting: u8,
}

impl<'a, I2C: I2c> CalibratedSensor<'a, I2C> {
    pub fn read_sensor(&mut self) -> SensorData {
        let mut i2c = self.i2c.borrow_mut();
        // uncompensated temperature
        // 1 write
        // hum
        i2c.write(BARO_DEVICE_ADDR, &[0xF2, 0x01]).unwrap();
        // temp press
        i2c.write(BARO_DEVICE_ADDR, &[0xF4, 0x25]).unwrap();
        // 2 wait 10 ms
        // Timer::after_micros(10000).await;

        // 3 read
        let mut buf = [0u8; 8];
        i2c.write_read(BARO_DEVICE_ADDR, &[0xF7], &mut buf).unwrap();
        let ut = ((buf[3] as i32) << 12) | ((buf[4] as i32) << 4) | ((buf[5] as i32) >> 4);

        let up = ((buf[0] as i32) << 12) | ((buf[1] as i32) << 4) | ((buf[2] as i32) >> 4);

        let uh = ((buf[6] as i32) << 8) | (buf[7] as i32);

        // true temperature
        let var1: i32 = (((ut >> 3) - ((self.calibration_data.t1 as i32) << 1))
            * (self.calibration_data.t2 as i32))
            >> 11;

        let var2: i32 = (((((ut >> 4) - (self.calibration_data.t1 as i32))
            * ((ut >> 4) - (self.calibration_data.t1 as i32)))
            >> 12)
            * (self.calibration_data.t3 as i32))
            >> 14;
        let t_fine = var1 + var2;
        let temperature = (t_fine * 5 + 128) >> 8;

        // // true pressure
        // let b6 = b5 - 4000;
        // let x1 = (self.calibration_data.b2 as i32 * (b6 * b6 / 2i32.pow(12))) / 2i32.pow(11);
        // let x2 = self.calibration_data.ac2 as i32 * b6 / 2i32.pow(11);
        // let x3 = x1 + x2;
        // let b3 =
        //     (((self.calibration_data.ac1 as i32 * 4 + x3) << self.over_sampling_setting) + 2) / 4;
        // let x1 = self.calibration_data.ac3 as i32 * b6 / 2i32.pow(13);
        // let x2 = ((self.calibration_data.b1 as i32) * b6 * b6 / 2i32.pow(12)) / 2i32.pow(16);
        // let x3 = (x1 + x2 + 2) / 2i32.pow(2);
        // let b4 = self.calibration_data.ac4 as i32 * (x3 + 32768) / 2i32.pow(15);
        // let b7 = (up - b3) * (50000 >> self.over_sampling_setting);
        // let p = if (b7 as u32) < 0x80000000 {
        //     ((b7 as i64 * 2) as i64 / b4 as i64) as i32
        // } else {
        //     (b7 / b4) * 2
        // };
        // let x1 = (p / 2i32.pow(8)).pow(2);
        // let x1 = (x1 * 3038) / 2i32.pow(16);
        // let x2 = (-7357 * p) / 2i32.pow(16);

        // let pressure = p + (x1 + x2 + 3791) / 2i32.pow(4);

        SensorData {
            temperature,
            pressure: 0,
            humidity: 0,
        }
    }
}
pub struct SensorData {
    pub temperature: i32,
    pub pressure: i32,
    pub humidity: i32,
}
