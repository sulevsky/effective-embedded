#pragma once

#include "stdint.h"
#include "safe_state.h"
#include "esp_timer.h"
#include "config.h"

SafeState::SafeState() {};
bool SafeState::is_matching_code()
{
    bool is_matching = true;
    for (int i = 0; i < Config::Common::CODE_LENGTH; i++)
    {
        if (entered_code[i] != Config::Common::CODE[i])
        {
            is_matching = false;
        }
    }
    return is_matching;
}
void SafeState::reset()
{
    for (auto &el : entered_code)
    {
        el = 0;
    }
    curent_position = 0;
    is_current_direction_increment = true;
};
void SafeState::increment_tries_taken()
{
    tries++;
};

bool SafeState::are_tries_consumed()
{
    return tries >= Config::Common::MAX_TRIES;
};
bool SafeState::reset_tries()
{
    return tries == 0;
};
bool SafeState::is_counter_updated(uint8_t new_counter)
{
    return new_counter != current_counter;
};
bool SafeState::is_direction_same(uint8_t new_counter)
{
    bool is_new_direction_increment = new_counter > current_counter;
    return is_current_direction_increment == is_new_direction_increment;
};
bool SafeState::is_last_digit()
{
    return curent_position == Config::Common::CODE_LENGTH - 1;
};
void SafeState::change_direction()
{
    is_current_direction_increment = !is_current_direction_increment;
};
void SafeState::increment_position()
{
    curent_position++;
};
void SafeState::update_counter(uint8_t new_counter)
{
    current_counter = new_counter;
};
void SafeState::increment_digit()
{
    entered_code[curent_position] = (entered_code[curent_position] + 1) % 10;
};
uint8_t SafeState::get_curent_position()
{
    return curent_position;
};
uint8_t SafeState::get_entered_digit(int32_t index)
{
    return entered_code[index];
};
uint8_t SafeState::get_tries_left()
{
    return Config::Common::MAX_TRIES - tries;
};
