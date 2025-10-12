#include <iostream>

#include <echo_strike/transform/rect.hpp>

int main()
{
    Rect r1(0, 0, 10, 10);
    Rect r2(5, 5, 10, 10);
    Rect r3(20, 20, 5, 5);
    std ::cout << r1 << std::endl;
    std ::cout << r2 << std::endl;
    std ::cout << r3 << std::endl;

    std::cout << "r1 vs r2: " << r1.is_intersect(r2) << " (expect 1)\n";
    std::cout << "r1 vs r3: " << r1.is_intersect(r3) << " (expect 0)\n";

    Vec2 p1(1, 1);
    Vec2 p2(15, 15);
    std::cout << "r1 contains p1: " << r1.is_inside(p1) << " (expect 1)\n";
    std::cout << "r1 contains p2: " << r1.is_inside(p2) << " (expect 0)\n";

    Vec2 p3(0, 5);
    Vec2 p4(10, 10);
    std::cout << "r1 on edge p3: " << r1.is_on_edge(p3) << " (expect 1)\n";
    std::cout << "r1 on edge p4: " << r1.is_on_edge(p4) << " (expect 1)\n";

    return 0;
}