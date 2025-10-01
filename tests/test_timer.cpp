#include <iostream>

#include <echo_strike/core/timer.hpp>

int main()
{
    auto t = Timer(
        []()
        { puts("Shot!"); });
    t.set_one_shot(true);
    t.set_timeout_time(100);
    t.restart();
    for (int i = 1; i <= 10; ++i)
        t.update(20);

    t.set_one_shot(false);
    t.restart();
    puts("restart!");
    for (int i = 1; i <= 10; ++i)
        t.update(20);
}