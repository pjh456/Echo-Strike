#ifndef INCLUDE_TRANSITION
#define INCLUDE_TRANSITION

#include <functional>
#include <string>

class Transition
{
public:
    std::string target_state_name;
    std::function<bool()> condition;
};

#endif // INCLUDE_TRANSITION