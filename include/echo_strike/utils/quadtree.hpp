#ifndef INCLUDE_QUADTREE
#define INCLUDE_QUADTREE

#include <echo_strike/utils/rect.hpp>

#include <cstdint>
#include <vector>
#include <utility>

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

        /*
            0------------------------> x
            |          down
            |       | 0 | 1 |
            | left              right
            |       | 2 | 3 |
            |           up
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
            auto mid_height = boundary.down() + boundary.get_height() * 0.5f;

            if (rect.right() < mid_width)
            {
                if (rect.up() < mid_height)
                    return 0;
                else if (rect.down() >= mid_height)
                    return 2;
            }
            else if (rect.left() >= mid_width)
            {
                if (rect.up() < mid_height)
                    return 1;
                else if (rect.down() >= mid_height)
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
};

#endif // INCLUDE_QUADTREE