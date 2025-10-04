#include <iostream>
#include <vector>
#include <string>

#include <echo_strike/utils/vec2.hpp>
#include <echo_strike/utils/ray.hpp>

struct TestCase
{
    Ray r1, r2;
    bool expected;
    std::string name;
};

int main()
{
    std::vector<TestCase> tests = {
        {Ray(Vec2(0, 0), Vec2(4, 4)), Ray(Vec2(0, 4), Vec2(4, 0)), true, "Cross intersect"},
        {Ray(Vec2(0, 0), Vec2(2, 2)), Ray(Vec2(3, 3), Vec2(5, 5)), false, "Separate segments"},
        {Ray(Vec2(0, 0), Vec2(4, 4)), Ray(Vec2(4, 4), Vec2(6, 6)), true, "Touch at endpoint"},
        {Ray(Vec2(0, 0), Vec2(4, 0)), Ray(Vec2(2, 0), Vec2(6, 0)), true, "Overlapping horizontal"},
        {Ray(Vec2(0, 0), Vec2(0, 4)), Ray(Vec2(0, 5), Vec2(0, 8)), false, "Separate vertical"},
        {Ray(Vec2(1, 1), Vec2(3, 3)), Ray(Vec2(3, 3), Vec2(5, 5)), true, "Touch at endpoint diagonal"},
        {Ray(Vec2(0, 0), Vec2(1, 1)), Ray(Vec2(1, 0), Vec2(2, 1)), false, "Parallel non-overlap"}};

    for (const auto &test : tests)
    {
        bool result = test.r1.is_intersect(test.r2);
        std::cout << "Test: " << test.name
                  << " | Expected: " << test.expected
                  << " | Result: " << result
                  << " | " << (result == test.expected ? "PASS" : "FAIL")
                  << "\n";
    }
    return 0;
}
