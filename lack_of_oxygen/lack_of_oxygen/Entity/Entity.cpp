/**
 * @file Entity.cpp
 * @brief Implements the Entity class member functions.
 * @author Simon Chan (100%)
 * @date September 15, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

// Include header file
#include "Entity.h"

namespace lof {

    Entity::Entity(EntityID id, const std::string& name)
        : id(id), name(name) {}


    EntityID Entity::get_id() const { return id; }

    const std::string& Entity::get_name() const {
        return name;
    }

    void Entity::set_name(const std::string& new_name) {
        name = new_name;
    }

    void Entity::add_component(std::size_t component_id) { mask.set(component_id); }


    void Entity::remove_component(std::size_t component_id) { mask.reset(component_id); }


    bool Entity::has_component(std::size_t component_id) const { return mask.test(component_id); }

 
    ComponentMask Entity::get_component_mask() const { return mask; }

} // namespace lof