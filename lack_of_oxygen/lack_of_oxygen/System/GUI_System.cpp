/**
 * @file GUI_System.cpp
 * @brief Defines the definition for GUI systems.
 */

#include "GUI_System.h"

 // Include necessary headers
#include "../Component/Component.h"
#include "../Manager/Input_Manager.h"
#include "../Manager/Log_Manager.h"
#include "../Manager/Game_Manager.h"
#include "../Manager/Graphics_Manager.h"
#include "../Manager/Audio_Manager.h"
#include "../Manager/IMGUI_Manager.h"
#include "../System/Movement_System.h"

// Include Utility headers
#include "../Utility/Constant.h"

namespace lof {
    GUI_System::GUI_System(ECS_Manager& ecs_manager)
        : ecs_manager(ecs_manager)
    {
        container_id = INVALID_ENTITY_ID;
        background_bar_id = INVALID_ENTITY_ID;
        progress_bar_id = INVALID_ENTITY_ID;

        // Set up the required components
        signature.set(ecs_manager.get_component_id<Transform2D>());
        signature.set(ecs_manager.get_component_id<Graphics_Component>());
        signature.set(ecs_manager.get_component_id<GUI_Component>());
    }

    void GUI_System::update(float delta_time)
    {
        // Add logging for current scene and GUI state
        LM.write_log("Current scene: %d", GM.get_current_scene());
        if (mineral_e_prompt != INVALID_ENTITY_ID) {
            LM.write_log("Mineral E prompt exists with ID: %d", mineral_e_prompt);
        }

        // Win screen check
        // Check for win screen first, before any GUI-related code
        if (GM.get_current_scene() == 4) {  // Win screen
            // Force-invalidate all GUI entity IDs
            mineral_e_prompt = INVALID_ENTITY_ID;
            mineral_interaction_container = INVALID_ENTITY_ID;
            mineral_progress_bar = INVALID_ENTITY_ID;
            mineral_percentage_text = INVALID_ENTITY_ID;
            mineral_deposit_count_text = INVALID_ENTITY_ID;
            oxygen_e_prompt = INVALID_ENTITY_ID;
            oxygen_interaction_container = INVALID_ENTITY_ID;
            oxygen_progress_bar1 = INVALID_ENTITY_ID;
            oxygen_progress_bar2 = INVALID_ENTITY_ID;
            oxygen_percentage_text1 = INVALID_ENTITY_ID;
            oxygen_percentage_text2 = INVALID_ENTITY_ID;
            return;  // Skip all GUI updates on win screen
        }

        // == Mineral E prompt bobbing ==
        if (mineral_e_prompt != INVALID_ENTITY_ID) {
            e_prompt_animation_timer += delta_time;
            if (auto* transform = get_component_safe<Transform2D>(mineral_e_prompt)) {
                float offset = std::sin(e_prompt_animation_timer * E_PROMPT_SPEED) * E_PROMPT_AMPLITUDE;
                transform->position.y = original_e_prompt_y + offset;
                transform->position.x = mineral_e_prompt_x; // keep X constant
            }
        }

        // == Oxygen E prompt bobbing ==
        if (oxygen_e_prompt != INVALID_ENTITY_ID) {
            oxygen_e_prompt_animation_timer += delta_time;
            if (auto* transform = get_component_safe<Transform2D>(oxygen_e_prompt)) {
                float offset = std::sin(oxygen_e_prompt_animation_timer * E_PROMPT_SPEED) * E_PROMPT_AMPLITUDE;
                transform->position.y = original_e_prompt_y + offset;
                transform->position.x = oxygen_e_prompt_x; // keep X constant
            }
        }

        // Check if we've reached 100% (50,000 minerals)
        // Check if we've reached 100% (50,000 minerals)
        if (stored_mineral_progress * 50000.0f >= 50000.0f) {
            // Force hide all GUI elements before scene transition
            hide_mineral_tank_gui();
            hide_oxygen_tank_gui();
            hide_oxygen_warning(50.0f);
            hide_oxygen_warning(20.0f);
            hide_oxygen_warning(5.0f);

            // Reset GUI state variables
            mineral_e_prompt = INVALID_ENTITY_ID;
            oxygen_e_prompt = INVALID_ENTITY_ID;
            mineral_interaction_container = INVALID_ENTITY_ID;
            oxygen_interaction_container = INVALID_ENTITY_ID;

            // Clear mineral progress
            stored_mineral_progress = 0.0f;

            // Clear dynamic entities first
            bool found_movement_system = false;
            for (auto& system : ECSM.get_systems()) {
                if (auto* movement_system = dynamic_cast<Movement_System*>(system.get())) {
                    movement_system->clear_dynamic_entities();
                    found_movement_system = true;
                    LM.write_log("Found and cleared Movement System");
                    break;
                }
            }
            if (!found_movement_system) {
                LM.write_log("Warning: Movement System not found");
            }

            // Set up scene loading
            const std::string SCENES = "Scenes";
            std::string scene_file = "win_screen.scn";
            std::string scene_path = ASM.get_full_path(SCENES, scene_file);
            LM.write_log("Attempting to load scene from path: %s", scene_path.c_str());

            // Try to load win screen
            if (SM.load_scene(scene_path.c_str())) {
                LM.write_log("Win screen loaded successfully");

                // Reset camera position
                auto& camera = GFXM.get_camera();
                camera.pos_x = DEFAULT_CAMERA_POS_X;
                camera.pos_y = DEFAULT_CAMERA_POS_Y;

                // Stop all currently playing audio
                ADM.stop_mastergroup();

                // Update current scene in Game Manager
                GM.set_current_scene(4);

                // Update IMGUI Manager's current file
                IMGUIM.set_current_file_shown(scene_file);

                // Reset stored mineral progress
                stored_mineral_progress = 0.0f;

                return;
            }
            else {
                LM.write_log("Failed to load scene file: %s", scene_path.c_str());
            }
        }

        // --------------------------------------------------------
        // Update oxygen bars automatically every second
        // --------------------------------------------------------
        if (oxygen_interaction_container != INVALID_ENTITY_ID) {
            // Update once per second if you want, or every frame
            oxygen_update_accumulator += delta_time;
            if (oxygen_update_accumulator >= 1.0f) {
                oxygen_update_accumulator = 0.0f;

                // Just read the current oxygen
                float player_oxygen = GM.get_current_oxygen_level();
                float fractionPlayer = player_oxygen / 100.0f;
                update_oxygen_progress1(fractionPlayer);
            }
        }
        // --------------------------------------------------------

        // Check oxygen level and update warnings
        float current_oxygen = GM.get_current_oxygen_level();

        // Handle 50% warning
        if (current_oxygen == 50.0f && !warning_50_active) {
            show_oxygen_warning(50.0f);
            warning_50_display_time = 0.0f;
        }
        if (warning_50_shown) {
            warning_50_display_time += delta_time;
            if (warning_50_display_time >= WARNING_DURATION) {
                hide_oxygen_warning(50.0f);
                warning_50_active = false;  // Allow the warning to trigger again
                warning_50_shown = false;
            }
        }

        // Handle 20% warning
        if (current_oxygen == 20.0f && !warning_20_active) {
            show_oxygen_warning(20.0f);
            warning_20_display_time = 0.0f;
        }
        if (warning_20_shown) {
            warning_20_display_time += delta_time;
            if (warning_20_display_time >= WARNING_DURATION) {
                hide_oxygen_warning(20.0f);
                warning_20_active = false;  // Allow the warning to trigger again
                warning_20_shown = false;
            }
        }

        // Handle 5% warning
        if (current_oxygen == 5.0f && !warning_5_active) {
            show_oxygen_warning(5.0f);
            warning_5_display_time = 0.0f;
        }
        if (warning_5_shown) {
            warning_5_display_time += delta_time;
            if (warning_5_display_time >= WARNING_DURATION) {
                hide_oxygen_warning(5.0f);
                warning_5_active = false;  // Allow the warning to trigger again
                warning_5_shown = false;
            }
        }

        auto* container_transform = get_component_safe<Transform2D>(container_id);
        if (!container_transform) {
            return; // No container? done
        }
        Vec2D container_pos = container_transform->position;
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

    // ---------------------------------------------------------
    // MINERAL TANK GUI
    // ---------------------------------------------------------
    void GUI_System::show_mineral_tank_gui()
    {
        // Add scene check at the start
        if (GM.get_current_scene() == 4) {  // Win screen
            return;  // Don't show GUI on win screen
        }

        if (mineral_interaction_container != INVALID_ENTITY_ID) {
            return; // GUI already shown
        }

        // 1) Create E prompt
        mineral_e_prompt = ecs_manager.clone_entity_from_prefab("gui_container");
        if (mineral_e_prompt != INVALID_ENTITY_ID) {
            if (auto* graphics = get_component_safe<Graphics_Component>(mineral_e_prompt)) {
                graphics->model_name = "square";
                graphics->texture_name = "E_Gold_02_Batch_14";  // E key icon
                graphics->color = glm::vec3(1.0f);
            }
            if (auto* transform = get_component_safe<Transform2D>(mineral_e_prompt)) {
                transform->position = Vec2D(mineral_e_prompt_x, original_e_prompt_y);
                transform->scale = Vec2D(50.0f, 50.0f);
                e_prompt_animation_timer = 0.0f;
            }
        }

        // 2) Main container with background texture
        mineral_interaction_container = ecs_manager.clone_entity_from_prefab("gui_container");
        if (mineral_interaction_container != INVALID_ENTITY_ID) {
            auto* container_gui = get_component_safe<GUI_Component>(mineral_interaction_container);
            if (!container_gui) {
                hide_mineral_tank_gui();
                return;
            }
            container_gui->is_container = true;

            if (auto* graphics = get_component_safe<Graphics_Component>(mineral_interaction_container)) {
                graphics->model_name = "square";
                graphics->texture_name = "Mineral_Deposit UI_BG_Batch_14";
                graphics->color = glm::vec3(1.0f);
            }
            if (auto* transform = get_component_safe<Transform2D>(mineral_interaction_container)) {
                transform->position = Vec2D(-350.0f, 120.0f);
                transform->scale = Vec2D(500.0f, 300.0f);
            }
        }

        // 3) Single progress bar
        mineral_progress_bar = ecs_manager.clone_entity_from_prefab("gui_progress_bar");
        if (mineral_progress_bar != INVALID_ENTITY_ID) {
            if (auto* graphics = get_component_safe<Graphics_Component>(mineral_progress_bar)) {
                graphics->model_name = "square";
                graphics->texture_name = "NoTexture";
                // White or Gold color for minerals
                graphics->color = glm::vec3(1.0f, 1.0f, 1.0f);
            }
            if (auto* transform = get_component_safe<Transform2D>(mineral_progress_bar)) {
                float max_width = 435.0f;
                float current_width = max_width * stored_mineral_progress;
                transform->position = Vec2D(-569.5f + (current_width / 2.0f), 110.0f);
                transform->scale = Vec2D(current_width, 52.0f);
            }
            if (auto* gui = get_component_safe<GUI_Component>(mineral_progress_bar)) {
                gui->is_progress_bar = true;
                gui->progress = stored_mineral_progress;
            }
        }

        // 4) Percentage text (xx%)
        mineral_percentage_text = ecs_manager.clone_entity_from_prefab("text_object");
        if (mineral_percentage_text != INVALID_ENTITY_ID) {
            if (auto* text = get_component_safe<Text_Component>(mineral_percentage_text)) {
                text->font_name = DEFAULT_FONT_NAME;
                int percentage = static_cast<int>(stored_mineral_progress * 100);
                text->text = std::to_string(percentage) + "%";
                text->color = glm::vec3(1.0f);
                text->scale = glm::vec2(0.5f, 0.5f);
            }
            if (auto* transform = get_component_safe<Transform2D>(mineral_percentage_text)) {
                transform->position = Vec2D(-350.0f, 40.0f);
                transform->scale = Vec2D(1.0f, 1.0f);
            }
        }

        // 5) Mineral count text: "currentAmount / 50000"
        mineral_deposit_count_text = ecs_manager.clone_entity_from_prefab("text_object");
        if (mineral_deposit_count_text != INVALID_ENTITY_ID) {
            if (auto* text = get_component_safe<Text_Component>(mineral_deposit_count_text)) {
                text->font_name = DEFAULT_FONT_NAME;

                // depositCount = progress * 50000
                int depositCount = static_cast<int>(stored_mineral_progress * 50000.0f);

                // Construct the string "1234 / 50000"
                text->text = std::to_string(depositCount) + " / 50000";
                text->color = glm::vec3(0.0f, 0.0f, 0.0f);
                text->scale = glm::vec2(0.38f, 0.38f);
            }
            if (auto* transform = get_component_safe<Transform2D>(mineral_deposit_count_text)) {
                transform->position = Vec2D(-350.0f, 110.0f);
                transform->scale = Vec2D(0.38f, 0.38f);
            }
        }
    }


    void GUI_System::hide_mineral_tank_gui() {
        // (5) Destroy deposit count text
        if (mineral_deposit_count_text != INVALID_ENTITY_ID) {
            LM.write_log("Destroying mineral deposit count text entity: %d", mineral_deposit_count_text);
            ecs_manager.destroy_entity(mineral_deposit_count_text);
            mineral_deposit_count_text = INVALID_ENTITY_ID;
        }

        // (4) Destroy percentage text
        if (mineral_percentage_text != INVALID_ENTITY_ID) {
            LM.write_log("Destroying mineral percentage text entity: %d", mineral_percentage_text);
            ecs_manager.destroy_entity(mineral_percentage_text);
            mineral_percentage_text = INVALID_ENTITY_ID;
        }

        // (3) Destroy progress bar
        if (mineral_progress_bar != INVALID_ENTITY_ID) {
            LM.write_log("Destroying mineral progress bar entity: %d", mineral_progress_bar);
            ecs_manager.destroy_entity(mineral_progress_bar);
            mineral_progress_bar = INVALID_ENTITY_ID;
        }

        // (2) Destroy container
        if (mineral_interaction_container != INVALID_ENTITY_ID) {
            LM.write_log("Destroying mineral container entity: %d", mineral_interaction_container);
            ecs_manager.destroy_entity(mineral_interaction_container);
            mineral_interaction_container = INVALID_ENTITY_ID;
        }

        // (1) Finally, destroy the E prompt
        if (mineral_e_prompt != INVALID_ENTITY_ID) {
            LM.write_log("Destroying mineral E prompt entity: %d", mineral_e_prompt);
            ecs_manager.destroy_entity(mineral_e_prompt);
            mineral_e_prompt = INVALID_ENTITY_ID;
        }
    }

    void GUI_System::update_mineral_progress(float progress)
    {
        // 1) Clamp and store the new progress
        stored_mineral_progress = std::clamp(progress, 0.0f, 1.0f);

        // 2) Update the progress bar width/position
        if (mineral_progress_bar != INVALID_ENTITY_ID) {
            if (auto* transform = get_component_safe<Transform2D>(mineral_progress_bar)) {
                float max_width = 435.0f;
                float new_width = max_width * stored_mineral_progress;

                transform->scale.x = new_width;
                transform->position.x = -569.5f + (new_width / 2.0f);
            }

            if (auto* gui = get_component_safe<GUI_Component>(mineral_progress_bar)) {
                gui->progress = stored_mineral_progress;
            }
        }

        // 3) Update the percentage text ("xx%")
        if (mineral_percentage_text != INVALID_ENTITY_ID) {
            if (auto* text = get_component_safe<Text_Component>(mineral_percentage_text)) {
                int percentage = static_cast<int>(stored_mineral_progress * 100);
                text->text = std::to_string(percentage) + "%";
            }
        }

        // 4) Update the deposit count text ("X / 50000")
        if (mineral_deposit_count_text != INVALID_ENTITY_ID) {
            if (auto* text = get_component_safe<Text_Component>(mineral_deposit_count_text)) {
                // Example calculation: depositCount = stored_mineral_progress * 50000
                int depositCount = static_cast<int>(stored_mineral_progress * 50000.0f);
                text->text = std::to_string(depositCount) + " / 50000";
            }
        }
    }

    // ---------------------------------------------------------
    // OXYGEN TANK GUI
    // ---------------------------------------------------------
    void GUI_System::show_oxygen_tank_gui()
    {
        // If the oxygen GUI is already shown, do nothing
        if (oxygen_interaction_container != INVALID_ENTITY_ID) {
            return;
        }

        // 1) Create E prompt
        oxygen_e_prompt = ecs_manager.clone_entity_from_prefab("gui_container");
        if (oxygen_e_prompt != INVALID_ENTITY_ID) {
            if (auto* graphics = get_component_safe<Graphics_Component>(oxygen_e_prompt)) {
                graphics->model_name = "square";
                graphics->texture_name = "E_Gold_02_Batch_14"; // E key icon
                graphics->color = glm::vec3(1.0f);
            }
            if (auto* transform = get_component_safe<Transform2D>(oxygen_e_prompt)) {
                transform->position = Vec2D(oxygen_e_prompt_x, original_e_prompt_y);
                transform->scale = Vec2D(50.0f, 50.0f);
            }
            // Reset its animation timer
            oxygen_e_prompt_animation_timer = 0.0f;
        }

        // 2) Main container
        oxygen_interaction_container = ecs_manager.clone_entity_from_prefab("gui_container");
        if (oxygen_interaction_container != INVALID_ENTITY_ID) {
            auto* container_gui = get_component_safe<GUI_Component>(oxygen_interaction_container);
            if (!container_gui) {
                hide_oxygen_tank_gui();
                return;
            }
            container_gui->is_container = true;

            if (auto* graphics = get_component_safe<Graphics_Component>(oxygen_interaction_container)) {
                graphics->model_name = "square";
                graphics->texture_name = "Oxygen_Refill UI_BG_Batch_14";
                graphics->color = glm::vec3(1.0f);
            }
            if (auto* transform = get_component_safe<Transform2D>(oxygen_interaction_container)) {
                transform->position = Vec2D(-350.0f, 120.0f);
                transform->scale = Vec2D(500.0f, 300.0f);
            }
        }

        //
        // == Player Oxygen Bar ==
        // Use current_oxygen_level directly from Game_Manager, convert [0..100] -> [0..1]
        //
        float playerOxygen = GM.get_current_oxygen_level(); // 0..100
        float playerFraction = playerOxygen / 100.0f;       // 0..1
        stored_oxygen_progress1 = playerFraction;

        // We'll set a consistent bar width & height for both bars
        const float BAR_MAX_WIDTH = 437.0f; // same as ship bar
        const float BAR_HEIGHT = 18.0f;
        constexpr float SHIP_MAX = 400.0f;
        // Anchor them similarly in X and just offset Y for the top bar

        //
        // 3) First progress bar (Player Oxygen)
        //
        oxygen_progress_bar1 = ecs_manager.clone_entity_from_prefab("gui_progress_bar");
        if (oxygen_progress_bar1 != INVALID_ENTITY_ID)
        {
            if (auto* graphics = get_component_safe<Graphics_Component>(oxygen_progress_bar1)) {
                graphics->model_name = "square";
                graphics->texture_name = "NoTexture";
                graphics->color = glm::vec3(0.0f, 0.68f, 1.0f);
            }
            if (auto* transform = get_component_safe<Transform2D>(oxygen_progress_bar1)) {
                float current_width = BAR_MAX_WIDTH * stored_oxygen_progress1;
                float bar_y = 134.0f; // Some Y offset

                transform->position = Vec2D(-570.0f + (current_width / 2.0f), bar_y);
                transform->scale = Vec2D(current_width, BAR_HEIGHT);
            }
            if (auto* gui = get_component_safe<GUI_Component>(oxygen_progress_bar1)) {
                gui->is_progress_bar = true;
                gui->progress = stored_oxygen_progress1;
            }
        }

        //
        // 4) Text for first bar (Player Oxygen %)
        //
        oxygen_percentage_text1 = ecs_manager.clone_entity_from_prefab("text_object");
        if (oxygen_percentage_text1 != INVALID_ENTITY_ID)
        {
            if (auto* text = get_component_safe<Text_Component>(oxygen_percentage_text1)) {
                text->font_name = DEFAULT_FONT_NAME;

                // stored_oxygen_progress1 = 1.0 => "100%"
                int percentage = static_cast<int>(stored_oxygen_progress1 * 100);
                text->text = std::to_string(percentage) + "%";
                text->color = glm::vec3(1.0f);
                text->scale = glm::vec2(0.4f, 0.4f);
            }
            if (auto* transform = get_component_safe<Transform2D>(oxygen_percentage_text1)) {
                transform->position = Vec2D(-540.0f, 157.0f);
                transform->scale = Vec2D(0.5f, 0.5f);
            }
        }

        // 5) Second progress bar (Ship Oxygen) in YELLOW
        {
            float currentShipOxy = GM.get_ship_oxygen_level(); // e.g. 400 => full
            // "Used fraction" => how much we've consumed.
            // If ship is still at 400 => used fraction=0 => bar is full
            float usedFraction = (SHIP_MAX - currentShipOxy) / SHIP_MAX;
            // Then "reversed_value" for the fill:
            float reversed_value = 1.0f - usedFraction; // if usedFraction=0 => reversed_value=1 => 100% fill
            float current_width = BAR_MAX_WIDTH * reversed_value;

            // Create the bar
            oxygen_progress_bar2 = ecs_manager.clone_entity_from_prefab("gui_progress_bar");
            if (oxygen_progress_bar2 != INVALID_ENTITY_ID) {
                if (auto* graphics = get_component_safe<Graphics_Component>(oxygen_progress_bar2)) {
                    graphics->model_name = "square";
                    graphics->texture_name = "NoTexture";
                    graphics->color = glm::vec3(1.0f, 1.0f, 0.0f); // Yellow
                }
                if (auto* transform = get_component_safe<Transform2D>(oxygen_progress_bar2)) {
                    // Anchor left at -570.0f, same as your code
                    float bar_y_ship = 106.0f;
                    transform->position = Vec2D(-570.0f + (current_width / 2.0f), bar_y_ship);
                    transform->scale = Vec2D(current_width, BAR_HEIGHT);
                }
                if (auto* gui = get_component_safe<GUI_Component>(oxygen_progress_bar2)) {
                    gui->is_progress_bar = true;
                    // You could store the "usedFraction" or something else if needed,
                    // but it's not strictly required for just drawing the bar.
                }
            }

            // 6) Text for second bar (Ship Oxygen %)
            // If you want 400 => "100%" on first show, do the same reversed approach:
            oxygen_percentage_text2 = ecs_manager.clone_entity_from_prefab("text_object");
            if (oxygen_percentage_text2 != INVALID_ENTITY_ID) {
                if (auto* text = get_component_safe<Text_Component>(oxygen_percentage_text2)) {
                    text->font_name = DEFAULT_FONT_NAME;

                    // reversed_value in [0..1], so reversed_value=1 => 100%
                    int reversed_pct = static_cast<int>(reversed_value * 100);
                    text->text = std::to_string(reversed_pct) + "%";
                    text->color = glm::vec3(1.0f);
                    text->scale = glm::vec2(0.4f, 0.4f);
                }
                if (auto* transform = get_component_safe<Transform2D>(oxygen_percentage_text2)) {
                    transform->position = Vec2D(-540.0f, 78.0f);
                    transform->scale = Vec2D(0.4f, 0.4f);
                }
            }
        }
    }

    void GUI_System::hide_oxygen_tank_gui()
    {
        // The creation order:
        //   1) oxygen_e_prompt
        //   2) oxygen_interaction_container
        //   3) oxygen_progress_bar1
        //   4) oxygen_percentage_text1
        //   5) oxygen_progress_bar2
        //   6) oxygen_percentage_text2
        // So remove in reverse (LIFO):

        // (6) Second bar text
        if (oxygen_percentage_text2 != INVALID_ENTITY_ID) {
            LM.write_log("Destroying oxygen percentage text 2 entity %d", oxygen_percentage_text2);
            ecs_manager.destroy_entity(oxygen_percentage_text2);
            oxygen_percentage_text2 = INVALID_ENTITY_ID;
        }

        // (5) Second progress bar
        if (oxygen_progress_bar2 != INVALID_ENTITY_ID) {
            LM.write_log("Destroying oxygen progress bar 2 entity %d", oxygen_progress_bar2);
            ecs_manager.destroy_entity(oxygen_progress_bar2);
            oxygen_progress_bar2 = INVALID_ENTITY_ID;
        }

        // (4) First bar text
        if (oxygen_percentage_text1 != INVALID_ENTITY_ID) {
            LM.write_log("Destroying oxygen percentage text 1 entity %d", oxygen_percentage_text1);
            ecs_manager.destroy_entity(oxygen_percentage_text1);
            oxygen_percentage_text1 = INVALID_ENTITY_ID;
        }

        // (3) First progress bar
        if (oxygen_progress_bar1 != INVALID_ENTITY_ID) {
            LM.write_log("Destroying oxygen progress bar 1 entity %d", oxygen_progress_bar1);
            ecs_manager.destroy_entity(oxygen_progress_bar1);
            oxygen_progress_bar1 = INVALID_ENTITY_ID;
        }

        // (2) Main container
        if (oxygen_interaction_container != INVALID_ENTITY_ID) {
            LM.write_log("Destroying oxygen container entity %d", oxygen_interaction_container);
            ecs_manager.destroy_entity(oxygen_interaction_container);
            oxygen_interaction_container = INVALID_ENTITY_ID;
        }

        // (1) E prompt
        if (oxygen_e_prompt != INVALID_ENTITY_ID) {
            LM.write_log("Destroying oxygen E prompt entity %d", oxygen_e_prompt);
            ecs_manager.destroy_entity(oxygen_e_prompt);
            oxygen_e_prompt = INVALID_ENTITY_ID;
        }
    }

    //
    // Player Oxygen (#1) - same anchor, same width as in show_oxygen_tank_gui()
    //
    void GUI_System::update_oxygen_progress1(float progress)
    {
        // 'progress' in [0..1]
        stored_oxygen_progress1 = std::clamp(progress, 0.0f, 1.0f);

        if (oxygen_progress_bar1 != INVALID_ENTITY_ID)
        {
            // MATCH the logic/anchors from show_oxygen_tank_gui()
            float BAR_MAX_WIDTH = 437.0f;
            float bar_y = 134.0f;         // e.g. from show_oxygen_tank_gui() for the player bar
            float new_width = BAR_MAX_WIDTH * stored_oxygen_progress1;

            if (auto* transform = get_component_safe<Transform2D>(oxygen_progress_bar1)) {
                transform->scale.x = new_width;
                transform->position.x = -570.0f + (new_width / 2.0f);
                transform->position.y = bar_y;
            }

            if (auto* gui = get_component_safe<GUI_Component>(oxygen_progress_bar1)) {
                gui->progress = stored_oxygen_progress1;
            }

            // Update text for the player bar #1
            if (oxygen_percentage_text1 != INVALID_ENTITY_ID) {
                if (auto* text = get_component_safe<Text_Component>(oxygen_percentage_text1)) {
                    int percentage = static_cast<int>(stored_oxygen_progress1 * 100);
                    text->text = std::to_string(percentage) + "%";
                }
            }
        }
    }


    //
    // Ship Oxygen (#2) - same anchor / width as in show_oxygen_tank_gui()
    //
    void GUI_System::update_oxygen_progress2(float usedFraction)
    {
        // usedFraction in [0..1], 0 => 0% used => bar is 100% left
        // 1 => 100% used => bar is 0% left
        stored_oxygen_progress2 = std::clamp(usedFraction, 0.0f, 1.0f);

        if (oxygen_progress_bar2 != INVALID_ENTITY_ID)
        {
            float BAR_MAX_WIDTH = 437.0f;
            float bar_y = 106.0f;

            // Reversed fill => 1 - usedFraction
            float reversed_value = 1.0f - stored_oxygen_progress2;
            float new_width = BAR_MAX_WIDTH * reversed_value;

            if (auto* transform = get_component_safe<Transform2D>(oxygen_progress_bar2)) {
                transform->scale.x = new_width;
                transform->position.x = -570.0f + (new_width / 2.0f);
                transform->position.y = bar_y;
            }

            // Update text (#2)
            if (oxygen_percentage_text2 != INVALID_ENTITY_ID) {
                if (auto* text = get_component_safe<Text_Component>(oxygen_percentage_text2)) {
                    int reversed_pct = static_cast<int>(reversed_value * 100);
                    text->text = std::to_string(reversed_pct) + "%";
                }
            }
        }
    }

    // ---------------------------------------------------------
    // OXYGEN WARNING GUI
    // ---------------------------------------------------------
    void GUI_System::show_oxygen_warning(float percent) {
        // Determine which warning flags to use
        bool& warning_active = (percent == 50.0f) ? warning_50_active :
            (percent == 20.0f) ? warning_20_active :
            warning_5_active;
        bool& warning_shown = (percent == 50.0f) ? warning_50_shown :
            (percent == 20.0f) ? warning_20_shown :
            warning_5_shown;

        // If warning is already active, don't restart it
        if (warning_active) {
            return;
        }

        warning_active = true;
        warning_shown = true;

        EntityID& container_id = (percent == 50.0f) ? warning_container_50 :
            (percent == 20.0f) ? warning_container_20 :
            warning_container_5;

        EntityID& text_id = (percent == 50.0f) ? warning_text_50 :
            (percent == 20.0f) ? warning_text_20 :
            warning_text_5;

        // Get the appropriate texture and message based on warning level
        std::string texture_name;
        std::string warning_message;
        glm::vec3 text_color;

        if (percent == 50.0f) {
            texture_name = "Purple_Oxy_Warning_Batch_14";
            warning_message = "WARNING: OXYGEN LEVEL 50%";
            text_color = glm::vec3(1.0f, 1.0f, 1.0f);
        }
        else if (percent == 20.0f) {
            texture_name = "Red_Oxy_Warning_Batch_14";
            warning_message = "WARNING: OXYGEN CRITICAL! REFILL OXYGEN!";
            text_color = glm::vec3(1.0f, 1.0f, 1.0f);
        }
        else { // 5%
            texture_name = "Black Oxy Warning_Batch 14";
            warning_message = "WARNING: OXYGEN CRTICAL!";
            text_color = glm::vec3(1.0f, 1.0f, 1.0f);
        }

        // Create warning container
        container_id = ecs_manager.clone_entity_from_prefab("gui_container");
        if (container_id != INVALID_ENTITY_ID) {
            auto* container_gui = get_component_safe<GUI_Component>(container_id);
            if (!container_gui) {
                hide_oxygen_warning(percent);
                return;
            }
            container_gui->is_container = true;

            if (auto* graphics = get_component_safe<Graphics_Component>(container_id)) {
                graphics->model_name = "square";
                graphics->texture_name = texture_name;
                graphics->color = glm::vec3(1.0f);
            }

            // Position warning - same for all levels
            if (auto* transform = get_component_safe<Transform2D>(container_id)) {
                transform->position = Vec2D(0.0f, 200.0f);
                transform->scale = Vec2D(2000.0f, 50.0f);
            }
        }

        // Create warning text
        text_id = ecs_manager.clone_entity_from_prefab("text_object");
        if (text_id != INVALID_ENTITY_ID) {
            if (auto* text = get_component_safe<Text_Component>(text_id)) {
                text->font_name = DEFAULT_FONT_NAME;  // Added font name
                text->text = warning_message;
                text->color = text_color;
                text->scale = glm::vec2(0.7f, 0.7f);  // Matched scale style
            }
            if (auto* transform = get_component_safe<Transform2D>(text_id)) {
                transform->position = Vec2D(0.0f, 195.0f);
                transform->scale = Vec2D(0.7f, 0.7f);  // Matched transform scale
            }
        }

        if (percent == 50.0f) warning_50_shown = true;
        else if (percent == 20.0f) warning_20_shown = true;
        else if (percent == 5.0f) warning_5_shown = true;
    }

    void GUI_System::hide_oxygen_warning(float percent) {
        EntityID& text_id = (percent == 50.0f) ? warning_text_50 :
            (percent == 20.0f) ? warning_text_20 :
            warning_text_5;

        EntityID& container_id = (percent == 50.0f) ? warning_container_50 :
            (percent == 20.0f) ? warning_container_20 :
            warning_container_5;

        if (text_id != INVALID_ENTITY_ID) {
            ecs_manager.destroy_entity(text_id);
            text_id = INVALID_ENTITY_ID;
        }
        if (container_id != INVALID_ENTITY_ID) {
            ecs_manager.destroy_entity(container_id);
            container_id = INVALID_ENTITY_ID;
        }
    }

} // namespace lof
