#include "config.h"
#include "safe_state.h"
#include "renderer.h"

void pad_right()
{
    printf("                                     ");
}
void draw_blocked()
{
    printf("No tries left, lock is BLOCKED for 1 hour");
}

void draw_active(SafeState *safe_state)
{
    for (int i = 0; i < Config::Common::CODE_LENGTH; i++)
    {
        if (i == safe_state->get_curent_position())
        {
            printf(">");
        }
        else
        {
            printf(" ");
        }
        printf("%d", safe_state->get_entered_digit(i));
        if (i == safe_state->get_curent_position())
        {
            printf("<");
        }
        else
        {
            printf(" ");
        }
    }
    printf("   LOCKED, tries left %d", safe_state->get_tries_left());
}

void draw_open()
{
    printf("OPEN");
}

void draw(SafeState *safe_state)
{
    printf("\r");
    if (safe_state->are_tries_consumed())
    {
        draw_blocked();
    }
    else if (safe_state->is_matching_code())
    {
        draw_open();
    }
    else
    {
        draw_active(safe_state);
    }
    pad_right();
    fflush(stdout);
}
