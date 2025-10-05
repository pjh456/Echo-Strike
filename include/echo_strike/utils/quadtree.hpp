#ifndef INCLUDE_QUADTREE
#define INCLUDE_QUADTREE

#include <echo_strike/utils/rect.hpp>

#include <cstdint>
#include <vector>
#include <algorithm>

template <typename T>
class QuadTree
{
private:
    static const size_t MAX_DEPTH = 5;

private:
    struct Storage
    {
        Rect pos;
        T *value;
    };

    struct Node
    {
        Rect boundary;
        std::vector<Storage> values;
        Node *child[4];
        size_t depth;

        Node(size_t dth)
            : depth(dth)
        {
            for (int i = 0; i < 4; ++i)
                child[i] = nullptr;
        }

        ~Node()
        {
            for (int idx = 0; idx < 4; ++idx)
                if (child[idx])
                    delete child[idx];
        }

        bool insert(const Rect &rect, T *val)
        {
            auto op = get_rect_op(rect);
            if (op == 5)
                return false;
            if (op == 4 || depth == MAX_DEPTH)
            {
                values.emplace_back(rect, val);
                return true;
            }
            else
            {
                if (child[op] == nullptr)
                {
                    child[op] = new Node(depth + 1);
                    float sub_width = boundary.get_width() * 0.5f;
                    float sub_height = boundary.get_height() * 0.5f;
                    float x = boundary.get_x();
                    float y = boundary.get_y();

                    switch (op)
                    {
                    case 0:
                        child[op]->boundary = Rect(x, y, sub_width, sub_height);
                        break;
                    case 1:
                        child[op]->boundary = Rect(x + sub_width, y, sub_width, sub_height);
                        break;
                    case 2:
                        child[op]->boundary = Rect(x, y + sub_height, sub_width, sub_height);
                        break;
                    case 3:
                        child[op]->boundary = Rect(x + sub_width, y + sub_height, sub_width, sub_height);
                        break;
                    }
                }
                return child[op]->insert(rect, val);
            }
        }

        void query(const Rect &rect, std::vector<T *> &result) const
        {
            if (!boundary.is_intersect(rect))
                return;

            for (auto &storage : values)
                if (rect.is_intersect(storage.pos))
                    result.push_back(storage.value);

            for (int idx = 0; idx < 4; ++idx)
                if (child[idx] != nullptr)
                    child[idx]->query(rect, result);
        }

        Storage *find(const Rect &rect, T *val)
        {
            if (!boundary.is_intersect(rect))
                return nullptr;

            auto it = std::find_if(
                values.begin(),
                values.end(),
                [&](const Storage &storage)
                { return storage.value == val; });

            if (it != values.end())
                return &(*it);

            for (int idx = 0; idx < 4; ++idx)
            {
                if (child[idx] != nullptr)
                {
                    auto found = child[idx]->find(rect, val);
                    if (found)
                        return found;
                }
            }

            return nullptr;
        }

        const Storage *find(const Rect &rect, T *val) const
        {
            if (!boundary.is_intersect(rect))
                return nullptr;

            auto it = std::find_if(
                values.begin(),
                values.end(),
                [&](const Storage &storage)
                { return storage.value == val; });

            if (it != values.end())
                return &(*it);

            for (int idx = 0; idx < 4; ++idx)
            {
                if (child[idx] != nullptr)
                {
                    auto found = child[idx]->find(rect, val);
                    if (found)
                        return found;
                }
            }

            return nullptr;
        }

        // bool remove(const Rect &rect, T *val)
        // {
        //     if (!boundary.is_intersect(rect))
        //         return false;

        //     for (auto it = values.begin(); it != values.end(); ++it)
        //     {
        //         if (it->value == val)
        //         {
        //             values.erase(it);
        //             return true;
        //         }
        //     }

        //     for (int idx = 0; idx < 4; ++idx)
        //         if (child[idx] != nullptr && child[idx]->remove(rect, val))
        //             return true;

        //     return false;
        // }
        bool remove(T *val)
        {
            for (auto it = values.begin(); it != values.end(); ++it)
            {
                if (it->value == val)
                {
                    values.erase(it);
                    return true;
                }
            }

            for (int idx = 0; idx < 4; ++idx)
                if (child[idx] != nullptr && child[idx]->remove(val))
                    return true;

            return false;
        }

        /*
    0------------------------> x
    |        bottom
    |       | 0 | 1 |
    | left              right
    |       | 2 | 3 |
    |          top
    |
    V
    y

    4 if not in [0, 3]
    5 if out of range
*/
        int get_rect_op(const Rect &rect) const
        {
            if (!rect.is_inside(boundary))
                return 5;

            auto mid_width = boundary.left() + boundary.get_width() * 0.5f;
            auto mid_height = boundary.bottom() + boundary.get_height() * 0.5f;

            if (rect.right() < mid_width)
            {
                if (rect.top() < mid_height)
                    return 0;
                else if (rect.bottom() >= mid_height)
                    return 2;
            }
            else if (rect.left() >= mid_width)
            {
                if (rect.top() < mid_height)
                    return 1;
                else if (rect.bottom() >= mid_height)
                    return 3;
            }
            return 4;
        }
    };

    Node *root;

public:
    QuadTree(const Rect &bound)
        : root(new Node(1))
    {
        root->boundary = bound;
    }

    ~QuadTree()
    {
        delete root;
    }

public:
    bool insert(const Rect &rect, T *val)
    {
        return root->insert(rect, val);
    }

    std::vector<T *> query(const Rect &rect) const
    {
        std::vector<T *> result;
        root->query(rect, result);
        return result;
    }

    Storage *find(const Rect &rect, T *val) { return root->find(rect, val); }
    const Storage *find(const Rect &rect, T *val) const { return root->find(rect, val); }

    // bool remove(const Rect &rect, T *val) { return root->remove(rect, val); }
    bool remove(T *val) { return root->remove(val); }

    // void update(const Rect &old_rect, const Rect &new_rect, T *val)
    // {
    //     if (remove(old_rect, val))
    //         insert(new_rect, val);
    // }
    void update(const Rect &rect, T *val)
    {
        if (remove(val))
            insert(rect, val);
    }
};

#endif // INCLUDE_QUADTREE