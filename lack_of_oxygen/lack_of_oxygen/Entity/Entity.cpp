/**
 * @file Entity.cpp
 * @brief Implements the Entity class member functions.
 * @author Simon Chan
 * @date September 15, 2024
 */

// Include header file
#include "Entity.h"

namespace lof {

    Entity::Entity(EntityID id) : id(id) {}


    EntityID Entity::get_id() const { return id; }


    void Entity::add_component(std::size_t component_id) { mask.set(component_id); }


    void Entity::remove_component(std::size_t component_id) { mask.reset(component_id); }


    bool Entity::has_component(std::size_t component_id) const { return mask.test(component_id); }

 
    ComponentMask Entity::get_component_mask() const { return mask; }

} // namespace lof