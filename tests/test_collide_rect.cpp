#include <iostream>
#include <limits>

#include <echo_strike/transform/rect.hpp>

int main()
{
    // 两个矩形
    Rect r1(0, 0, 10, 10);  // 左上角 (0,0), 宽高10
    Rect r2(20, 0, 10, 10); // 左上角 (20,0), 宽高10

    Vec2 v1(10, 0); // r1向右移动 10/s

    float t = r1.time_to_collide(v1, r2);

    std::cout << "Test 1: r1 -> r2 horizontally\n";
    std::cout << "Expected collision time: 1.0\n";
    std::cout << "Computed collision time: " << t << "\n\n";

    // 垂直方向测试
    Rect r3(0, 0, 10, 10);
    Rect r4(0, 25, 10, 10);

    Vec2 v2(0, 5); // r3向下移动

    t = r3.time_to_collide(v2, r4);

    std::cout << "Test 2: r3 -> r4 vertically\n";
    std::cout << "Expected collision time: 3.0\n";
    std::cout << "Computed collision time: " << t << "\n\n";

    // 没有碰撞测试
    Rect r5(0, 0, 10, 10);
    Rect r6(50, 50, 10, 10);

    Vec2 v3(5, 0);

    t = r5.time_to_collide(v3, r6);

    std::cout << "Test 3: r5 -> r6 no collision\n";
    std::cout << "Expected collision time: -1 (no collision)\n";
    std::cout << "Computed collision time: " << t << "\n\n";

    // 对角线碰撞
    Rect r7(0, 0, 10, 10);
    Rect r8(20, 20, 10, 10);

    Vec2 v4(10, 10);

    t = r7.time_to_collide(v4, r8);

    std::cout << "Test 4: r7 -> r8 diagonal\n";
    std::cout << "Expected collision time: 1.0\n";
    std::cout << "Computed collision time: " << t << "\n\n";
    return 0;
}
