use embassy_embedded_hal::adapter::BlockingAsync;
use esp_hal::peripherals::FLASH;
use esp_storage::FlashStorage;
use sequential_storage::{
    cache::NoCache,
    map::{MapConfig, MapStorage},
};
const FLASH_RANGE: core::ops::Range<u32> = 0x9000..0xf000;
const KEY: u8 = 0;

pub struct NonVolatileStorage<'d> {
    storage: MapStorage<u8, BlockingAsync<FlashStorage<'d>>, NoCache>,
}

impl<'d> NonVolatileStorage<'d> {
    pub fn new(peripheral_flash: FLASH<'d>) -> Self {
        let flash = FlashStorage::new(peripheral_flash);

        let storage = MapStorage::<u8, BlockingAsync<FlashStorage<'d>>, NoCache>::new(
            BlockingAsync::new(flash),
            const { MapConfig::new(FLASH_RANGE) },
            NoCache::new(),
        );
        Self { storage }
    }

    pub async fn save(&mut self, value: u32) {
        let mut data_buffer = [0u8; 32];
        self.storage
            .store_item(&mut data_buffer, &KEY, &value)
            .await
            .unwrap();
    }
    pub async fn load(&mut self) -> Option<u32> {
        let mut data_buffer = [0u8; 32];
        self.storage
            .fetch_item::<u32>(&mut data_buffer, &KEY)
            .await
            .ok()
            .flatten()
    }
}
