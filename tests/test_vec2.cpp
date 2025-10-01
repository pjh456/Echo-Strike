#include <iostream>
#include <echo_strike/utils/vec2.hpp>

int main()
{
    printf("??");
    auto v1 = Vec2(1, 2);
    auto v2 = Vec2(2, 3);
    std::cout << v1 << " " << v2 << std::endl;
    std::cout << v1 + v2 << " " << v1 - v2 << " " << v1 * v2 << std::endl;
    std::cout << v1 * 5 << " " << v1 * v2 / 2 << std::endl;
    std::cout << v1.euclidean_dis(v2) << " " << v1.manhattan_dis(v2) << std::endl;
    return 0;
}