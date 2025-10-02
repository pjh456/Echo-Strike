#ifndef INCLUDE_STATUS
#define INCLUDE_STATUS

#include <echo_strike/utils/class_marcos.hpp>

class StatusBuilder;

class Status
{
private:
    CLASS_PROPERTY(int, hp)
    CLASS_PROPERTY(int, attack)
    CLASS_PROPERTY(float, speed)

public:
    static StatusBuilder create();

public:
    Status() = default;
    ~Status() = default;

    Status(const Status &) = default;
    Status(Status &&) noexcept = default;

    Status &operator=(const Status &) = default;
    Status &operator=(Status &&) noexcept = default;

public:
    Status operator+(const Status &);
    Status operator-(const Status &);
    Status operator*(int);
    Status operator/(int);

    Status &operator+=(const Status &);
    Status &operator-=(const Status &);
    Status &operator*=(int);
    Status &operator/=(int);
};

class StatusBuilder
{
private:
    Status stus;

public:
    StatusBuilder() = default;
    ~StatusBuilder() = default;

public:
    Status build();

    StatusBuilder &hp(int val) { return stus.set_hp(val), *this; }
    StatusBuilder &attack(int val) { return stus.set_attack(val), *this; }
    StatusBuilder &speed(int val) { return stus.set_speed(val), *this; }
};

#endif // INCLUDE_STATUS