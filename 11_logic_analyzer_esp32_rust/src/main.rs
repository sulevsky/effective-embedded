#![no_std]
#![no_main]

use core::cell::RefCell;

use critical_section::Mutex;
use esp_backtrace as _;
use esp_hal::gpio::{Input, InputConfig, Io};
use esp_hal::{handler, main, ram};
use esp_println::println;
esp_bootloader_esp_idf::esp_app_desc!();

static COUNTER: Mutex<RefCell<u32>> = Mutex::new(RefCell::new(0));
static BUTTON: Mutex<RefCell<Option<Input>>> = Mutex::new(RefCell::new(None));

#[main]
fn main() -> ! {
    println!("Init...");
    let peripherals = esp_hal::init(esp_hal::Config::default());

    let mut io = Io::new(peripherals.IO_MUX);
    io.set_interrupt_handler(push_button_handler);

    let input_config = InputConfig::default().with_pull(esp_hal::gpio::Pull::Up);
    let mut external_button = Input::new(peripherals.GPIO19, input_config);
    external_button.listen(esp_hal::gpio::Event::FallingEdge);

    critical_section::with(|cs| {
        BUTTON.borrow_ref_mut(cs).replace(external_button);
    });
    loop {}
}

#[handler]
#[ram(unstable(rtc_fast))]
fn push_button_handler() {
    critical_section::with(|cs| {
        let mut button = BUTTON.borrow_ref_mut(cs);
        if button.as_ref().unwrap().is_interrupt_set() {
            let mut count = COUNTER.borrow_ref_mut(cs);
            println!("Button is pressed, count {}", *count);
            button.as_mut().unwrap().clear_interrupt();
            *count += 1;
        }
    });
}
