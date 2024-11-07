#pragma once
#include "System.h"
#include "../Manager/ECS_Manager.h"
#include "../Utility/Type.h"

#define LOGIC_SYSTEM lof::Logic_System::get_instance()

namespace lof {
    class GUI_System : public System {
    private:
        ECS_Manager& ecs_manager;
        EntityID container_id;      // Main container entity
        EntityID background_bar_id; // Background/empty progress bar
        EntityID progress_bar_id;   // Filling progress bar
        EntityID left_image_id;     // Left image entity
        EntityID right_image_id;    // Right image entity

        // Constants for layout
        const float PROGRESS_BAR_WIDTH = 450.0f;
        const float PROGRESS_BAR_HEIGHT = 40.0f;
        const float IMAGE_OFFSET = 150.0f;
        const float IMAGE_SIZE = 100.0f;
        const float VERTICAL_SPACING = 50.0f;

        // Helper function to clamp values
        float clamp(float value, float min, float max) const {
            if (value < min) return min;
            if (value > max) return max;
            return value;
        }

        // Helper function to safely get component
        template<typename T>
        const T* get_component_safe(EntityID entity_id) const {
            if (entity_id == INVALID_ENTITY_ID) return nullptr;
            if (!ecs_manager.has_component<T>(entity_id)) return nullptr;
            return &ecs_manager.get_component<T>(entity_id);
        }

        template<typename T>
        T* get_component_safe(EntityID entity_id) {
            if (entity_id == INVALID_ENTITY_ID) return nullptr;
            if (!ecs_manager.has_component<T>(entity_id)) return nullptr;
            return &ecs_manager.get_component<T>(entity_id);
        }

        void debug_entity(const char* prefix, EntityID id);
        void validate_gui_state();

    public:
        GUI_System(ECS_Manager& ecs_manager);
        void show_loading_screen();
        void hide_loading_screen();
        void set_progress(float progress);
        void update(float delta_time) override;
        std::string get_type() const override { return "GUI_System"; }

        float get_progress() const {
            if (progress_bar_id == INVALID_ENTITY_ID) return 0.0f;
            const auto* gui = get_component_safe<GUI_Component>(progress_bar_id);
            return gui ? gui->progress : 0.0f;
        }

        bool is_visible() const { return container_id != INVALID_ENTITY_ID; }
    };
}