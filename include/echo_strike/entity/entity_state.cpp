#include <echo_strike/entity/entity_state.hpp>

#include <echo_strike/physics/object.hpp>

void EntityState::on_update(float delta)
{
    if (obj)
        anim.set_position(obj->get_rect().get_position());
    anim.update(delta);
}