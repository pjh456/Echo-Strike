#ifndef INCLUDE_STATE_NODE
#define INCLUDE_STATE_NODE

class StateNode
{
public:
    StateNode() = default;
    virtual ~StateNode() = default;

    virtual void on_enter() {}
    virtual void on_update(float) {}
    virtual void on_exit() {}
};

#endif // INCLUDE_STATE_NODE