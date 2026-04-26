#include "nvs.h"
#include "nvs_flash.h"

static const char *NAMESPACE_NAME = "storage";
static const char *KEY_NAME = "counter";
static const uint32_t INITIAL_COUNTER_VALUE = 0;

nvs_handle_t handle = NULL;

void init_non_volatile_storage()
{
    nvs_flash_init();
    nvs_open(NAMESPACE_NAME, NVS_READWRITE, &handle);
}
void save(uint32_t value)
{
    nvs_set_u32(handle, KEY_NAME, value);
    nvs_commit(handle);
}

uint32_t read()
{
    uint32_t value;
    esp_err_t result = nvs_get_u32(handle, KEY_NAME, &value);
    if (result == ESP_OK)
    {
        return value;
    }
    else
    {
        return INITIAL_COUNTER_VALUE;
    }
}
