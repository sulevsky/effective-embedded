#pragma once

#include "stdint.h"
#include "config.h"

class SafeState
{
private:
    uint8_t entered_code[Config::Common::CODE_LENGTH] = {0};
    uint8_t curent_position = 0;
    uint8_t tries = 0;
    int32_t current_counter = 0;
    bool is_current_direction_increment = true;

public:
    SafeState();
    bool is_matching_code();
    void reset();
    void increment_tries_taken();
    bool are_tries_consumed();
    bool reset_tries();
    bool is_counter_updated(int32_t new_counter);
    bool is_direction_same(int32_t new_counter);
    bool is_last_digit();
    void change_direction();
    void increment_position();
    void update_counter(int32_t new_counter);
    void increment_digit();
    uint8_t get_curent_position();
    uint8_t get_entered_digit(int32_t index);
    uint8_t get_tries_left();
};
