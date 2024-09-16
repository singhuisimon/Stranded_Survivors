/**
 * @file ECS_Manager.cpp
 * @brief Implements the ECS_Manager class methods.
 * @author Simon Chan
 * @date September 15, 2024
 */

 // Include header file
#include "ECS_Manager.h"

// Include file dependencies
#include "../System/System.h"

namespace lof {

    std::unique_ptr<ECS_Manager> ECS_Manager::instance;
    std::once_flag ECS_Manager::once_flag;


    ECS_Manager::ECS_Manager() {
        set_type("ECS_Manager");
        m_is_started = false;
    }


    ECS_Manager::~ECS_Manager() {
        if (is_started()) {
            shut_down();
        }
    }


    ECS_Manager& ECS_Manager::get_instance() {
        std::call_once(once_flag, []() {
            instance.reset(new ECS_Manager());
            });
        return *instance;
    }


    int ECS_Manager::start_up() {
        if (is_started()) {
            return 0; // Already started
        }

        try {
            // Register components
            register_component<Position_Component>();
            register_component<Velocity_Component>();

            // Create an initial entity (player)
            EntityID player = create_entity();
            add_component<Position_Component>(player, Position_Component(0.0f, 0.0f));
            add_component<Velocity_Component>(player, Velocity_Component(1.0f, 0.0f));

            // Add systems
            add_system(std::make_unique<Movement_System>(*this));

            m_is_started = true;
            LM.write_log("ECS_Manager started successfully.");
            return 0;
        }
        catch (const std::exception& e) {
            LM.write_log("ECS_Manager::start_up(): Failed to initialize. Error: %s", e.what());
            return -1;
        }
    }


    void ECS_Manager::shut_down() {
        if (!is_started()) {
            return; // Not started, nothing to shut down
        }

        // Perform shutdown tasks specific to ECS_Manager
        systems.clear();
        entities.clear();
        component_arrays.clear();
        component_type_to_id.clear();
        id_to_component_type.clear();
        next_component_id = 0;

        m_is_started = false; // Indicate that the manager is no longer started
    }

 
    EntityID ECS_Manager::create_entity() {
        EntityID id = static_cast<EntityID>(entities.size());
        entities.push_back(std::make_unique<Entity>(id));
        return id;
    }


    void ECS_Manager::add_system(std::unique_ptr<System> system) {
        systems.push_back(std::move(system));
    }


    void ECS_Manager::update(float delta_time) {
        for (auto& system : systems) {
            system->update(delta_time);
        }
    }


    const std::vector<std::unique_ptr<Entity>>& ECS_Manager::get_entities() const {
        return entities;
    }

} // namespace lof