#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "constants.h"
#include "time_utils.h"

#define RESOLUTION 4096
#define ADC_CHANNEL ADC_CHANNEL_7

adc_atten_t experiment_configs[] = {
    ADC_ATTEN_DB_0,
    ADC_ATTEN_DB_2_5,
    ADC_ATTEN_DB_6,
    ADC_ATTEN_DB_12,
};

void app_main(void)
{
    while (1)
    {
        // Init uart for logging
        vTaskDelay(pdMS_TO_TICKS(5000));
        printf("Starting experiments ---for removal\n");
        // csv header
        // raw - 0-2^12, calibrated - mV
        printf("experiment_id,raw,calibrated\n");
        uint8_t experiments_num = sizeof(experiment_configs) / sizeof(adc_atten_t);
        adc_oneshot_unit_handle_t adc1_handle;
        adc_oneshot_unit_init_cfg_t init_config1 = {
            .unit_id = ADC_UNIT_1,
        };
        ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

        for (uint8_t i = 0; i < experiments_num; i++)
        {
            uint8_t experiment_id = i + 1;
            printf("Starting experiment %d ---for removal\n", experiment_id);
            adc_atten_t attenuation = experiment_configs[i];
            adc_oneshot_chan_cfg_t channel_config = {
                .atten = attenuation,
                .bitwidth = ADC_BITWIDTH_DEFAULT,
            };
            ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL, &channel_config));

            adc_cali_handle_t adc1_cali_chan0_handle = NULL;
            adc_cali_curve_fitting_config_t cali_config = {
                .unit_id = ADC_UNIT_1,
                .chan = ADC_CHANNEL,
                .atten = attenuation,
                .bitwidth = ADC_BITWIDTH_12,
            };
            ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&cali_config, &adc1_cali_chan0_handle));

            int adc_raw;
            int calibrated_voltage;
            uint32_t experiment_start_millis = now_millis();
            while (!is_expired(now_millis(), experiment_start_millis, EXPERIMENT_DURATION_MILLIS))
            {
                ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL, &adc_raw));
                ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc1_cali_chan0_handle, adc_raw, &calibrated_voltage));
                printf("%d,%d,%d\n", experiment_id, adc_raw, calibrated_voltage);
                vTaskDelay(pdMS_TO_TICKS(TIMESTEP_MILLIS));
            }
            printf("Finished experiment %d ---for removal\n", experiment_id);
        }
        printf("Experiments are completed, restarting in 60 secs ---for removal\n");
        vTaskDelay(pdMS_TO_TICKS(60000));
    }
}
