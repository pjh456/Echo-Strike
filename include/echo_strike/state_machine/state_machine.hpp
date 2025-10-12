#ifndef INCLUDE_STATE_MACHINE
#define INCLUDE_STATE_MACHINE

#include <echo_strike/state_machine/transition.hpp>
#include <echo_strike/state_machine/state_node.hpp>

#include <string>
#include <unordered_map>

class StateMachine
{
public:
    using StateTable = std::unordered_map<std::string, StateNode *>;

private:
    StateTable states;
    StateNode *current_state = nullptr;

    bool is_init = false;

public:
    StateMachine() = default;
    ~StateMachine()
    {
        for (auto &[k, v] : states)
            if (v != nullptr)
                delete v;
    }

public:
    StateNode *get_current_state() const { return current_state; }

public:
    void on_update(float delta)
    {
        if (!current_state)
            return;

        if (!is_init)
        {
            current_state->on_enter();
            is_init = true;
        }

        for (const auto &transition : current_state->get_transitions())
        {
            if (transition.condition())
            {
                switch_to(transition.target_state_name);
                return;
            }
        }

        current_state->on_update(delta);
    }

    void set_entry(const std::string &name)
    {
        if (states.count(name))
            current_state = states[name];
        else
            current_state = nullptr;

        is_init = false;
    }

    void switch_to(const std::string &name)
    {
        if (current_state && is_init)
            current_state->on_exit();

        set_entry(name);

        if (current_state && !is_init)
        {
            current_state->on_enter();
            is_init = true;
        }
    }

    void register_state(const std::string &name, StateNode *state) { states[name] = state; }

public:
    StateTable &get_states() { return states; }
    const StateTable &get_states() const { return states; }
};

#endif // INCLUDE_STATE_MACHINE