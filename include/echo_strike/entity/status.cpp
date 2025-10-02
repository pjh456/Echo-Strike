#include <echo_strike/entity/status.hpp>

#include <utility>

StatusBuilder Status::create() { return {}; }

Status Status::operator+(const Status &other)
{
    return Status ::create()
        .hp(m_hp + other.m_hp)
        .attack(m_attack + other.m_attack)
        .speed(m_speed + other.m_speed)
        .build();
}
Status Status::operator-(const Status &other)
{
    return Status::create()
        .hp(m_hp - other.m_hp)
        .attack(m_attack - other.m_attack)
        .speed(m_speed - other.m_speed)
        .build();
}

Status Status::operator*(int ratio)
{
    return Status::create()
        .hp(m_hp * ratio)
        .attack(m_attack * ratio)
        .speed(m_speed * ratio)
        .build();
}

Status Status::operator/(int ratio)
{
    if (ratio == 0)
        return {};
    return Status::create()
        .hp(m_hp / ratio)
        .attack(m_attack / ratio)
        .speed(m_speed / ratio)
        .build();
}

Status &Status::operator+=(const Status &other)
{
    this->operator=(this->operator+(other));
    return *this;
}

Status &Status::operator-=(const Status &other)
{
    this->operator=(this->operator-(other));
    return *this;
}

Status &Status::operator*=(int ratio)
{
    this->operator=(this->operator*(ratio));
    return *this;
}

Status &Status::operator/=(int ratio)
{
    this->operator=(this->operator/(ratio));
    return *this;
}

Status StatusBuilder::build() { return std::move(stus); }