#ifndef INCLUDE_STATE_NODE
#define INCLUDE_STATE_NODE

#include <echo_strike/state_machine/transition.hpp>

#include <vector>
#include <string>
#include <functional>

class StateNode
{
private:
    std::vector<Transition> transes;

public:
    StateNode() = default;
    virtual ~StateNode() = default;

public:
    void add_transition(const std::string target, std::function<bool()> cond) { transes.emplace_back(target, cond); }

public:
    virtual void on_enter() {}
    virtual void on_update(float) {}
    virtual void on_exit() {}

public:
    std::vector<Transition> &get_transitions() { return transes; }
    const std::vector<Transition> &get_transitions() const { return transes; }
};

#endif // INCLUDE_STATE_NODE