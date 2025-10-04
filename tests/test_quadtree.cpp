#include <iostream>
#include <cassert>

#include <echo_strike/utils/quadtree.hpp>

struct TestObject
{
    int id;
    std::string name;
};

void print_results(const std::string &test_name, const std::vector<TestObject *> &results)
{
    std::cout << "--- " << test_name << " ---\n";
    if (results.empty())
    {
        std::cout << "  No objects found.\n";
    }
    else
    {
        for (const auto &obj : results)
        {
            std::cout << "  Found: ID=" << obj->id << " Name=" << obj->name << "\n";
        }
    }
    std::cout << std::endl;
}

int main()
{
    Rect boundary(0, 0, 100, 100);
    QuadTree<TestObject> tree(boundary);

    // --- 准备测试对象 ---
    TestObject topLeft{1, "TopLeft"};         // quadrant 0
    TestObject topRight{2, "TopRight"};       // quadrant 1
    TestObject bottomLeft{3, "BottomLeft"};   // quadrant 2
    TestObject bottomRight{4, "BottomRight"}; // quadrant 3
    TestObject center{5, "Center"};           // near middle
    TestObject edge{6, "Edge"};               // on boundary
    TestObject outside{7, "OutOfBounds"};     // completely outside

    // --- 1. 插入测试 ---
    assert(tree.insert(Rect(10, 10, 5, 5), &topLeft));
    assert(tree.insert(Rect(80, 10, 5, 5), &topRight));
    assert(tree.insert(Rect(10, 80, 5, 5), &bottomLeft));
    assert(tree.insert(Rect(80, 80, 5, 5), &bottomRight));
    assert(tree.insert(Rect(48, 48, 4, 4), &center));
    assert(tree.insert(Rect(0, 50, 10, 10), &edge));      // 边界上
    assert(!tree.insert(Rect(110, 110, 5, 5), &outside)); // 超出范围

    std::cout << "Insertion tests passed.\n\n";

    // --- 2. 查询测试 ---

    // A. 精确查询左上角
    auto resA = tree.query(Rect(0, 0, 20, 20));
    print_results("Query TopLeft", resA);
    assert(resA.size() == 1 && resA[0]->id == 1);

    // B. 查询右下角
    auto resB = tree.query(Rect(70, 70, 40, 40));
    print_results("Query BottomRight", resB);
    assert(resB.size() == 1 && resB[0]->id == 4);

    // C. 查询中心区域
    auto resC = tree.query(Rect(45, 45, 10, 10));
    print_results("Query Center", resC);
    assert(resC.size() == 1 && resC[0]->id == 5);

    // D. 查询跨越多个象限
    auto resD = tree.query(Rect(0, 0, 100, 30));
    print_results("Query Across Top", resD);
    // 命中 topLeft + topRight
    assert(resD.size() == 2);

    // E. 查询边界（正好包含 Edge）
    auto resE = tree.query(Rect(0, 50, 10, 10));
    print_results("Query Edge", resE);
    assert(resE.size() == 1 && resE[0]->id == 6);

    // F. 查询空区域
    auto resF = tree.query(Rect(90, 0, 5, 5));
    print_results("Query Empty", resF);
    assert(resF.empty());

    // G. 查询整个区域
    auto resG = tree.query(boundary);
    print_results("Query Whole Boundary", resG);
    // 应该有 6 个（除了越界）
    assert(resG.size() == 6);

    std::cout << "All tests completed successfully.\n";
    return 0;
}
