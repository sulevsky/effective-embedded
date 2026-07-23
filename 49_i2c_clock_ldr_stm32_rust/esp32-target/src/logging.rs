use esp_println::println;

fn _esp_println_timestamp() -> u64 {
    esp_hal::time::Instant::now()
        .duration_since_epoch()
        .as_millis()
}

struct CrlfLogger;
static LOGGER: CrlfLogger = CrlfLogger;
impl log::Log for CrlfLogger {
    fn enabled(&self, _metadata: &log::Metadata) -> bool {
        true
    }

    fn log(&self, record: &log::Record) {
        print_log_record(record);
    }

    fn flush(&self) {}
}

pub fn init_logging(level: log::LevelFilter) {
    log::set_logger(&LOGGER).unwrap();
    log::set_max_level(level);
}

fn print_log_record(record: &log::Record) {
    const RESET: &str = "\u{001B}[0m";
    const RED: &str = "\u{001B}[31m";
    const GREEN: &str = "\u{001B}[32m";
    const YELLOW: &str = "\u{001B}[33m";
    const BLUE: &str = "\u{001B}[34m";
    const CYAN: &str = "\u{001B}[35m";

    let color = match record.level() {
        log::Level::Error => RED,
        log::Level::Warn => YELLOW,
        log::Level::Info => GREEN,
        log::Level::Debug => BLUE,
        log::Level::Trace => CYAN,
    };
    let reset = RESET;

    println!(
        "{}{} ({}) - {}{}\r",
        color,
        record.level(),
        _esp_println_timestamp(),
        record.args(),
        reset
    );
}
