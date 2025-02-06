/**
 * @file GUI_System.h
 * @brief Defines the declaration for GUI systems.
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
        EntityID container_id;      // Example container entity for a general GUI
        EntityID background_bar_id; // Example background/empty progress bar for a general GUI
        EntityID progress_bar_id;   // Example filling progress for a general GUI

        // == MINERAL HOPPER MEMBERS ==
        EntityID mineral_e_prompt = INVALID_ENTITY_ID;  // 'E' prompt entity
        EntityID mineral_interaction_container = INVALID_ENTITY_ID;
        EntityID mineral_background_bar = INVALID_ENTITY_ID;
        EntityID mineral_progress_bar = INVALID_ENTITY_ID;
        EntityID mineral_percentage_text = INVALID_ENTITY_ID;
        EntityID mineral_deposit_count_text = INVALID_ENTITY_ID;
        float mineral_e_prompt_x = 550.0f;  // X position for 'E' prompt
        float stored_mineral_progress = 0.0f;  // Store progress between [0.0f ... 1.0f]

        // == OXYGEN TANK MEMBERS ==
        EntityID oxygen_e_prompt = INVALID_ENTITY_ID;  // 'E' prompt entity for Oxygen tank
        EntityID oxygen_interaction_container = INVALID_ENTITY_ID;
        float oxygen_e_prompt_x = 800.0f;  // X position for 'E' prompt

        static constexpr float OXYGEN_BAR_MAX_WIDTH = 630.0f;
        static constexpr float OXYGEN_BAR_HEIGHT = 15.0f;
        static constexpr float OXYGEN_BAR_PLAYER_Y = 78.0f;
        static constexpr float OXYGEN_BAR_SHIP_Y = 50.0f;
        static constexpr float OXYGEN_BAR_START_X = -657.0f;
        // 1st bar
        EntityID oxygen_percentage_text1 = INVALID_ENTITY_ID;
        EntityID oxygen_progress_bar1 = INVALID_ENTITY_ID;
        float stored_oxygen_progress1 = 0.0f;
        // 2nd bar
        EntityID oxygen_percentage_text2 = INVALID_ENTITY_ID;
        EntityID oxygen_progress_bar2 = INVALID_ENTITY_ID;
        float stored_oxygen_progress2 = 0.0f;

        // == WARNING POPUP MEMBERS ==
        // Warning states
        bool warning_50_active = false;
        bool warning_20_active = false;
        bool warning_5_active = false;
        bool warning_50_shown = false;
        bool warning_20_shown = false;
        bool warning_5_shown = false;
        static constexpr float WARNING_DURATION = 3.0f; // 3 seconds display time

        // Warning display timers
        float warning_50_display_time = 0.0f;
        float warning_20_display_time = 0.0f;
        float warning_5_display_time = 0.0f;

        // Warning entity names (instead of IDs)
        std::string warning_text_50_name;
        std::string warning_container_50_name;
        std::string warning_text_20_name;
        std::string warning_container_20_name;
        std::string warning_text_5_name;
        std::string warning_container_5_name;

        // Timers, positions, etc.
        float e_prompt_animation_timer = 0.0f;
        float original_e_prompt_y = 30.0f;   // The base Y position for 'E' prompt
        float oxygen_e_prompt_animation_timer = 0.0f; // Separate timer if you want separate animation
        const float E_PROMPT_AMPLITUDE = 10.0f;  // How far it moves up/down
        const float E_PROMPT_SPEED = 2.0f;       // How fast it bobs

        float oxygen_update_accumulator = 0.0f;  // Accumulates time to update oxygen bars once per second

        float last_progress_value = 0.0f;  // Store last progress value for something else if needed

        float current_oxygen_level = 100.0f; // Track oxygen level
        float current_mineral_count = 0.0f;  // Track mineral count

        /**
         * @brief Clamps a value between a minimum and maximum range.
         */
        float clamp(float value, float min, float max) const {
            if (value < min) return min;
            if (value > max) return max;
            return value;
        }

        /**
         * @brief Safely retrieves a constant component from an entity.
         */
        template<typename T>
        const T* get_component_safe(EntityID entity_id) const {
            if (entity_id == INVALID_ENTITY_ID) return nullptr;
            if (!ecs_manager.has_component<T>(entity_id)) return nullptr;
            return &ecs_manager.get_component<T>(entity_id);
        }

        /**
         * @brief Safely retrieves a mutable component from an entity.
         */
        template<typename T>
        T* get_component_safe(EntityID entity_id) {
            if (entity_id == INVALID_ENTITY_ID) return nullptr;
            if (!ecs_manager.has_component<T>(entity_id)) return nullptr;
            return &ecs_manager.get_component<T>(entity_id);
        }

        /**
         * @brief Logs debug information about a specified entity.
         */
        void debug_entity(const char* prefix, EntityID id);

        /**
         * @brief Validates the current state of all GUI entities and components.
         */
        void validate_gui_state();

        // Helper function to get entity ID from name
        EntityID get_warning_entity(const std::string& name) {
            if (name.empty()) return INVALID_ENTITY_ID;
            return ecs_manager.find_entity_by_name(name);
        }

    public:
        /**
         * @brief Constructor for the GUI System.
         */
        GUI_System(ECS_Manager& ecs_manager);

        /**
         * @brief Updates the GUI system's state every frame.
         */
        void update(float delta_time) override;

        /**
         * @brief Gets the system type identifier.
         */
        std::string get_type() const override { return "GUI_System"; }

        // == MINERAL TANK GUI SHOW/HIDE ==
        void show_mineral_tank_gui();
        void hide_mineral_tank_gui();
        void update_mineral_progress(float progress);

        /**
         * @brief Get the current mineral hopper percentage progress.
         */
        float get_current_hopper_percentage() const {
            if (mineral_progress_bar != INVALID_ENTITY_ID) {
                if (auto* gui = get_component_safe<GUI_Component>(mineral_progress_bar)) {
                    return gui->progress;
                }
            }
            return 0.0f;
        }

        // == OXYGEN TANK GUI SHOW/HIDE ==
        void show_oxygen_tank_gui();
        void hide_oxygen_tank_gui();

        // == OXYGEN PROGRESS UPDATE FUNCTIONS ==
        void update_oxygen_progress1(float progress);
        void update_oxygen_progress2(float progress);

        // == OXYGEN WARNING FUNCTIONS ==
        void show_oxygen_warning(float percent);
        void hide_oxygen_warning(float percent);

        /**
         * @brief Checks if a general container is currently visible.
         */
        bool is_visible() const { return container_id != INVALID_ENTITY_ID; }

        void reset_all_game_state();

    };

} // namespace lof

#endif // LOF_GUI_SYSTEM_H
