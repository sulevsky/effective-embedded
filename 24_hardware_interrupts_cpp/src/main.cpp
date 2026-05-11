#include "config.h"
#include "time_utils.h"
#include "solution_interrupt_no_debounce.h"
#include "solution_interrupt_debounce_state.h"
#include "solution_interrupt_debounce_time.h"
#include "solution_polling_debounce.h"

extern "C" void app_main(void)
{
    while (true)
    {
        printf("--------------------\n");
        printf("Starting experiments\n");
        solution_interrupt_no_debounce();
        solution_interrupt_debounce_time();
        solution_interrupt_debounce_state();
        solution_polling_debounce();
        printf("Finished experiments\n");
        printf("--------------------\n\n");
        printf("Restarting in %ld seconds...\n\n", Config::Common::DELAY_BETWEEN_EXPERIMENTS_BATCH_MILLIS / Config::Common::MILLIS_IN_SECONDS);
    }
}
