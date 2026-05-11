#pragma once

#include "state_management/event_generator.h"
#include "state_management/state_context.h"

class State
{
public:
    virtual State *on_event(const Event event, StateContext *context) = 0;
    virtual ~State() = default;
};

class IdleState : public State
{
public:
    State *on_event(const Event event, StateContext *context) override;
};

class PressedState : public State
{
public:
    State *on_event(const Event event, StateContext *context) override;
};

class HeldShortState : public State
{
public:
    State *on_event(const Event event, StateContext *context) override;
};

class HeldLongState : public State
{
public:
    State *on_event(const Event event, StateContext *context) override;
};

class ReleasedState : public State
{
public:
    State *on_event(const Event event, StateContext *context) override;
};

State *get_initial_state();
