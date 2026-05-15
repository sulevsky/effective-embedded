use core::cell::RefCell;

use embedded_hal::i2c::I2c;
use stm32f4xx_hal::serial::Serial1;

const DISPLAY_ADDRESS: u8 = 0x3C;
const COMMAND_REGISTER_ADDRESS: u8 = 0x00;
const DATA_REGISTER_ADDRESS: u8 = 0x40;

pub struct UninitializedDisplay<'a, I2C> {
    i2c: &'a RefCell<I2C>,
}
impl<'a, I2C: I2c> UninitializedDisplay<'a, I2C> {
    pub fn new(i2c: &'a RefCell<I2C>) -> Self {
        Self { i2c }
    }
    pub fn init(self) -> InitializedDisplay<'a, I2C> {
        let i2c = &mut (*self.i2c.borrow_mut());

        // off
        make_command(i2c, &[0xAE]);
        // devide ratio
        make_command(i2c, &[0xD5, 0x80]);
        // multiplex ratio
        make_command(i2c, &[0xA8, 0x3F]);
        // display offset
        make_command(i2c, &[0xD3, 0x00]);
        // start line
        make_command(i2c, &[0x40]);
        // charge pump
        make_command(i2c, &[0x8D, 0x14]);
        // memory addressing mode
        make_command(i2c, &[0x20, 0x00]);
        // segment remap
        make_command(i2c, &[0xA1]);
        make_command(i2c, &[0xC8]);
        // com pins
        make_command(i2c, &[0xDA, 0x12]);
        // contrast
        make_command(i2c, &[0x81, 0xCF]);
        // precharge period
        make_command(i2c, &[0xD9, 0xF1]);
        // vcomh deselect
        make_command(i2c, &[0xDB, 0x40]);
        //on
        make_command(i2c, &[0xA4]);
        // not inverted
        make_command(i2c, &[0xA6]);
        // display on
        make_command(i2c, &[0xAF]);

        InitializedDisplay { i2c: self.i2c }
    }
}

// 5x7 bitmap font for digits 0-9
// each [u8; 5] = 5 columns, bit0=top row, bit6=bottom row
pub const FONT_DIGITS: [[u8; 5]; 10] = [
    // 0          1          2          3          4
    // .###.    ..#..    .###.    .###.    ..#..
    // #...#    .##..    #...#    ....#    .##..
    // #...#    ..#..    ....#    ....#    #.#..
    // #...#    ..#..    ..#..    ..##.    #.#..
    // #...#    ..#..    .#...    ....#    #####
    // #...#    ..#..    #....    ....#    ..#..
    // .###.    ..#..    #####    .###.    ..#..
    [0x3E, 0x41, 0x41, 0x41, 0x3E], // 0
    [0x00, 0x02, 0x7F, 0x00, 0x00], // 1
    [0x42, 0x61, 0x51, 0x49, 0x46], // 2
    [0x00, 0x41, 0x49, 0x49, 0x36], // 3
    [0x18, 0x14, 0x12, 0x7F, 0x10], // 4
    [0x27, 0x45, 0x45, 0x45, 0x39], // 5
    [0x3E, 0x49, 0x49, 0x49, 0x30], // 6
    [0x01, 0x71, 0x09, 0x05, 0x03], // 7
    [0x36, 0x49, 0x49, 0x49, 0x36], // 8
    [0x06, 0x49, 0x49, 0x49, 0x3E], // 9
];
pub struct InitializedDisplay<'a, I2C> {
    i2c: &'a RefCell<I2C>,
}
impl<'a, I2C: I2c> InitializedDisplay<'a, I2C> {
    pub fn print(&mut self, d: f64) {
        let d1 = d as i32 / 10;
        let d2 = d as i32 % 10;
        let d3 = (d * 10.0) as i32 % 10;
        let d4 = (d * 100.0) as i32 % 10;
        let i2c = &mut (*self.i2c.borrow_mut());
        make_command(i2c, &[0x22, 0, 0]); // set page address
        for (i, &el) in [d1, d2, d3, d4].iter().enumerate() {
            let digit = FONT_DIGITS[el as usize];
            make_command(i2c, &[0x21, 0x00 + (i as u8 * 8), 0x00 + (i as u8 * 8) + 8]); // set column start

            for row in 0..5 {
                i2c.write(DISPLAY_ADDRESS, &[DATA_REGISTER_ADDRESS, digit[row]])
                    .unwrap();
            }
        }
    }
    pub fn clear(&mut self) {
        let i2c = &mut (*self.i2c.borrow_mut());
        for page in 0..8 {
            make_command(i2c, &[0x22, page, page]); // set page address
            make_command(i2c, &[0x21, 0x00, 0x7F]); // set column start
            for _ in 0..128 {
                i2c.write(DISPLAY_ADDRESS, &[DATA_REGISTER_ADDRESS, 0x00])
                    .unwrap();
            }
        }
    }
}

fn make_command<I2C: I2c>(i2c: &mut I2C, command: &[u8]) {
    let mut buf = [0u8; 4];
    buf[0] = COMMAND_REGISTER_ADDRESS;

    buf[1..=command.len()].copy_from_slice(command);
    i2c.write(DISPLAY_ADDRESS, &buf[..=command.len()]).unwrap();
}
