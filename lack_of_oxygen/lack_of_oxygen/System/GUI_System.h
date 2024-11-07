/**
 * @file GUI_System.h
 * @brief Defines the declaration for GUI systems.
 * @author Simon Chan (100%)
 * @date November 07, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */
#pragma once
#ifndef LOF_GUI_SYSTEM_H
#define LOF_GUI_SYSTEM_H

 // Include base system header
#include "System.h"
// Include ECS Manager header
#include "../Manager/ECS_Manager.h"
// Include Utility headers
#include "../Utility/Type.h"

namespace lof {
    class GUI_System : public System {
    private:
        ECS_Manager& ecs_manager;
        EntityID container_id;      // Main container entity
        EntityID background_bar_id; // Background/empty progress bar
        EntityID progress_bar_id;   // Filling progress bar
        EntityID left_image_id;     // Left image entity
        EntityID right_image_id;    // Right image entity
        float last_progress_value = 0.0f;  // Store last progress value

        /**
         * @brief Clamps a value between a minimum and maximum range.
         * @param value The value to be clamped.
         * @param min The minimum allowed value.
         * @param max The maximum allowed value.
         * @return The clamped value within the specified range.
         */
        float clamp(float value, float min, float max) const {
            if (value < min) return min;
            if (value > max) return max;
            return value;
        }

        /**
         * @brief Safely retrieves a constant component from an entity.
         * @tparam T The type of component to retrieve.
         * @param entity_id The ID of the entity to get the component from.
         * @return Pointer to the component if it exists, nullptr otherwise.
         */
        template<typename T>
        const T* get_component_safe(EntityID entity_id) const {
            if (entity_id == INVALID_ENTITY_ID) return nullptr;
            if (!ecs_manager.has_component<T>(entity_id)) return nullptr;
            return &ecs_manager.get_component<T>(entity_id);
        }

        /**
         * @brief Safely retrieves a mutable component from an entity.
         * @tparam T The type of component to retrieve.
         * @param entity_id The ID of the entity to get the component from.
         * @return Pointer to the component if it exists, nullptr otherwise.
         */
        template<typename T>
        T* get_component_safe(EntityID entity_id) {
            if (entity_id == INVALID_ENTITY_ID) return nullptr;
            if (!ecs_manager.has_component<T>(entity_id)) return nullptr;
            return &ecs_manager.get_component<T>(entity_id);
        }

        /**
         * @brief Logs debug information about a specified entity.
         * @param prefix The prefix to use in the debug log message.
         * @param id The ID of the entity to debug.
         */
        void debug_entity(const char* prefix, EntityID id);

        /**
         * @brief Validates the current state of all GUI entities and components.
         */
        void validate_gui_state();

    public:
        /**
         * @brief Constructor for the GUI System.
         * @param ecs_manager Reference to the ECS manager instance.
         */
        GUI_System(ECS_Manager& ecs_manager);

        /**
         * @brief Creates and shows the loading screen GUI elements.
         */
        void show_loading_screen();

        /**
         * @brief Hides and cleans up the loading screen GUI elements.
         */
        void hide_loading_screen();

        /**
         * @brief Updates the progress bar value.
         * @param progress The new progress value between 0.0 and 1.0.
         */
        void set_progress(float progress);

        /**
         * @brief Updates the GUI system's state.
         * @param delta_time Time elapsed since the last update.
         */
        void update(float delta_time) override;

        /**
         * @brief Gets the system type identifier.
         * @return String identifying this system as "GUI_System".
         */
        std::string get_type() const override { return "GUI_System"; }

        /**
         * @brief Gets the current progress value of the loading bar.
         * @return Current progress value between 0.0 and 1.0, or 0.0 if no progress bar exists.
         */
        float get_progress() const {
            if (progress_bar_id == INVALID_ENTITY_ID) return 0.0f;
            const auto* gui = get_component_safe<GUI_Component>(progress_bar_id);
            return gui ? gui->progress : 0.0f;
        }

        /**
         * @brief Checks if the GUI is currently visible.
         * @return True if the GUI container exists, false otherwise.
         */
        bool is_visible() const { return container_id != INVALID_ENTITY_ID; }
    };
} // namespace lof

#endif // LOF_GUI_SYSTEM_H