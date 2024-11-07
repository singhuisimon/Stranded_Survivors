#include "GUI_System.h"
#include "../Component/Component.h"
#include "../Manager/Input_Manager.h"
#include "../Manager/Log_Manager.h"
#include "Logic_System.h"

namespace lof {
    GUI_System::GUI_System(ECS_Manager& ecs_manager) : ecs_manager(ecs_manager) {
        container_id = INVALID_ENTITY_ID;
        background_bar_id = INVALID_ENTITY_ID;
        progress_bar_id = INVALID_ENTITY_ID;
        left_image_id = INVALID_ENTITY_ID;
        right_image_id = INVALID_ENTITY_ID;

        // Set up the required components
        signature.set(ecs_manager.get_component_id<Transform2D>());
        signature.set(ecs_manager.get_component_id<Graphics_Component>());
        signature.set(ecs_manager.get_component_id<GUI_Component>());
    }

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
        LM.write_log("=== Starting GUI cleanup ===");
        validate_gui_state();

        // Log current state of background bar specifically
        if (background_bar_id != INVALID_ENTITY_ID) {
            auto* entity = ecs_manager.get_entity(background_bar_id);
            if (entity) {
                LM.write_log("Background bar before deletion - ID: %u, Name: %s",
                    background_bar_id, entity->get_name().c_str());

                if (ecs_manager.has_component<Graphics_Component>(background_bar_id)) {
                    auto& graphics = ecs_manager.get_component<Graphics_Component>(background_bar_id);
                    LM.write_log("  Graphics component - Color: (%.2f, %.2f, %.2f)",
                        graphics.color.x, graphics.color.y, graphics.color.z);
                }
            }
        }

        // Store current valid IDs, but handle background bar first
        std::vector<std::pair<EntityID, const char*>> entities_to_destroy;

        // Add background bar first if it exists
        if (background_bar_id != INVALID_ENTITY_ID) {
            entities_to_destroy.push_back({ background_bar_id, "Background Bar" });
        }

        // Then add the rest
        if (right_image_id != INVALID_ENTITY_ID) entities_to_destroy.push_back({ right_image_id, "Right Image" });
        if (left_image_id != INVALID_ENTITY_ID) entities_to_destroy.push_back({ left_image_id, "Left Image" });
        if (progress_bar_id != INVALID_ENTITY_ID) entities_to_destroy.push_back({ progress_bar_id, "Progress Bar" });
        if (container_id != INVALID_ENTITY_ID) entities_to_destroy.push_back({ container_id, "Container" });

        // Destroy each entity with extra validation
        for (const auto& [id, name] : entities_to_destroy) {
            LM.write_log("Attempting to destroy %s (ID: %u)", name, id);

            bool was_in_system = false;
            bool was_in_ecs = false;

            // Check initial state
            if (entities.find(id) != entities.end()) {
                was_in_system = true;
                LM.write_log("%s found in GUI system", name);
            }

            if (auto* entity = ecs_manager.get_entity(id)) {
                was_in_ecs = true;
                LM.write_log("%s found in ECS", name);

                // MUST Remove from this system first 
                if (was_in_system) {
                    remove_entity(id);
                    LM.write_log("%s removed from GUI system", name);

                    // Validate removal from system
                    if (entities.find(id) == entities.end()) {
                        LM.write_log("%s successfully removed from GUI system", name);
                    }
                    else {
                        LM.write_log("WARNING: %s still in GUI system after removal attempt", name);
                    }
                }

                // Then destroy in ECS
                LM.write_log("Calling destroy_entity on %s (ID: %u)", name, id);
                ecs_manager.destroy_entity(id);

                // Validate destruction
                if (ecs_manager.get_entity(id) == nullptr) {
                    LM.write_log("%s successfully destroyed in ECS", name);
                }
                else {
                    LM.write_log("WARNING: %s still exists in ECS after destruction attempt", name);
                }
            }
            else {
                LM.write_log("Warning: %s (ID: %u) not found in ECS", name, id);
            }

            // Final validation
            if (was_in_system && entities.find(id) != entities.end()) {
                LM.write_log("ERROR: Failed to remove %s from GUI system", name);
            }
            if (was_in_ecs && ecs_manager.get_entity(id) != nullptr) {
                LM.write_log("ERROR: Failed to destroy %s in ECS", name);
            }
        }

        // Reset IDs
        right_image_id = INVALID_ENTITY_ID;
        left_image_id = INVALID_ENTITY_ID;
        progress_bar_id = INVALID_ENTITY_ID;
        background_bar_id = INVALID_ENTITY_ID;
        container_id = INVALID_ENTITY_ID;

        // Final validation
        validate_gui_state();

        // Specific check for background bar
        if (auto* entity = ecs_manager.get_entity(background_bar_id)) {
            LM.write_log("ERROR: Background bar still exists after cleanup!");
            debug_entity("Lingering Background Bar", background_bar_id);
        }

        LM.write_log("=== GUI cleanup completed ===");

        // Double check system state
        const auto& remaining_entities = get_entities();
        if (!remaining_entities.empty()) {
            LM.write_log("WARNING: %zu entities still in GUI system after cleanup", remaining_entities.size());
            for (EntityID id : remaining_entities) {
                debug_entity("Remaining", id);
            }
        }
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

    void GUI_System::debug_entity(const char* prefix, EntityID id) {
        if (id == INVALID_ENTITY_ID) {
            LM.write_log("%s: Invalid entity ID", prefix);
            return;
        }

        auto* entity = ecs_manager.get_entity(id);
        if (!entity) {
            LM.write_log("%s: Entity %u not found in ECS", prefix, id);
            return;
        }

        LM.write_log("%s: Entity %u exists, name: %s", prefix, id, entity->get_name().c_str());

        if (ecs_manager.has_component<Transform2D>(id)) {
            auto& transform = ecs_manager.get_component<Transform2D>(id);
            LM.write_log("  - Transform2D: pos(%.2f, %.2f)", transform.position.x, transform.position.y);
        }

        if (ecs_manager.has_component<Graphics_Component>(id)) {
            auto& graphics = ecs_manager.get_component<Graphics_Component>(id);
            LM.write_log("  - Graphics: texture='%s'", graphics.texture_name.c_str());
        }

        if (ecs_manager.has_component<GUI_Component>(id)) {
            auto& gui = ecs_manager.get_component<GUI_Component>(id);
            LM.write_log("  - GUI: progress=%.2f, isContainer=%d, isProgressBar=%d",
                gui.progress, gui.is_container, gui.is_progress_bar);
        }

        // Check if entity is in this system
        if (entities.find(id) != entities.end()) {
            LM.write_log("  - Present in GUI_System");
        }
        else {
            LM.write_log("  - NOT present in GUI_System");
        }
    }

    void GUI_System::validate_gui_state() {
        LM.write_log("=== GUI State Validation ===");
        debug_entity("Container", container_id);
        debug_entity("Background Bar", background_bar_id);
        debug_entity("Progress Bar", progress_bar_id);
        debug_entity("Left Image", left_image_id);
        debug_entity("Right Image", right_image_id);

        // List all entities in this system
        LM.write_log("Entities in GUI_System:");
        for (EntityID id : entities) {
            auto* entity = ecs_manager.get_entity(id);
            if (entity) {
                LM.write_log("  - Entity %u (%s)", id, entity->get_name().c_str());
            }
        }
        LM.write_log("=== End GUI State Validation ===");
    }
}