use embedded_hal::i2c::I2c;
pub struct Display<I2C> {
    i2c: I2C,
}
impl<I2C: I2c> Display<I2C> {
    pub fn new(i2c: I2C) -> Self {
        Self { i2c }
    }
    pub fn exec(&mut self) {
        let display_addr = 0x3cu8;
        let control_register_addr = 0x00;
        let data_register_addr = 0x40;
        // let mut buf = [0u8; 1];
        self.i2c
            .write(display_addr, &[control_register_addr, 0xAE])
            .unwrap();
        self.i2c
            .write(display_addr, &[control_register_addr, 0xAF])
            .unwrap();
        self.i2c
            .write(display_addr, &[control_register_addr, 0xA4])
            .unwrap();
        for i in 0..128 {
            self.i2c
                .write(display_addr, &[data_register_addr, 0xFF])
                .unwrap();
        }
    }
}
