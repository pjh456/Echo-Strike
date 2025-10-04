#include <echo_strike/collision/collision_box.hpp>

#include <echo_strike/collision/collision_manager.hpp>

CollisionBox::CollisionBox()
    : m_object(nullptr),
      m_enable(true)
{
}

CollisionBox::CollisionBox(CollisionBox &&other) noexcept
    : collide_callback(std::move(other.collide_callback)),
      m_enable(other.m_enable),
      m_src(other.m_src),
      m_dst(std::move(other.m_dst)),
      m_rect(std::move(other.m_rect)),
      m_object(other.m_object)
{
    other.m_src = CollisionLayer::None;
    other.m_dst.clear();
    other.m_object = nullptr;
}

CollisionBox &CollisionBox::operator=(CollisionBox &&other) noexcept
{
    if (this == &other)
        return *this;

    collide_callback = std::move(other.collide_callback);
    m_enable = other.m_enable;
    m_src = other.m_src, other.m_src = CollisionLayer::None;
    m_dst = std::move(other.m_dst), other.m_dst.clear();
    m_rect = std::move(other.m_rect);
    m_object = other.m_object, other.m_object = nullptr;

    return *this;
}

std::vector<CollisionBox *> CollisionBox::process_collide() const
{
    std::vector<CollisionBox *> result;

    if (!get_enable())
        return result;

    if (m_src == CollisionLayer::None || m_dst.empty())
        return result;

    auto collide_result = CollisionManager::instance().quad_tree().query(m_rect);
    result.reserve(collide_result.size());

    for (auto dst_box : collide_result)
    {
        if ((this == dst_box) || (!has_dst(dst_box->get_src())))
            continue;

        if (!dst_box->get_enable())
            continue;

        if (dst_box->get_src() == CollisionLayer::None)
            continue;

        result.push_back(dst_box);
    }

    return result;
}

void CollisionBox::set_rect(const Rect &rect)
{
    // CollisionManager::instance().quad_tree().update(m_rect, rect, this);
    CollisionManager::instance().quad_tree().update(rect, this);
    m_rect = rect;
}