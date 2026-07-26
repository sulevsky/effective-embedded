use chrono::{Datelike, NaiveDate, NaiveDateTime, Timelike};

const RTC_ADDR: u8 = 0x68;

pub struct Clock;

impl Clock {
    pub async fn read<T: embedded_hal_async::i2c::I2c>(&self, i2c: &mut T) -> NaiveDateTime {
        let mut buf = [0u8; 7];
        i2c.write_read(RTC_ADDR, &[0x00], &mut buf).await.unwrap();
        NaiveDate::from_ymd_opt(
            (from_bcd(buf[6]) as i32) + 2000,
            from_bcd(buf[5]) as u32,
            from_bcd(buf[4]) as u32,
        )
        .unwrap()
        .and_hms_opt(
            from_bcd(buf[2] & 0x3F) as u32,
            from_bcd(buf[1]) as u32,
            from_bcd(buf[0] & 0x7F) as u32,
        )
        .unwrap()
    }

    pub async fn write<T: embedded_hal_async::i2c::I2c>(
        &self,
        i2c: &mut T,
        datetime: &NaiveDateTime,
    ) {
        let mut buf = [0u8; 8];
        buf[0] = 0x00;
        buf[1] = to_bcd(datetime.second() as u8) | 0x80;
        buf[2] = to_bcd(datetime.minute() as u8);
        buf[3] = to_bcd(datetime.hour() as u8) & 0x3F;
        buf[4] = 0;
        buf[5] = to_bcd(datetime.day() as u8);
        buf[6] = to_bcd(datetime.month() as u8);
        buf[7] = to_bcd((datetime.year() - 2000) as u8);

        i2c.write(RTC_ADDR, &buf).await.unwrap();
    }
}

fn from_bcd(value: u8) -> u8 {
    (value >> 4) * 10 + (value & 0x0F)
}

fn to_bcd(value: u8) -> u8 {
    ((value / 10) << 4) | (value % 10)
}
