#include <iostream>
#include <cassert>
#include <string>
#include <echo_strike/utils/quadtree.hpp>

struct Entity
{
    std::string name;
    Entity(const std::string &n) : name(n) {}
};

int main()
{
    using namespace std;

    // 创建一个边界为 0,0 到 100,100 的四叉树
    QuadTree<Entity> qt(Rect(0, 0, 100, 100));

    Entity a("A"), b("B"), c("C"), d("D");

    Rect ra(10, 10, 10, 10); // 在左上
    Rect rb(60, 10, 10, 10); // 在右上
    Rect rc(10, 60, 10, 10); // 在左下
    Rect rd(90, 90, 10, 10); // 在右下边界

    // ---------- 测试插入 ----------
    assert(qt.insert(ra, &a));
    assert(qt.insert(rb, &b));
    assert(qt.insert(rc, &c));
    assert(qt.insert(rd, &d));

    // ---------- 测试查询 ----------
    {
        auto result = qt.query(Rect(0, 0, 50, 50));
        // 应该至少包含 a
        bool has_a = false;
        for (auto *e : result)
            if (e == &a)
                has_a = true;
        assert(has_a);
    }

    {
        auto result = qt.query(Rect(50, 0, 50, 50));
        bool has_b = false;
        for (auto *e : result)
            if (e == &b)
                has_b = true;
        assert(has_b);
    }

    {
        auto result = qt.query(Rect(0, 50, 50, 50));
        bool has_c = false;
        for (auto *e : result)
            if (e == &c)
                has_c = true;
        assert(has_c);
    }

    {
        auto result = qt.query(Rect(80, 80, 20, 20));
        bool has_d = false;
        for (auto *e : result)
            if (e == &d)
                has_d = true;
        assert(has_d);
    }

    // ---------- 测试 find ----------
    {
        auto *storage = qt.find(ra, &a);
        assert(storage && storage->value == &a);
    }

    // ---------- 测试 remove ----------
    {
        bool removed = qt.remove(&b);
        assert(removed);
        auto *storage = qt.find(rb, &b);
        assert(storage == nullptr);
    }

    // ---------- 测试 update ----------
    {
        Rect rc_new(70, 70, 10, 10);
        qt.update(rc_new, &c);

        // 原位置查不到
        auto *storage_old = qt.find(rc, &c);
        assert(storage_old == nullptr);

        // 新位置能找到
        auto *storage_new = qt.find(rc_new, &c);
        assert(storage_new && storage_new->value == &c);
    }

    cout << "QuadTree tests passed!" << endl;
    return 0;
}