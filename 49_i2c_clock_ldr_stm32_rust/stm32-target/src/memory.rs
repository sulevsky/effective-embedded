use defmt::{error, trace, warn};
use messages::entry;

use crate::utils::log_warn;


const MEMORY_ADDR: u8 = 0x50;
const START_ADDR: u16 = 0x0000;
const END_ADDR: u16 = 0x0FFF;
const MAX_RETRIES: u8 = 5;

// TODO vova overflow is not handled

#[derive(Debug, defmt::Format)]
pub struct Memory {
    is_full: bool,
    next_address: u16,
}

impl Memory {
    pub fn new<T: embedded_hal::i2c::I2c>(i2c: &mut T) -> Memory {
        let mut buf = [0u8; 2];
        ensure_bus_available(i2c);
        i2c.write_read(MEMORY_ADDR, &[0x00, 0x00], &mut buf)
            .unwrap();
        let is_full = (buf[0] & 0x80) != 0;
        let mut next_address = (((buf[0] & 0x7F) as u16) << 8) | buf[1] as u16;
        if next_address == 0 {
            next_address = 2;
        }
        Memory {
            is_full,
            next_address,
        }
    }
    pub fn clear<T: embedded_hal::i2c::I2c>(&mut self, i2c: &mut T) {
        let mut pointer = [0u8; 4];
        pointer[2..4].copy_from_slice(&(2u16.to_be_bytes()));
        ensure_bus_available(i2c);
        i2c.write(MEMORY_ADDR, &pointer).unwrap();
        self.is_full = false;
        self.next_address = 2;
    }

    // TODO vova reworked to by byte save due to page issues, consider rework
    pub fn write_next<T: embedded_hal::i2c::I2c>(
        &mut self,
        i2c: &mut T,
        log_entry: &entry::LogEntry,
    ) {
        let serialized_entry = log_entry.serialize();
        let mut buf = [0u8; 3];
        for try_num in 1..=MAX_RETRIES {
            for b in 0..entry::SERIALIZED_LOG_ENTRY_SIZE {
                ensure_bus_available(i2c);
                let memory_address = (self.next_address + b as u16).to_be_bytes();

                buf[0..2].copy_from_slice(&memory_address);
                buf[2] = serialized_entry[b];
                i2c.write(MEMORY_ADDR, &buf).unwrap();
            }
            let log_entry_after_save = self.read(i2c, self.next_address);
            if log_entry != &log_entry_after_save {
                warn!(
                    "Entry was not properly saved, retrying, retry {} of {}",
                    try_num, MAX_RETRIES,
                );
                log_warn("Before", log_entry);
                log_warn("After", &log_entry_after_save);
            } else {
                log_warn("Saved", log_entry);
                break;
            }
        }

        let next_state = self.calculate_next_state();
        self.is_full = next_state.is_full;
        self.next_address = next_state.next_address;
        let mut pointer = [0u8; 4];
        pointer[2..4].copy_from_slice(&self.next_address.to_be_bytes());
        ensure_bus_available(i2c);
        i2c.write(MEMORY_ADDR, &pointer).unwrap();
    }

    fn calculate_next_state(&self) -> Memory {
        let after_write_end = self.next_address + 2 * entry::SERIALIZED_LOG_ENTRY_SIZE as u16;
        if after_write_end > END_ADDR {
            Memory {
                is_full: true,
                next_address: 2,
            }
        } else {
            error!(
                "next address {}",
                self.next_address + entry::SERIALIZED_LOG_ENTRY_SIZE as u16
            );
            Memory {
                is_full: self.is_full,
                next_address: self.next_address + entry::SERIALIZED_LOG_ENTRY_SIZE as u16,
            }
        }
    }

    pub fn get_entries_num(&self) -> u16 {
        if self.is_full {
            (END_ADDR - 2) / 10
        } else {
            (self.next_address - 2) / 10
        }
    }

    pub fn read_all<T: embedded_hal::i2c::I2c>(
        &self,
        i2c: &mut T,
        buf: &mut [Option<entry::LogEntry>],
    ) -> Result<(), &str> {
        let num_entries = self.get_entries_num() as usize;
        let len = buf.len();
        if num_entries > len {
            return Err("buffer is too small");
        }
        let range = if self.is_full {
            2..END_ADDR
        } else {
            2..self.next_address
        }
        .step_by(entry::SERIALIZED_LOG_ENTRY_SIZE as usize);
        let mut el_num = 0;
        for i in range {
            let mut entry_buf = [0u8; 10];
            let mut entry_memory_address = [0u8; 2];
            entry_memory_address.copy_from_slice(&i.to_be_bytes());
            ensure_bus_available(i2c);
            i2c.write_read(MEMORY_ADDR, &entry_memory_address, &mut entry_buf)
                .unwrap();
            let entry = entry::LogEntry::deserialize(&entry_buf);
            warn!("{} {}", el_num, &entry_buf);
            buf[el_num].replace(entry);
            el_num += 1;
        }
        Ok(())
    }
    pub fn read<T: embedded_hal::i2c::I2c>(
        &self,
        i2c: &mut T,
        memory_address: u16,
    ) -> entry::LogEntry {
        let mut entry_memory_address = [0u8; 2];
        entry_memory_address.copy_from_slice(&memory_address.to_be_bytes());
        let mut entry_buf = [0u8; 10];
        ensure_bus_available(i2c);
        i2c.write_read(MEMORY_ADDR, &entry_memory_address, &mut entry_buf)
            .unwrap();
        entry::LogEntry::deserialize(&entry_buf)
    }
}

fn ensure_bus_available<T: embedded_hal::i2c::I2c>(i2c: &mut T) {
    while i2c.write(MEMORY_ADDR, &[]).is_err() {
        trace!("I2C bus is not available, retrying");
    }
}
