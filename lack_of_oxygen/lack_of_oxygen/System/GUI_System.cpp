#include "GUI_System.h"
#include "../Component/Component.h"
#include "../Manager/Input_Manager.h"
#include "../Manager/Log_Manager.h"

namespace lof {
    void GUI_System::show_loading_screen() {
        if (is_visible()) {
            LM.write_log("GUI_System::show_loading_screen(): Loading screen already shown");
            return;
        }

        // Create container box
        container_id = ecs_manager.clone_entity_from_prefab("gui_container");
        if (container_id != INVALID_ENTITY_ID) {
            auto* container_gui = get_component_safe<GUI_Component>(container_id);
            if (!container_gui) {
                hide_loading_screen();
                return;
            }
            container_gui->is_container = true;

            // Create circular images FIRST (on top)
            left_image_id = ecs_manager.clone_entity_from_prefab("gui_image");
            if (left_image_id != INVALID_ENTITY_ID) {
                if (auto* transform = get_component_safe<Transform2D>(left_image_id)) {
                    transform->position = Vec2D(-IMAGE_OFFSET, VERTICAL_SPACING);
                    transform->scale = Vec2D(IMAGE_SIZE, IMAGE_SIZE);
                }
                if (auto* graphics = get_component_safe<Graphics_Component>(left_image_id)) {
                    graphics->texture_name = "Oxygen_Refill_Red_circle";
                    graphics->color = glm::vec3(1.0f); // White color to show texture
                }
            }

            right_image_id = ecs_manager.clone_entity_from_prefab("gui_image");
            if (right_image_id != INVALID_ENTITY_ID) {
                if (auto* transform = get_component_safe<Transform2D>(right_image_id)) {
                    transform->position = Vec2D(IMAGE_OFFSET, VERTICAL_SPACING);
                    transform->scale = Vec2D(IMAGE_SIZE, IMAGE_SIZE);
                }
                if (auto* graphics = get_component_safe<Graphics_Component>(right_image_id)) {
                    graphics->texture_name = "Oxygen_Refill_Green_circle";
                    graphics->color = glm::vec3(1.0f); // White color to show texture
                }
            }

            // Create background bar (below images)
            background_bar_id = ecs_manager.clone_entity_from_prefab("gui_progress_bar");
            if (background_bar_id != INVALID_ENTITY_ID) {
                if (auto* bg_gui = get_component_safe<GUI_Component>(background_bar_id)) {
                    bg_gui->is_progress_bar = true;
                    bg_gui->progress = 1.0f;
                }
                if (auto* bg_graphics = get_component_safe<Graphics_Component>(background_bar_id)) {
                    bg_graphics->color = glm::vec3(1.f); // White for background
                }
                if (auto* bg_transform = get_component_safe<Transform2D>(background_bar_id)) {
                    bg_transform->scale = Vec2D(PROGRESS_BAR_WIDTH, PROGRESS_BAR_HEIGHT);
                    bg_transform->position.y = -VERTICAL_SPACING;
                }
            }

            // Create main progress bar
            progress_bar_id = ecs_manager.clone_entity_from_prefab("gui_progress_bar");
            if (progress_bar_id != INVALID_ENTITY_ID) {
                if (auto* progress_gui = get_component_safe<GUI_Component>(progress_bar_id)) {
                    progress_gui->is_progress_bar = true;
                    progress_gui->progress = 0.0f;
                }
                if (auto* progress_graphics = get_component_safe<Graphics_Component>(progress_bar_id)) {
                    progress_graphics->color = glm::vec3(0.2f, 0.6f, 1.0f);
                }
                if (auto* progress_transform = get_component_safe<Transform2D>(progress_bar_id)) {
                    progress_transform->scale = Vec2D(0.0f, PROGRESS_BAR_HEIGHT);
                    progress_transform->position = Vec2D(-PROGRESS_BAR_WIDTH / 2.0f, -VERTICAL_SPACING);
                }
            }
        }
    }

    void GUI_System::hide_loading_screen() {
        // Destroy all entities and remove from system
        for (EntityID id : {container_id, background_bar_id, progress_bar_id, left_image_id, right_image_id}) {
            if (id != INVALID_ENTITY_ID) {
                ecs_manager.destroy_entity(id);
                remove_entity(id);
            }
        }

        // Reset all IDs
        container_id = INVALID_ENTITY_ID;
        background_bar_id = INVALID_ENTITY_ID;
        progress_bar_id = INVALID_ENTITY_ID;
        left_image_id = INVALID_ENTITY_ID;
        right_image_id = INVALID_ENTITY_ID;

        LM.write_log("GUI_System::hide_loading_screen(): Loading screen hidden and cleaned up");
    }

    void GUI_System::set_progress(float progress) {
        auto* progress_gui = get_component_safe<GUI_Component>(progress_bar_id);
        auto* transform = get_component_safe<Transform2D>(progress_bar_id);

        if (!progress_gui || !transform) return;

        // Update progress value and bar width
        progress_gui->progress = clamp(progress, 0.0f, 1.0f);
        transform->scale.x = PROGRESS_BAR_WIDTH * progress_gui->progress;

        // Keep the progress bar anchored to the left side and grow rightward
        transform->position.x = -PROGRESS_BAR_WIDTH / 2.0f + (transform->scale.x / 2.0f);

        LM.write_log("GUI_System::set_progress(): Progress updated to %.2f", progress_gui->progress);
    }

    void GUI_System::update(float delta_time) {
        auto* container_transform = get_component_safe<Transform2D>(container_id);
        if (!container_transform) return;

        Vec2D container_pos = container_transform->position;

        // Update circular images positions (on top)
        if (auto* left_transform = get_component_safe<Transform2D>(left_image_id)) {
            left_transform->position = container_pos + Vec2D(-IMAGE_OFFSET, VERTICAL_SPACING);
        }

        if (auto* right_transform = get_component_safe<Transform2D>(right_image_id)) {
            right_transform->position = container_pos + Vec2D(IMAGE_OFFSET, VERTICAL_SPACING);
        }

        // Update background bar position (below images)
        if (auto* bg_transform = get_component_safe<Transform2D>(background_bar_id)) {
            bg_transform->position = container_pos + Vec2D(0.0f, -VERTICAL_SPACING);
        }

        // Update progress bar position
        if (auto* progress_transform = get_component_safe<Transform2D>(progress_bar_id)) {
            // Keep X position for progress animation, update Y position only
            float current_x = progress_transform->position.x;
            progress_transform->position = Vec2D(current_x, container_pos.y - VERTICAL_SPACING);
        }
    }
}