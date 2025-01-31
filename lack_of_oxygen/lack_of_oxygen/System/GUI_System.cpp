/**
 * @file GUI_System.cpp
 * @brief Defines the definition for GUI systems.
 * @author Simon Chan (100%)
 * @date November 07, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

// Include header file
#include "GUI_System.h"

// Include necessary headers
#include "../Component/Component.h"
#include "../Manager/Input_Manager.h"
#include "../Manager/Log_Manager.h"

// Include Utility headers
#include "../Utility/Constant.h"

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

    void GUI_System::update(float delta_time) {
        (void)delta_time;
        auto* container_transform = get_component_safe<Transform2D>(container_id);
        if (!container_transform) return;

        Vec2D container_pos = container_transform->position;

        // Update circular images positions (on top)
        if (auto* left_transform = get_component_safe<Transform2D>(left_image_id)) {
            left_transform->position = container_pos + Vec2D(-DEFAULT_GUI_IMAGE_OFFSET, DEFAULT_GUI_VERTICAL_SPACING);
        }

        if (auto* right_transform = get_component_safe<Transform2D>(right_image_id)) {
            right_transform->position = container_pos + Vec2D(DEFAULT_GUI_IMAGE_OFFSET, DEFAULT_GUI_VERTICAL_SPACING);
        }

        // Update background bar position (below images)
        if (auto* bg_transform = get_component_safe<Transform2D>(background_bar_id)) {
            bg_transform->position = container_pos + Vec2D(0.0f, -DEFAULT_GUI_VERTICAL_SPACING);
        }

        // Update progress bar position
        if (auto* progress_transform = get_component_safe<Transform2D>(progress_bar_id)) {
            // Keep X position for progress animation, update Y position only
            float current_x = progress_transform->position.x;
            progress_transform->position = Vec2D(current_x, container_pos.y - DEFAULT_GUI_VERTICAL_SPACING);
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

    void GUI_System::show_mineral_tank_gui() {
        if (mineral_interaction_container != INVALID_ENTITY_ID) {
            return; // GUI already shown
        }

        // First create main container with the background texture
        mineral_interaction_container = ecs_manager.clone_entity_from_prefab("gui_container");
        if (mineral_interaction_container != INVALID_ENTITY_ID) {
            auto* container_gui = get_component_safe<GUI_Component>(mineral_interaction_container);
            if (!container_gui) {
                hide_mineral_tank_gui();
                return;
            }
            container_gui->is_container = true;
            // Set background texture and properties
            if (auto* graphics = get_component_safe<Graphics_Component>(mineral_interaction_container)) {
                graphics->model_name = "square";
                graphics->texture_name = "Mineral_Refill_UI_BG_Batch_9";
                graphics->color = glm::vec3(1.0f);
            }
            // Position and scale the container
            if (auto* transform = get_component_safe<Transform2D>(mineral_interaction_container)) {
                transform->position = Vec2D(-350.0f, 180.0f);
                transform->scale = Vec2D(500.0f, 300.0f);
            }
        }

        // First add the background (empty) bar 
        mineral_background_bar = ecs_manager.clone_entity_from_prefab("gui_progress_bar");
        if (mineral_background_bar != INVALID_ENTITY_ID) {
            if (auto* graphics = get_component_safe<Graphics_Component>(mineral_background_bar)) {
                graphics->model_name = "square";
                graphics->texture_name = "NoTexture";
                graphics->color = glm::vec3(1.f, 1.f, 1.f); // White background
            }
            if (auto* transform = get_component_safe<Transform2D>(mineral_background_bar)) {
                transform->position = Vec2D(-345.0f, 220.0f);
                transform->scale = Vec2D(430.0f, 35.0f); // Full width background
            }
        }

        // Then add the progress (fill) bar that will grow
        mineral_progress_bar = ecs_manager.clone_entity_from_prefab("gui_progress_bar");
        if (mineral_progress_bar != INVALID_ENTITY_ID) {
            if (auto* graphics = get_component_safe<Graphics_Component>(mineral_progress_bar)) {
                graphics->model_name = "square";
                graphics->texture_name = "NoTexture";
                graphics->color = glm::vec3(1.0f, 0.843f, 0.0f); // Gold color for minerals
            }
            if (auto* transform = get_component_safe<Transform2D>(mineral_progress_bar)) {
                transform->position = Vec2D(-345.0f, 220.0f); // Match background bar position
                transform->scale = Vec2D(0.0f, 35.0f); // Match background bar height
            }
            if (auto* gui = get_component_safe<GUI_Component>(mineral_progress_bar)) {
                gui->is_progress_bar = true;
                gui->progress = 0.0f; // Start at 0%
            }
        }

        // Add percentage text using text_object prefab
        mineral_percentage_text = ecs_manager.clone_entity_from_prefab("text_object");
        if (mineral_percentage_text != INVALID_ENTITY_ID) {
            if (auto* text = get_component_safe<Text_Component>(mineral_percentage_text)) {
                text->font_name = DEFAULT_FONT_NAME;
                int percentage = 0; // 50% to match the progress bar
                text->text = std::to_string(percentage) + "%";
                text->color = glm::vec3(1.0f);
                // Set the text scale directly in the Text_Component
                text->scale = glm::vec2(0.5f, 0.5f); // Adjust this value as needed
            }
            if (auto* transform = get_component_safe<Transform2D>(mineral_percentage_text)) {
                transform->position = Vec2D(-350.0f, 175.0f);
                // Don't set transform scale for text objects, or set it to 1.0
                transform->scale = Vec2D(1.0f, 1.0f);
            }
        }
    }

    void GUI_System::show_oxygen_tank_gui() {
        if (oxygen_interaction_container != INVALID_ENTITY_ID) {
            return; // GUI already shown
        }

        // First  create main container with the background texture
        oxygen_interaction_container = ecs_manager.clone_entity_from_prefab("gui_container");
        if (oxygen_interaction_container != INVALID_ENTITY_ID) {
            auto* container_gui = get_component_safe<GUI_Component>(oxygen_interaction_container);
            if (!container_gui) {
                hide_oxygen_tank_gui();
                return;
            }
            container_gui->is_container = true;
            // Set background texture and properties
            if (auto* graphics = get_component_safe<Graphics_Component>(oxygen_interaction_container)) {
                graphics->model_name = "square";
                graphics->texture_name = "Oxygen_Refill_UI_BG_Batch_10";
                graphics->color = glm::vec3(1.0f);
            }
            // Position and scale the container
            if (auto* transform = get_component_safe<Transform2D>(oxygen_interaction_container)) {
                transform->position = Vec2D(-350.0f, 180.0f);
                transform->scale = Vec2D(500.0f, 300.0f);
            }
        }
    }

    void GUI_System::hide_mineral_tank_gui() {
        // Destroy percentage text first
        if (mineral_percentage_text != INVALID_ENTITY_ID) {
            LM.write_log("Attempting to destroy percentage text entity: %d", mineral_percentage_text);
            ecs_manager.destroy_entity(mineral_percentage_text);
            mineral_percentage_text = INVALID_ENTITY_ID;
        }

        // Then destroy progress bar 
        if (mineral_progress_bar != INVALID_ENTITY_ID) {
            LM.write_log("Attempting to destroy progress bar entity: %d", mineral_progress_bar);
            ecs_manager.destroy_entity(mineral_progress_bar);
            mineral_progress_bar = INVALID_ENTITY_ID;
        }

        // Then destroy background bar
        if (mineral_background_bar != INVALID_ENTITY_ID) {
            LM.write_log("Attempting to destroy background bar entity: %d", mineral_background_bar);
            ecs_manager.destroy_entity(mineral_background_bar);
            mineral_background_bar = INVALID_ENTITY_ID;
        }

        // Finally destroy container
        if (mineral_interaction_container != INVALID_ENTITY_ID) {
            LM.write_log("Attempting to destroy container entity: %d", mineral_interaction_container);
            ecs_manager.destroy_entity(mineral_interaction_container);
            mineral_interaction_container = INVALID_ENTITY_ID;
        }

        // Verify destruction of all entities
        auto verify_container = ecs_manager.get_entity(mineral_interaction_container);
        auto verify_background = ecs_manager.get_entity(mineral_background_bar);
        auto verify_progress = ecs_manager.get_entity(mineral_progress_bar);
        auto verify_text = ecs_manager.get_entity(mineral_percentage_text);

        if (verify_container || verify_background || verify_progress || verify_text) {
            LM.write_log("Warning: Some mineral tank GUI entities still exist after destruction attempt");
            if (verify_text) {
                LM.write_log("Text entity %d still exists", mineral_percentage_text);
            }
            if (verify_background) {
                LM.write_log("Background bar entity %d still exists", mineral_background_bar);
            }
            if (verify_progress) {
                LM.write_log("Progress bar entity %d still exists", mineral_progress_bar);
            }
            if (verify_container) {
                LM.write_log("Container entity %d still exists", mineral_interaction_container);
            }
        }
    }

    void GUI_System::hide_oxygen_tank_gui() {
        //LM.write_log("Starting to hide oxygen tank GUI");
        // Finally destroy container
        if (oxygen_interaction_container != INVALID_ENTITY_ID) {
            LM.write_log("Attempting to destroy container entity: %d", oxygen_interaction_container);
            ecs_manager.destroy_entity(oxygen_interaction_container);
            oxygen_interaction_container = INVALID_ENTITY_ID;
        }

        // Verify destruction
        auto verify_container = ecs_manager.get_entity(oxygen_interaction_container);

        if (verify_container) {
            LM.write_log("Warning: Some entities still exist after destruction attempt");
        }
        else {
            //LM.write_log("All oxygen tank GUI entities successfully destroyed");
        }
    }
}