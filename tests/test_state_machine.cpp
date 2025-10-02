#include <iostream>

#include <echo_strike/state_machine/state_node.hpp>
#include <echo_strike/state_machine/state_machine.hpp>

class Data : public StateNode
{
private:
    std::string name;

public:
    Data(const std::string &str) : name(str) {}
    ~Data() override = default;

public:
    void on_enter() override { std::cout << "Enter " << name << std::endl; }
    void on_update(float delta) override { std::cout << "Update " << name << " For duration " << delta << std::endl; }
    void on_exit() override { std::cout << "Exit " << name << std::endl; }
};

int main()
{
    StateMachine sm;

    sm.register_state("Idle", new Data("Idle"));
    sm.register_state("Run", new Data("Run"));

    // 设置初始状态
    sm.set_entry("Idle");

    // 模拟循环
    for (int i = 0; i < 3; i++)
    {
        sm.on_update(0.016f);
    }

    // 切换到 Run 状态
    std::cout << "Switching state...\n";
    sm.switch_to("Run");

    for (int i = 0; i < 3; i++)
    {
        sm.on_update(0.016f);
    }

    return 0;
}