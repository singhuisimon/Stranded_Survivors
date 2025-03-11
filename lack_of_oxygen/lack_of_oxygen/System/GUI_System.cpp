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
#include "../System/Collision_System.h"
#include "../Utility/Entity_Selector_Helper.h"

// Include Utility headers
#include "../Utility/Constant.h"

namespace lof {
    GUI_System::GUI_System(ECS_Manager& ecs_manager)
        : ecs_manager(ecs_manager)
        , warning_text_50_name("")
        , warning_container_50_name("")
        , warning_text_20_name("")
        , warning_container_20_name("")
        , warning_text_5_name("")
        , warning_container_5_name("")
    {
        container_id = INVALID_ENTITY_ID;
        background_bar_id = INVALID_ENTITY_ID;
        progress_bar_id = INVALID_ENTITY_ID;

        pause_button_hover_states["resume_button"] = false;
        pause_button_hover_states["restart_button"] = false;
        pause_button_hover_states["main_menu_button"] = false;

        // Set up the required components
        signature.set(ecs_manager.get_component_id<Transform2D>());
        signature.set(ecs_manager.get_component_id<Graphics_Component>());
        signature.set(ecs_manager.get_component_id<GUI_Component>());
    }

    void GUI_System::reset_all_game_state() {
        // Reset the core progress tracker that triggers win condition
        stored_mineral_progress = 0.0f;

        // Reset stored goal percentage using Game Manager
        GM.set_stored_goal_percentage(0.0f);

        // Reset the total deposited minerals in Collision System
        for (auto& system : ECSM.get_systems()) {
            if (auto* collision_system = dynamic_cast<Collision_System*>(system.get())) {
                collision_system->reset_deposited_minerals();
                break;
            }
        }

        // Hide all GUI elements with logging
        LM.write_log("GUI_System::reset_all_game_state(): Hiding GUI elements...");
        hide_mineral_tank_gui();
        hide_oxygen_tank_gui();
        hide_oxygen_warning(50.0f);
        hide_oxygen_warning(20.0f);
        hide_oxygen_warning(5.0f);

        // Log initial state of entities before reset
        LM.write_log("GUI_System::reset_all_game_state(): Current entity states - "
            "mineral_e_prompt: %d, oxygen_e_prompt: %d",
            mineral_e_prompt, oxygen_e_prompt);

        // Reset GUI state variables
        mineral_e_prompt = INVALID_ENTITY_ID;
        oxygen_e_prompt = INVALID_ENTITY_ID;
        mineral_interaction_container = INVALID_ENTITY_ID;
        oxygen_interaction_container = INVALID_ENTITY_ID;
        oxygen_progress_bar1 = INVALID_ENTITY_ID;
        oxygen_progress_bar2 = INVALID_ENTITY_ID;
        oxygen_percentage_text1 = INVALID_ENTITY_ID;
        oxygen_percentage_text2 = INVALID_ENTITY_ID;

        // Reset gameplay values with logging
        LM.write_log("GUI_System::reset_all_game_state(): Resetting gameplay values...");
        GM.set_current_oxygen_level(100.0f);
        GM.set_ship_oxygen_level(400.0f);

        // Reset mineral count text
        EntityID text_entity = ECSM.find_entity_by_name("top_ui_mineral_count_text");
        if (text_entity != INVALID_ENTITY_ID && ECSM.has_component<Text_Component>(text_entity)) {
            auto& text_comp = ECSM.get_component<Text_Component>(text_entity);
            text_comp.text = "000000";
        }

        // Reset goal percentage text
        EntityID goal_text_entity = ECSM.find_entity_by_name("top_ui_goal_percentage_text");
        if (goal_text_entity != INVALID_ENTITY_ID && ECSM.has_component<Text_Component>(goal_text_entity)) {
            auto& text_comp = ECSM.get_component<Text_Component>(goal_text_entity);
            text_comp.text = "00%";
        }

        // Reset timer text
        EntityID timer_text_entity = ECSM.find_entity_by_name("top_ui_timer_count_text");
        if (timer_text_entity != INVALID_ENTITY_ID && ECSM.has_component<Text_Component>(timer_text_entity)) {
            auto& text_comp = ECSM.get_component<Text_Component>(timer_text_entity);
            text_comp.text = "300";
            GM.reset_timer();
        }

        // Reset warning states
        LM.write_log("GUI_System::reset_all_game_state(): Resetting warning states...");
        warning_50_active = false;
        warning_20_active = false;
        warning_5_active = false;
        warning_50_shown = false;
        warning_20_shown = false;
        warning_5_shown = false;
        warning_50_display_time = 0.0f;
        warning_20_display_time = 0.0f;
        warning_5_display_time = 0.0f;

        LM.write_log("GUI_System::reset_all_game_state(): Reset complete");
    }


    void GUI_System::update(float delta_time) {
        // Log to confirm the pause state is being detected
        LM.write_log("GUI_System::update: is_paused=%d", GM.is_paused());

        // Win screen check
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

        // Check game over first
        if (game_over_shown) {
            check_game_over_button_collision(delta_time);

            // Still process E prompt bobbing animations during game over
            if (mineral_e_prompt != INVALID_ENTITY_ID) {
                e_prompt_animation_timer += delta_time;
                if (auto* transform = get_component_safe<Transform2D>(mineral_e_prompt)) {
                    float offset = std::sin(e_prompt_animation_timer * E_PROMPT_SPEED) * E_PROMPT_AMPLITUDE;
                    transform->position.y = original_e_prompt_y + offset;
                    transform->position.x = mineral_e_prompt_x; // keep X constant
                }
            }

            if (oxygen_e_prompt != INVALID_ENTITY_ID) {
                oxygen_e_prompt_animation_timer += delta_time;
                if (auto* transform = get_component_safe<Transform2D>(oxygen_e_prompt)) {
                    float offset = std::sin(oxygen_e_prompt_animation_timer * E_PROMPT_SPEED) * E_PROMPT_AMPLITUDE;
                    transform->position.y = original_e_prompt_y + offset;
                    transform->position.x = oxygen_e_prompt_x; // keep X constant
                }
            }

            return; // Skip remaining updates when game over is shown
        }
        else {
            // Update pause menu button interactions if the game is paused
            if (GM.is_paused()) {
                check_pause_menu_button_collision(delta_time);

                // Process minimal animations during pause
                if (mineral_e_prompt != INVALID_ENTITY_ID) {
                    e_prompt_animation_timer += delta_time;
                    if (auto* transform = get_component_safe<Transform2D>(mineral_e_prompt)) {
                        float offset = std::sin(e_prompt_animation_timer * E_PROMPT_SPEED) * E_PROMPT_AMPLITUDE;
                        transform->position.y = original_e_prompt_y + offset;
                        transform->position.x = mineral_e_prompt_x; // keep X constant
                    }
                }

                if (oxygen_e_prompt != INVALID_ENTITY_ID) {
                    oxygen_e_prompt_animation_timer += delta_time;
                    if (auto* transform = get_component_safe<Transform2D>(oxygen_e_prompt)) {
                        float offset = std::sin(oxygen_e_prompt_animation_timer * E_PROMPT_SPEED) * E_PROMPT_AMPLITUDE;
                        transform->position.y = original_e_prompt_y + offset;
                        transform->position.x = oxygen_e_prompt_x; // keep X constant
                    }
                }

                // Skip remaining updates when paused
                return;
            }
        }

        // Normal gameplay updates (not paused, not game over)

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
        if (stored_mineral_progress * 50000 >= 50000) {
            LM.write_log("Win condition met: %f minerals collected", stored_mineral_progress * 50000.0f);

            reset_all_game_state();

            LM.write_log("Reset met: %f minerals collected", stored_mineral_progress * 50000.0f);

            // Load win screen
            const std::string SCENES = "Scenes";
            std::string scene_file = "win_screen.scn";
            std::string scene_path = ASM.get_full_path(SCENES, scene_file);

            if (SM.load_scene(scene_path.c_str())) {
                auto& camera = GFXM.get_camera();
                camera.pos_x = DEFAULT_CAMERA_POS_X;
                camera.pos_y = DEFAULT_CAMERA_POS_Y;

                ADM.stop_mastergroup();

                GM.set_current_scene(4);
                IMGUIM.set_current_file_shown(scene_file);
                return;
            }
            else {
                LM.write_log("Failed to load scene file: %s", scene_path.c_str());
            }
        }

        // --------------------------------------------------------
        // Update oxygen bars automatically every second
        // --------------------------------------------------------
        // Only update if the game is not paused (already ensured by the returns above).
        // However, this extra check ensures that if you later reorganize this code,
        // we still won't update oxygen while paused.
        if (!GM.is_paused() && oxygen_interaction_container != INVALID_ENTITY_ID) {
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
                warning_50_active = false;
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
                warning_20_active = false;
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
                warning_5_active = false;
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
        // Check current scene
        if (GM.get_current_scene() == 4) {  // Win screen
            return;  // Don't show GUI on win screen
        }

        // Check if container already exists
        if (ecs_manager.find_entity_by_name("mineral_interaction_container") != INVALID_ENTITY_ID) {
            return; // GUI already shown
        }

        // 1) Create E prompt
        EntityID e_prompt = ecs_manager.clone_entity_from_prefab("gui_container", "mineral_e_prompt");
        if (e_prompt != INVALID_ENTITY_ID) {
            if (auto* graphics = get_component_safe<Graphics_Component>(e_prompt)) {
                graphics->model_name = "square";
                graphics->texture_name = "E_Gold_02_Batch_14";  // E key icon
                graphics->color = glm::vec4(1.0f);
            }
            if (auto* transform = get_component_safe<Transform2D>(e_prompt)) {
                transform->position = Vec2D(mineral_e_prompt_x, original_e_prompt_y);
                transform->scale = Vec2D(50.0f, 50.0f);
                e_prompt_animation_timer = 0.0f;
            }
        }

        // 2) Main container with background texture
        EntityID container = ecs_manager.clone_entity_from_prefab("gui_container", "mineral_interaction_container");
        if (container != INVALID_ENTITY_ID) {
            auto* container_gui = get_component_safe<GUI_Component>(container);
            if (!container_gui) {
                hide_mineral_tank_gui();
                return;
            }
            container_gui->is_container = true;

            if (auto* graphics = get_component_safe<Graphics_Component>(container)) {
                graphics->model_name = "square";
                graphics->texture_name = "UI_MineralsFill_1920x1080_v2";
                graphics->color = glm::vec4(1.0f);
            }
            if (auto* transform = get_component_safe<Transform2D>(container)) {
                transform->position = Vec2D(0.0f, 0.0f);
                transform->scale = Vec2D(1980.0f, 1020.0f);
            }
        }

        // 3) Progress bar
        EntityID progress_bar = ecs_manager.clone_entity_from_prefab("gui_progress_bar", "mineral_progress_bar");
        if (progress_bar != INVALID_ENTITY_ID) {
            if (auto* graphics = get_component_safe<Graphics_Component>(progress_bar)) {
                graphics->model_name = "square";
                graphics->texture_name = "NoTexture";
                graphics->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            }
            if (auto* transform = get_component_safe<Transform2D>(progress_bar)) {
                float max_width = 600.0f;
                float current_width = max_width * stored_mineral_progress;
                transform->position = Vec2D(-642.0f + (current_width / 2.0f), 60.0f);
                transform->scale = Vec2D(current_width, 42.0f);
            }
            if (auto* gui = get_component_safe<GUI_Component>(progress_bar)) {
                gui->is_progress_bar = true;
                gui->progress = stored_mineral_progress;
            }
        }

        // 4) Percentage text
        EntityID percentage_text = ecs_manager.clone_entity_from_prefab("text_object", "mineral_percentage_text");
        if (percentage_text != INVALID_ENTITY_ID) {
            if (auto* text = get_component_safe<Text_Component>(percentage_text)) {
                text->font_name = DEFAULT_FONT_NAME;
                int percentage = static_cast<int>(stored_mineral_progress * 100);
                text->text = std::to_string(percentage) + "%";
                text->color = glm::vec3(1.0f);
                text->scale = glm::vec2(0.5f, 0.5f);
            }
            if (auto* transform = get_component_safe<Transform2D>(percentage_text)) {
                transform->position = Vec2D(-350.0f, 10.0f);
                transform->scale = Vec2D(1.0f, 1.0f);
            }
        }

        // 5) Deposit count text
        EntityID count_text = ecs_manager.clone_entity_from_prefab("text_object", "mineral_deposit_count_text");
        if (count_text != INVALID_ENTITY_ID) {
            if (auto* text = get_component_safe<Text_Component>(count_text)) {
                text->font_name = DEFAULT_FONT_NAME;
                int depositCount = static_cast<int>(stored_mineral_progress * 50000.0f);
                text->text = std::to_string(depositCount) + " / 50000";
                text->color = glm::vec3(0.0f, 0.0f, 0.0f);
                text->scale = glm::vec2(0.38f, 0.38f);
            }
            if (auto* transform = get_component_safe<Transform2D>(count_text)) {
                transform->position = Vec2D(-350.0f, 57.0f);
                transform->scale = Vec2D(0.5f, 0.5f);
            }
        }
    }

    void GUI_System::hide_mineral_tank_gui() {
        // Order matters for clean removal - destroy from bottom up
        const std::vector<std::string> entity_names = {
            "mineral_deposit_count_text",
            "mineral_percentage_text",
            "mineral_progress_bar",
            "mineral_interaction_container",
            "mineral_e_prompt"
        };

        for (const auto& name : entity_names) {
            EntityID entity = ecs_manager.find_entity_by_name(name);
            if (entity != INVALID_ENTITY_ID) {
                ecs_manager.destroy_entity(entity);
            }
        }
    }

    void GUI_System::update_mineral_progress(float progress)
    {
        if (GM.get_current_scene() == 0) { // Main menu
            stored_mineral_progress = 0.0f;
            LM.write_log("GUI_System::update_mineral_progress(): Reset progress on main menu");
            return;
        }

        // 1) Clamp and store the new progress
        stored_mineral_progress = std::clamp(progress, 0.0f, 1.0f);

        // 2) Update the progress bar width/position
        EntityID progress_bar = ecs_manager.find_entity_by_name("mineral_progress_bar");
        if (progress_bar != INVALID_ENTITY_ID) {
            if (auto* transform = get_component_safe<Transform2D>(progress_bar)) {
                float max_width = 600.0f;
                float new_width = max_width * stored_mineral_progress;
                transform->scale.x = new_width;
                transform->position.x = -642.0f + (new_width / 2.0f);
            }

            if (auto* gui = get_component_safe<GUI_Component>(progress_bar)) {
                gui->progress = stored_mineral_progress;
            }
        }

        // 3) Update the percentage text
        EntityID percentage_text = ecs_manager.find_entity_by_name("mineral_percentage_text");
        if (percentage_text != INVALID_ENTITY_ID) {
            if (auto* text = get_component_safe<Text_Component>(percentage_text)) {
                int percentage = static_cast<int>(stored_mineral_progress * 100);
                text->text = std::to_string(percentage) + "%";
            }
        }

        // 4) Update the deposit count text
        EntityID count_text = ecs_manager.find_entity_by_name("mineral_deposit_count_text");
        if (count_text != INVALID_ENTITY_ID) {
            if (auto* text = get_component_safe<Text_Component>(count_text)) {
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
        // Check if the oxygen GUI is already shown
        if (!oxygen_container_name.empty()) {
            return;
        }

        // Generate unique names for this instance
        oxygen_e_prompt_name = "oxygen_e_prompt";
        oxygen_container_name = "oxygen_container";
        oxygen_progress_bar1_name = "oxygen_progress_bar1";
        oxygen_percentage_text1_name = "oxygen_percentage_text1";
        oxygen_progress_bar2_name = "oxygen_progress_bar2";
        oxygen_percentage_text2_name = "oxygen_percentage_text2";

        // 1) Create E prompt
        EntityID e_prompt = ecs_manager.clone_entity_from_prefab("gui_container", oxygen_e_prompt_name);
        if (e_prompt != INVALID_ENTITY_ID) {
            if (auto* graphics = get_component_safe<Graphics_Component>(e_prompt)) {
                graphics->model_name = "square";
                graphics->texture_name = "E_Gold_02_Batch_14";
                graphics->color = glm::vec4(1.0f);
            }
            if (auto* transform = get_component_safe<Transform2D>(e_prompt)) {
                transform->position = Vec2D(oxygen_e_prompt_x, original_e_prompt_y);
                transform->scale = Vec2D(50.0f, 50.0f);
            }
            oxygen_e_prompt_animation_timer = 0.0f;
        }

        // 2) Main container
        EntityID container = ecs_manager.clone_entity_from_prefab("gui_container", oxygen_container_name);
        if (container != INVALID_ENTITY_ID) {
            auto* container_gui = get_component_safe<GUI_Component>(container);
            if (!container_gui) {
                hide_oxygen_tank_gui();
                return;
            }
            container_gui->is_container = true;

            if (auto* graphics = get_component_safe<Graphics_Component>(container)) {
                graphics->model_name = "square";
                graphics->texture_name = "UI_OxygenRefill_1920x1080_v2";
                graphics->color = glm::vec4(1.0f);
            }
            if (auto* transform = get_component_safe<Transform2D>(container)) {
                transform->position = Vec2D(0.0f, 0.0f);
                transform->scale = Vec2D(1980.0f, 1020.0f);
            }
        }

        // == Player Oxygen Bar
        float playerOxygen = GM.get_current_oxygen_level();
        float playerFraction = playerOxygen / 100.0f;
        stored_oxygen_progress1 = playerFraction;

        const float BAR_MAX_WIDTH = 620.0f;
        const float BAR_HEIGHT = 14.0f;
        constexpr float SHIP_MAX = 400.0f;

        // 3) First progress bar (Player Oxygen)
        EntityID progress_bar1 = ecs_manager.clone_entity_from_prefab("gui_progress_bar", oxygen_progress_bar1_name);
        if (progress_bar1 != INVALID_ENTITY_ID) {
            if (auto* graphics = get_component_safe<Graphics_Component>(progress_bar1)) {
                graphics->model_name = "square";
                graphics->texture_name = "NoTexture";
                graphics->color = glm::vec4(0.0f, 0.68f, 1.0f, 1.0f);
            }
            if (auto* transform = get_component_safe<Transform2D>(progress_bar1)) {
                float current_width = BAR_MAX_WIDTH * stored_oxygen_progress1;
                float bar_y = 78.5f;
                transform->position = Vec2D(-657.0f + (current_width / 2.0f), bar_y);
                transform->scale = Vec2D(current_width, BAR_HEIGHT);
            }
            if (auto* gui = get_component_safe<GUI_Component>(progress_bar1)) {
                gui->is_progress_bar = true;
                gui->progress = stored_oxygen_progress1;
            }
        }

        // 4) Text for first bar (Player Oxygen %)
        EntityID percentage_text1 = ecs_manager.clone_entity_from_prefab("text_object", oxygen_percentage_text1_name);
        if (percentage_text1 != INVALID_ENTITY_ID) {
            if (auto* text = get_component_safe<Text_Component>(percentage_text1)) {
                text->font_name = DEFAULT_FONT_NAME;
                int percentage = static_cast<int>(stored_oxygen_progress1 * 100);
                text->text = std::to_string(percentage) + "%";
                text->color = glm::vec3(1.0f);
                text->scale = glm::vec2(0.4f, 0.4f);
            }
            if (auto* transform = get_component_safe<Transform2D>(percentage_text1)) {
                transform->position = Vec2D(-623.0f, 100.0f);
                transform->scale = Vec2D(0.5f, 0.5f);
            }
        }

        // 5) Second progress bar (Ship Oxygen)
        float currentShipOxy = GM.get_ship_oxygen_level();
        float usedFraction = (SHIP_MAX - currentShipOxy) / SHIP_MAX;
        float reversed_value = 1.0f - usedFraction;
        float current_width = BAR_MAX_WIDTH * reversed_value;

        EntityID progress_bar2 = ecs_manager.clone_entity_from_prefab("gui_progress_bar", oxygen_progress_bar2_name);
        if (progress_bar2 != INVALID_ENTITY_ID) {
            if (auto* graphics = get_component_safe<Graphics_Component>(progress_bar2)) {
                graphics->model_name = "square";
                graphics->texture_name = "NoTexture";
                graphics->color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
            }
            if (auto* transform = get_component_safe<Transform2D>(progress_bar2)) {
                float bar_y_ship = 53.3f;
                transform->position = Vec2D(-657.0f + (current_width / 2.0f), bar_y_ship);
                transform->scale = Vec2D(current_width, BAR_HEIGHT);
            }
            if (auto* gui = get_component_safe<GUI_Component>(progress_bar2)) {
                gui->is_progress_bar = true;
            }
        }

        // 6) Text for second bar (Ship Oxygen %)
        EntityID percentage_text2 = ecs_manager.clone_entity_from_prefab("text_object", oxygen_percentage_text2_name);
        if (percentage_text2 != INVALID_ENTITY_ID) {
            if (auto* text = get_component_safe<Text_Component>(percentage_text2)) {
                text->font_name = DEFAULT_FONT_NAME;
                int reversed_pct = static_cast<int>(reversed_value * 100);
                text->text = std::to_string(reversed_pct) + "%";
                text->color = glm::vec3(1.0f);
                text->scale = glm::vec2(0.4f, 0.4f);
            }
            if (auto* transform = get_component_safe<Transform2D>(percentage_text2)) {
                transform->position = Vec2D(-620.0f, 25.0f);
                transform->scale = Vec2D(0.4f, 0.4f);
            }
        }
    }

    void GUI_System::hide_oxygen_tank_gui()
    {
        const std::vector<std::string*> entity_names = {
            &oxygen_percentage_text2_name,
            &oxygen_progress_bar2_name,
            &oxygen_percentage_text1_name,
            &oxygen_progress_bar1_name,
            &oxygen_container_name,
            &oxygen_e_prompt_name
        };

        for (auto* name_ptr : entity_names) {
            if (!name_ptr->empty()) {
                EntityID entity = ecs_manager.find_entity_by_name(*name_ptr);
                if (entity != INVALID_ENTITY_ID) {
                    ecs_manager.destroy_entity(entity);
                }
                name_ptr->clear();
            }
        }
    }

    void GUI_System::update_oxygen_progress1(float progress)
    {
        stored_oxygen_progress1 = std::clamp(progress, 0.0f, 1.0f);

        EntityID progress_bar1 = ecs_manager.find_entity_by_name(oxygen_progress_bar1_name);
        if (progress_bar1 != INVALID_ENTITY_ID) {
            float BAR_MAX_WIDTH = 620.0f;
            float bar_y = 78.5f;
            float new_width = BAR_MAX_WIDTH * stored_oxygen_progress1;

            if (auto* transform = get_component_safe<Transform2D>(progress_bar1)) {
                transform->scale.x = new_width;
                transform->position.x = -657.0f + (new_width / 2.0f);
                transform->position.y = bar_y;
            }

            if (auto* gui = get_component_safe<GUI_Component>(progress_bar1)) {
                gui->progress = stored_oxygen_progress1;
            }

            EntityID percentage_text1 = ecs_manager.find_entity_by_name(oxygen_percentage_text1_name);
            if (percentage_text1 != INVALID_ENTITY_ID) {
                if (auto* text = get_component_safe<Text_Component>(percentage_text1)) {
                    int percentage = static_cast<int>(stored_oxygen_progress1 * 100);
                    text->text = std::to_string(percentage) + "%";
                }
            }
        }
    }

    void GUI_System::update_oxygen_progress2(float usedFraction)
    {
        stored_oxygen_progress2 = std::clamp(usedFraction, 0.0f, 1.0f);

        EntityID progress_bar2 = ecs_manager.find_entity_by_name(oxygen_progress_bar2_name);
        if (progress_bar2 != INVALID_ENTITY_ID) {
            float BAR_MAX_WIDTH = 620.0f;
            float bar_y = 53.3f;
            float reversed_value = 1.0f - stored_oxygen_progress2;
            float new_width = BAR_MAX_WIDTH * reversed_value;

            if (auto* transform = get_component_safe<Transform2D>(progress_bar2)) {
                transform->scale.x = new_width;
                transform->position.x = -657.0f + (new_width / 2.0f);
                transform->position.y = bar_y;
            }

            EntityID percentage_text2 = ecs_manager.find_entity_by_name(oxygen_percentage_text2_name);
            if (percentage_text2 != INVALID_ENTITY_ID) {
                if (auto* text = get_component_safe<Text_Component>(percentage_text2)) {
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
        // Get the appropriate warning names references
        std::string& text_name = (percent == 50.0f) ? warning_text_50_name :
            (percent == 20.0f) ? warning_text_20_name :
            warning_text_5_name;

        std::string& container_name = (percent == 50.0f) ? warning_container_50_name :
            (percent == 20.0f) ? warning_container_20_name :
            warning_container_5_name;

        // If warning is already active, don't recreate it
        if (!text_name.empty() || !container_name.empty()) {
            return;
        }

        // Get the appropriate texture and message based on warning level
        std::string texture_name;
        std::string warning_message;
        glm::vec3 text_color = glm::vec3(1.0f, 1.0f, 1.0f);

        if (percent == 50.0f) {
            texture_name = "Purple_Oxy_Warning_Batch_14";
            warning_message = "WARNING: OXYGEN LEVEL 50%";
        }
        else if (percent == 20.0f) {
            texture_name = "Red_Oxy_Warning_Batch_14";
            warning_message = "WARNING: OXYGEN CRITICAL! REFILL OXYGEN!";
        }
        else { // 5%
            texture_name = "Black Oxy Warning_Batch 14";
            warning_message = "WARNING: OXYGEN CRITICAL!";
        }

        // Create warning container
        EntityID warning_container_id = ecs_manager.clone_entity_from_prefab("gui_container");
        if (warning_container_id != INVALID_ENTITY_ID) {
            // Create unique name for container
            std::string unique_container_name = "warning_container_" + std::to_string(static_cast<int>(percent));
            ecs_manager.update_entity_name(warning_container_id, unique_container_name);
            container_name = unique_container_name;

            auto* container_gui = get_component_safe<GUI_Component>(warning_container_id);
            if (!container_gui) {
                hide_oxygen_warning(percent);
                return;
            }
            container_gui->is_container = true;

            if (auto* graphics = get_component_safe<Graphics_Component>(warning_container_id)) {
                graphics->model_name = "square";
                graphics->texture_name = texture_name;
                graphics->color = glm::vec4(1.0f);
            }

            if (auto* transform = get_component_safe<Transform2D>(warning_container_id)) {
                transform->position = Vec2D(0.0f, 200.0f);
                transform->scale = Vec2D(2000.0f, 50.0f);
            }
        }

        // Create warning text
        EntityID warning_text_id = ecs_manager.clone_entity_from_prefab("text_object");
        if (warning_text_id != INVALID_ENTITY_ID) {
            // Create unique name for text
            std::string unique_text_name = "warning_text_" + std::to_string(static_cast<int>(percent));
            ecs_manager.update_entity_name(warning_text_id, unique_text_name);
            text_name = unique_text_name;

            if (auto* text = get_component_safe<Text_Component>(warning_text_id)) {
                text->font_name = DEFAULT_FONT_NAME;
                text->text = warning_message;
                text->color = text_color;
                text->scale = glm::vec2(0.7f, 0.7f);
            }
            if (auto* transform = get_component_safe<Transform2D>(warning_text_id)) {
                transform->position = Vec2D(0.0f, 195.0f);
                transform->scale = Vec2D(0.7f, 0.7f);
            }
        }

        if (percent == 50.0f) warning_50_shown = true;
        else if (percent == 20.0f) warning_20_shown = true;
        else if (percent == 5.0f) warning_5_shown = true;
    }

    void GUI_System::hide_oxygen_warning(float percent) {
        // Get the appropriate names
        std::string& text_name = (percent == 50.0f) ? warning_text_50_name :
            (percent == 20.0f) ? warning_text_20_name :
            warning_text_5_name;

        std::string& container_name = (percent == 50.0f) ? warning_container_50_name :
            (percent == 20.0f) ? warning_container_20_name :
            warning_container_5_name;

        // Find and destroy entities by name
        EntityID warning_text_id = ecs_manager.find_entity_by_name(text_name);
        if (warning_text_id != INVALID_ENTITY_ID) {
            ecs_manager.destroy_entity(warning_text_id);
        }

        EntityID warning_container_id = ecs_manager.find_entity_by_name(container_name);
        if (warning_container_id != INVALID_ENTITY_ID) {
            ecs_manager.destroy_entity(warning_container_id);
        }

        // Clear the stored names
        text_name.clear();
        container_name.clear();

        // Reset the appropriate warning flags
        if (percent == 50.0f) {
            warning_50_active = false;
            warning_50_shown = false;
        }
        else if (percent == 20.0f) {
            warning_20_active = false;
            warning_20_shown = false;
        }
        else if (percent == 5.0f) {
            warning_5_active = false;
            warning_5_shown = false;
        }
    }

    void GUI_System::show_pause_menu() {
        // Don't show if already shown
        if (!pause_menu_entities.empty()) {
            return;
        }

        LM.write_log("GUI_System::show_pause_menu(): Creating pause menu UI");

        // Create a semi-transparent background overlay
        EntityID overlay = ecs_manager.clone_entity_from_prefab("gui_container", "pause_overlay");
        if (overlay != INVALID_ENTITY_ID) {
            if (auto* graphics = get_component_safe<Graphics_Component>(overlay)) {
                graphics->model_name = "square";
                graphics->texture_name = "Pause_Screen_Batch_19";
                graphics->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            }
            if (auto* transform = get_component_safe<Transform2D>(overlay)) {
                transform->position = Vec2D(0.0f, -50.0f);
                transform->scale = Vec2D(1980.0f, 1180.0f);
            }
            pause_menu_entities["overlay"] = overlay;
        }

        // Create Resume button
        EntityID resume_button = ecs_manager.clone_entity_from_prefab("gui_container", "resume_button");
        if (resume_button != INVALID_ENTITY_ID) {
            if (auto* graphics = get_component_safe<Graphics_Component>(resume_button)) {
                graphics->model_name = "square";
                graphics->texture_name = "Resume_Batch_14_NORMAL";
                graphics->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            }
            if (auto* transform = get_component_safe<Transform2D>(resume_button)) {
                transform->position = Vec2D(0.0f, 0.0f);
                transform->scale = Vec2D(200.0f, 80.0f);
            }

            // Add empty Audio Component - needed for hover detection
            Audio_Component audio_comp;
            ecs_manager.add_component(resume_button, audio_comp);

            pause_menu_entities["resume"] = resume_button;
        }

        // Create Restart button
        EntityID restart_button = ecs_manager.clone_entity_from_prefab("gui_container", "restart_button");
        if (restart_button != INVALID_ENTITY_ID) {
            if (auto* graphics = get_component_safe<Graphics_Component>(restart_button)) {
                graphics->model_name = "square";
                graphics->texture_name = "Restart_Batch_14_NORMAL";
                graphics->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            }
            if (auto* transform = get_component_safe<Transform2D>(restart_button)) {
                transform->position = Vec2D(0.0f, -100.0f);
                transform->scale = Vec2D(200.0f, 80.0f);
            }

            // Add empty Audio Component - needed for hover detection
            Audio_Component audio_comp;
            ecs_manager.add_component(restart_button, audio_comp);

            pause_menu_entities["restart"] = restart_button;
        }

        // Create Main Menu button
        EntityID main_menu_button = ecs_manager.clone_entity_from_prefab("gui_container", "main_menu_button");
        if (main_menu_button != INVALID_ENTITY_ID) {
            if (auto* graphics = get_component_safe<Graphics_Component>(main_menu_button)) {
                graphics->model_name = "square";
                graphics->texture_name = "Main_Menu_Batch_14_NORMAL";
                graphics->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            }
            if (auto* transform = get_component_safe<Transform2D>(main_menu_button)) {
                transform->position = Vec2D(0.0f, -200.0f);
                transform->scale = Vec2D(200.0f, 80.0f);
            }

            // Add empty Audio Component - needed for hover detection
            Audio_Component audio_comp;
            ecs_manager.add_component(main_menu_button, audio_comp);

            pause_menu_entities["main_menu"] = main_menu_button;
        }
    }

    void GUI_System::hide_pause_menu() {
        LM.write_log("GUI_System::hide_pause_menu(): Removing pause menu UI in specific order");

        // First remove the overlay/background texture
        if (pause_menu_entities.find("overlay") != pause_menu_entities.end()) {
            EntityID overlay_id = pause_menu_entities["overlay"];
            if (overlay_id != INVALID_ENTITY_ID) {
                try {
                    ecs_manager.destroy_entity(overlay_id);
                    LM.write_log("Destroyed pause menu overlay (ID: %u)", overlay_id);
                }
                catch (const std::exception& e) {
                    LM.write_log("Error destroying overlay (ID: %u): %s", overlay_id, e.what());
                }
            }
        }

        // Then remove buttons in specific order
        const std::vector<std::string> button_order = {
            "resume",
            "restart",
            "main_menu"
        };

        for (const auto& button_key : button_order) {
            if (pause_menu_entities.find(button_key) != pause_menu_entities.end()) {
                EntityID button_id = pause_menu_entities[button_key];
                if (button_id != INVALID_ENTITY_ID) {
                    ecs_manager.destroy_entity(button_id);
                    LM.write_log("Destroyed %s button (ID: %u)", button_key.c_str(), button_id);
                }
            }
        }

        // Also try to find buttons by their entity names
        const std::vector<std::string> button_names = {
            "resume_button",
            "restart_button",
            "main_menu_button"
        };

        for (const auto& name : button_names) {
            EntityID entity_id = ecs_manager.find_entity_by_name(name);
            if (entity_id != INVALID_ENTITY_ID) {
                ecs_manager.destroy_entity(entity_id);
                LM.write_log("Destroyed %s by name (ID: %u)", name.c_str(), entity_id);
            }
        }

        // Clear the map after removing all entities
        pause_menu_entities.clear();
    }


    void GUI_System::check_pause_menu_button_collision(float delta_time) {
        (void)delta_time;  // Unused parameter

        // Return early if we are not actually paused
        if (!GM.is_paused()) return;

        // Get the current mouse position in screen coordinates
        double screen_mouse_x, screen_mouse_y;
        IM.get_mouse_position(screen_mouse_x, screen_mouse_y);

        // Retrieve window dimensions
        int window_width = WC.get_win_width();
        int window_height = WC.get_win_height();

        // Calculate the center of the screen
        float center_x = static_cast<float>(window_width) / 2.0f;
        float center_y = static_cast<float>(window_height) / 2.0f;

        // Go through every entity in this system and look for pause-menu buttons
        for (EntityID entity_id : entities) {
            auto* entity = ecs_manager.get_entity(entity_id);
            if (!entity) continue;

            const std::string& entity_name = entity->get_name();

            // We only care about these three named entities in the pause menu
            if (entity_name != "resume_button" &&
                entity_name != "restart_button" &&
                entity_name != "main_menu_button")
            {
                continue;
            }

            // Ensure required components are present
            if (!ecs_manager.has_component<Transform2D>(entity_id) ||
                !ecs_manager.has_component<Graphics_Component>(entity_id) ||
                !ecs_manager.has_component<Audio_Component>(entity_id))
            {
                LM.write_log("Missing required component on entity: %s", entity_name.c_str());
                continue;
            }

            auto& transform = ecs_manager.get_component<Transform2D>(entity_id);
            auto& graphics = ecs_manager.get_component<Graphics_Component>(entity_id);
            auto& audio = ecs_manager.get_component<Audio_Component>(entity_id);



            float half_width = transform.scale.x * 0.5f;
            float half_height = transform.scale.y * 0.5f;

            // NOTE the subtraction for Y:
            float button_screen_x = center_x + transform.position.x;
            float button_screen_y = center_y - transform.position.y - 95.0f;

            float screen_left = button_screen_x - half_width;
            float screen_right = button_screen_x + half_width;
            float screen_top = button_screen_y - half_height;
            float screen_bottom = button_screen_y + half_height;

            LM.write_log("Mouse Y: %f, Button top: %f, bottom: %f",
                screen_mouse_y, screen_top, screen_bottom);

            // Check if mouse is over the button in screen coordinates
            bool is_hovered =
                (screen_mouse_x >= screen_left && screen_mouse_x <= screen_right) &&
                (screen_mouse_y >= screen_top && screen_mouse_y <= screen_bottom);

            // Define texture and sounds
            std::string base_texture;
            std::string hover_sound = "button_hover";
            std::string click_sound = "main_menu";

            if (entity_name == "resume_button") {
                base_texture = "Resume_Batch_14";
            }
            else if (entity_name == "restart_button") {
                base_texture = "Restart_Batch_14";
            }
            else if (entity_name == "main_menu_button") {
                base_texture = "Main_Menu_Batch_14";
            }

            // Ensure we have a recorded hover state for this button
            if (pause_button_hover_states.find(entity_name) == pause_button_hover_states.end()) {
                pause_button_hover_states[entity_name] = false;
            }

            if (is_hovered) {
                // Play hover sound if we just entered hover
                if (!pause_button_hover_states[entity_name]) {
                    ADM.play_now(entity_id, hover_sound, audio);
                    pause_button_hover_states[entity_name] = true;
                }

                // Check if the mouse is clicked (pressed or held)
                if (IM.is_mouse_button_held(GLFW_MOUSE_BUTTON_LEFT)) {
                    // Show pressed texture
                    graphics.texture_name = base_texture + "_PRESSED";
                    ADM.play_now(entity_id, click_sound, audio);

                    // Handle the actual button click
                    if (entity_name == "resume_button") {
                        GM.set_paused(false);
                        hide_pause_menu();
                    }
                    else if (entity_name == "restart_button") {
                        LM.write_log("Restart button pressed - reloading to scene 2");

                        // Unpause first
                        GM.set_paused(false);

                        // Force hide pause menu first
                        hide_pause_menu();

                        // Reset all GUI states first - similar to your TNT code
                        for (auto& systems_gui : ecs_manager.get_systems()) {
                            if (auto* gui_system = dynamic_cast<GUI_System*>(systems_gui.get())) {
                                gui_system->reset_all_game_state();
                                LM.write_log("GUI state reset after restart button press");
                                break;
                            }
                        }

                        // Set up scene reload - same as your TNT code
                        std::string scene_file = "scene2.scn";
                        GM.set_current_scene(2);

                        // Create full path to the scene file
                        std::string scene_path = ASM.get_full_path("Scenes", scene_file);

                        // Try to load the new scene - follows your TNT code pattern
                        if (SM.load_scene(scene_path.c_str())) {
                            LM.write_log("Successfully loaded %s", scene_file.c_str());

                            // Reset camera position
                            auto& camera = GFXM.get_camera();
                            camera.pos_x = DEFAULT_CAMERA_POS_X;
                            camera.pos_y = DEFAULT_CAMERA_POS_Y;

                            // Stop all audio
                            ADM.stop_mastergroup();

                            // Reset player position
                            EntityID playerId = ecs_manager.find_entity_by_name(DEFAULT_PLAYER_NAME);
                            if (playerId != INVALID_ENTITY_ID) {
                                if (ecs_manager.has_component<Transform2D>(playerId)) {
                                    auto& transform = ecs_manager.get_component<Transform2D>(playerId);
                                    transform.position = Vec2D(0.0f, 0.0f);
                                    transform.prev_position = transform.position;
                                }
                                if (ecs_manager.has_component<Velocity_Component>(playerId)) {
                                    auto& velocity = ecs_manager.get_component<Velocity_Component>(playerId);
                                    velocity.velocity = Vec2D(0.0f, 0.0f);
                                }
                            }

                            // Update IMGUI
                            IMGUIM.set_current_file_shown(scene_file);
                        }
                        else {
                            LM.write_log("Failed to load %s", scene_file.c_str());
                            // Revert to main menu if load failed
                            GM.set_current_scene(0);
                        }
                    }
                    else if (entity_name == "main_menu_button") {
                        LM.write_log("Main Menu button pressed - returning to main menu");

                        // Unpause first
                        GM.set_paused(false);

                        // Force hide pause menu first
                        hide_pause_menu();

                        // Reset all GUI states
                        for (auto& systems_gui : ecs_manager.get_systems()) {
                            if (auto* gui_system = dynamic_cast<GUI_System*>(systems_gui.get())) {
                                gui_system->reset_all_game_state();
                                LM.write_log("GUI state reset after main menu button press");
                                break;
                            }
                        }

                        // Set up main menu load
                        std::string scene_file = "main_menu.scn";
                        GM.set_current_scene(0);

                        // Create full path to the scene file
                        std::string scene_path = ASM.get_full_path("Scenes", scene_file);

                        // Try to load the main menu
                        if (SM.load_scene(scene_path.c_str())) {
                            LM.write_log("Successfully loaded main menu");

                            // Reset camera position
                            auto& camera = GFXM.get_camera();
                            camera.pos_x = DEFAULT_CAMERA_POS_X;
                            camera.pos_y = DEFAULT_CAMERA_POS_Y;

                            // Stop all audio
                            ADM.stop_mastergroup();

                            // Update IMGUI
                            IMGUIM.set_current_file_shown(scene_file);
                        }
                        else {
                            LM.write_log("Failed to load main menu");
                        }
                    }
                    // Return now so we do not process more than one button
                    return;
                }
                else {
                    // Mouse is hovering but not clicked
                    graphics.texture_name = base_texture + "_HIGHLIGHTED";
                }
            }
            else {
                // Mouse is not over the button, set back to normal
                graphics.texture_name = base_texture + "_NORMAL";
                pause_button_hover_states[entity_name] = false;
            }
        }
    }


    void GUI_System::show_game_over_menu() {
        // Don't show if already shown
        if (!game_over_entities.empty() || game_over_shown) {
            return;
        }

        LM.write_log("GUI_System::show_game_over_menu(): Creating game over UI");

        // Set the flag
        game_over_shown = true;

        // Create a semi-transparent background overlay
        EntityID overlay = ecs_manager.clone_entity_from_prefab("gui_container", "game_over_overlay");
        if (overlay != INVALID_ENTITY_ID) {
            if (auto* graphics = get_component_safe<Graphics_Component>(overlay)) {
                graphics->model_name = "square";
                graphics->texture_name = "Game_over_Screen_Batch_14";
                graphics->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            }
            if (auto* transform = get_component_safe<Transform2D>(overlay)) {
                transform->position = Vec2D(0.0f, -50.0f);
                transform->scale = Vec2D(1980.0f, 1180.0f);
            }
            game_over_entities["overlay"] = overlay;

            // Explicitly add to system
            add_entity(overlay);
            LM.write_log("Added overlay to GUI system: %u", overlay);
        }

        // Create Restart button
        EntityID restart_button = ecs_manager.clone_entity_from_prefab("gui_container", "restart_button");
        if (restart_button != INVALID_ENTITY_ID) {
            if (auto* graphics = get_component_safe<Graphics_Component>(restart_button)) {
                graphics->model_name = "square";
                graphics->texture_name = "Restart_Batch_14_NORMAL";
                graphics->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            }
            if (auto* transform = get_component_safe<Transform2D>(restart_button)) {
                transform->position = Vec2D(0.0f, -100.0f);
                transform->scale = Vec2D(200.0f, 80.0f);
            }

            // Add GUI Component if needed
            if (!ecs_manager.has_component<GUI_Component>(restart_button)) {
                GUI_Component gui_comp;
                gui_comp.is_container = true;
                ecs_manager.add_component(restart_button, gui_comp);
            }

            // Add Audio Component
            if (!ecs_manager.has_component<Audio_Component>(restart_button)) {
                Audio_Component audio_comp;
                ecs_manager.add_component(restart_button, audio_comp);
            }

            game_over_entities["restart"] = restart_button;

            // Explicitly add to system
            add_entity(restart_button);
            LM.write_log("Added restart button to GUI system: %u", restart_button);
        }

        // Create Main Menu button
        EntityID main_menu_button = ecs_manager.clone_entity_from_prefab("gui_container", "main_menu_button");
        if (main_menu_button != INVALID_ENTITY_ID) {
            if (auto* graphics = get_component_safe<Graphics_Component>(main_menu_button)) {
                graphics->model_name = "square";
                graphics->texture_name = "Main_Menu_Batch_14_NORMAL";
                graphics->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            }
            if (auto* transform = get_component_safe<Transform2D>(main_menu_button)) {
                transform->position = Vec2D(0.0f, -200.0f);
                transform->scale = Vec2D(200.0f, 80.0f);
            }

            // Add GUI Component if needed
            if (!ecs_manager.has_component<GUI_Component>(main_menu_button)) {
                GUI_Component gui_comp;
                gui_comp.is_container = true;
                ecs_manager.add_component(main_menu_button, gui_comp);
            }

            // Add Audio Component
            if (!ecs_manager.has_component<Audio_Component>(main_menu_button)) {
                Audio_Component audio_comp;
                ecs_manager.add_component(main_menu_button, audio_comp);
            }

            game_over_entities["main_menu"] = main_menu_button;

            // Explicitly add to system
            add_entity(main_menu_button);
            LM.write_log("Added main menu button to GUI system: %u", main_menu_button);
        }

        // Log all entities in the system for debugging
        LM.write_log("Current entities in GUI system after creating game over menu:");
        for (EntityID entity_id : entities) {
            auto* entity = ecs_manager.get_entity(entity_id);
            if (entity) {
                LM.write_log("  Entity ID: %u, Name: %s", entity_id, entity->get_name().c_str());
            }
        }
    }

    void GUI_System::hide_game_over_menu() {
        LM.write_log("GUI_System::hide_game_over_menu(): Removing game over UI");

        // Reset the flag
        game_over_shown = false;

        // First remove the text
        if (game_over_entities.find("text") != game_over_entities.end()) {
            EntityID text_id = game_over_entities["text"];
            if (text_id != INVALID_ENTITY_ID) {
                try {
                    ecs_manager.destroy_entity(text_id);
                    LM.write_log("Destroyed game over text (ID: %u)", text_id);
                }
                catch (const std::exception& e) {
                    LM.write_log("Error destroying game over text (ID: %u): %s", text_id, e.what());
                }
            }
        }

        // Then remove the overlay/background texture
        if (game_over_entities.find("overlay") != game_over_entities.end()) {
            EntityID overlay_id = game_over_entities["overlay"];
            if (overlay_id != INVALID_ENTITY_ID) {
                try {
                    ecs_manager.destroy_entity(overlay_id);
                    LM.write_log("Destroyed game over overlay (ID: %u)", overlay_id);
                }
                catch (const std::exception& e) {
                    LM.write_log("Error destroying overlay (ID: %u): %s", overlay_id, e.what());
                }
            }
        }

        // Then remove buttons in specific order
        const std::vector<std::string> button_order = {
            "restart",
            "main_menu"
        };

        for (const auto& button_key : button_order) {
            if (game_over_entities.find(button_key) != game_over_entities.end()) {
                EntityID button_id = game_over_entities[button_key];
                if (button_id != INVALID_ENTITY_ID) {
                    ecs_manager.destroy_entity(button_id);
                    LM.write_log("Destroyed %s button (ID: %u)", button_key.c_str(), button_id);
                }
            }
        }

        // Also try to find buttons by their entity names
        const std::vector<std::string> button_names = {
            "game_over_restart_button",
            "game_over_main_menu_button"
        };

        for (const auto& name : button_names) {
            EntityID entity_id = ecs_manager.find_entity_by_name(name);
            if (entity_id != INVALID_ENTITY_ID) {
                ecs_manager.destroy_entity(entity_id);
                LM.write_log("Destroyed %s by name (ID: %u)", name.c_str(), entity_id);
            }
        }

        // Clear the map after removing all entities
        game_over_entities.clear();

        // Clear hover states
        game_over_button_hover_states.clear();
    }

    void GUI_System::check_game_over_button_collision(float delta_time) {
        (void)delta_time;  // Unused parameter

        // Return early if game over screen is not displayed
        if (!game_over_shown) return;

        // Get the current mouse position in screen coordinates
        double screen_mouse_x, screen_mouse_y;
        IM.get_mouse_position(screen_mouse_x, screen_mouse_y);

        // Retrieve window dimensions
        int window_width = WC.get_win_width();
        int window_height = WC.get_win_height();

        // Calculate the center of the screen
        float center_x = static_cast<float>(window_width) / 2.0f;
        float center_y = static_cast<float>(window_height) / 2.0f;

        // The keys for the buttons in our game_over_entities map
        const std::vector<std::string> button_keys = { "restart", "main_menu" };

        for (const auto& key : button_keys) {
            // Skip if button doesn't exist in our map
            if (game_over_entities.find(key) == game_over_entities.end()) {
                continue;
            }

            EntityID button_id = game_over_entities[key];
            if (button_id == INVALID_ENTITY_ID) {
                continue;
            }

            auto* entity = ecs_manager.get_entity(button_id);
            if (!entity) {
                continue;
            }

            // Ensure we have the required components
            if (!ecs_manager.has_component<Transform2D>(button_id) ||
                !ecs_manager.has_component<Graphics_Component>(button_id) ||
                !ecs_manager.has_component<Audio_Component>(button_id))
            {
                continue;
            }

            // Grab the needed components
            auto& transform = ecs_manager.get_component<Transform2D>(button_id);
            auto& graphics = ecs_manager.get_component<Graphics_Component>(button_id);
            auto& audio = ecs_manager.get_component<Audio_Component>(button_id);

            // Compute the button's screen position (similar logic to pause menu)
            float button_screen_x = center_x + transform.position.x;
            float button_screen_y = center_y - transform.position.y - 95.0f; // Matches your pause logic

            // Calculate bounding box in screen space
            float half_width = transform.scale.x * 0.5f;
            float half_height = transform.scale.y * 0.5f;

            float screen_left = button_screen_x - half_width;
            float screen_right = button_screen_x + half_width;
            float screen_top = button_screen_y - half_height;
            float screen_bottom = button_screen_y + half_height;

            // Debug logs (optional)
            LM.write_log("[Game Over] Button '%s' collision check:", key.c_str());
            LM.write_log("- Transform pos: (%.2f, %.2f)", transform.position.x, transform.position.y);
            LM.write_log("- Button screen pos: (%.2f, %.2f)", button_screen_x, button_screen_y);
            LM.write_log("- Screen boundaries: L=%.2f, R=%.2f, T=%.2f, B=%.2f",
                screen_left, screen_right, screen_top, screen_bottom);
            LM.write_log("- Mouse pos: (%.2f, %.2f)", screen_mouse_x, screen_mouse_y);

            // Check hover
            bool is_hovered =
                (screen_mouse_x >= screen_left && screen_mouse_x <= screen_right) &&
                (screen_mouse_y >= screen_top && screen_mouse_y <= screen_bottom);

            LM.write_log("- Mouse over button: %s", is_hovered ? "YES" : "NO");

            // Base texture name
            std::string base_texture;
            if (key == "restart") {
                base_texture = "Restart_Batch_14";
            }
            else if (key == "main_menu") {
                base_texture = "Main_Menu_Batch_14";
            }

            // Ensure we have a recorded hover state
            if (game_over_button_hover_states.find(entity->get_name()) == game_over_button_hover_states.end()) {
                game_over_button_hover_states[entity->get_name()] = false;
            }

            if (is_hovered) {
                // Change texture to highlighted if not pressed
                if (graphics.texture_name != base_texture + "_HIGHLIGHTED" &&
                    graphics.texture_name != base_texture + "_PRESSED")
                {
                    graphics.texture_name = base_texture + "_HIGHLIGHTED";
                    LM.write_log("Button %s changed to HIGHLIGHTED", key.c_str());
                }

                // Play hover sound if newly hovering
                if (!game_over_button_hover_states[entity->get_name()]) {
                    ADM.play_now(button_id, "button_hover", audio);
                    game_over_button_hover_states[entity->get_name()] = true;
                }

                // Check for click
                if (IM.is_mouse_button_pressed(GLFW_MOUSE_BUTTON_LEFT)) {
                    LM.write_log("CLICK DETECTED on button: %s", key.c_str());
                    graphics.texture_name = base_texture + "_PRESSED";
                    ADM.play_now(button_id, "main_menu", audio);

                    // Handle specific button
                    if (key == "restart") {
                        LM.write_log("Game over - Restart button pressed - reloading scene 2");

                        hide_game_over_menu();
                        reset_all_game_state();

                        std::string scene_file = "scene2.scn";
                        GM.set_current_scene(2);

                        std::string scene_path = ASM.get_full_path("Scenes", scene_file);
                        if (SM.load_scene(scene_path.c_str())) {
                            LM.write_log("Loaded %s successfully", scene_file.c_str());

                            auto& camera = GFXM.get_camera();
                            camera.pos_x = DEFAULT_CAMERA_POS_X;
                            camera.pos_y = DEFAULT_CAMERA_POS_Y;

                            ADM.stop_mastergroup();

                            // Reset player
                            EntityID playerId = ecs_manager.find_entity_by_name(DEFAULT_PLAYER_NAME);
                            if (playerId != INVALID_ENTITY_ID) {
                                if (ecs_manager.has_component<Transform2D>(playerId)) {
                                    auto& player_transform = ecs_manager.get_component<Transform2D>(playerId);
                                    player_transform.position = Vec2D(0.0f, 0.0f);
                                    player_transform.prev_position = player_transform.position;
                                }
                                if (ecs_manager.has_component<Velocity_Component>(playerId)) {
                                    auto& velocity = ecs_manager.get_component<Velocity_Component>(playerId);
                                    velocity.velocity = Vec2D(0.0f, 0.0f);
                                }
                            }
                            IMGUIM.set_current_file_shown(scene_file);
                        }
                    }
                    else if (key == "main_menu") {
                        LM.write_log("Game over - Main Menu button pressed - returning to main menu");

                        hide_game_over_menu();
                        reset_all_game_state();

                        std::string scene_file = "main_menu.scn";
                        GM.set_current_scene(0);

                        std::string scene_path = ASM.get_full_path("Scenes", scene_file);
                        if (SM.load_scene(scene_path.c_str())) {
                            LM.write_log("Loaded main menu successfully");

                            auto& camera = GFXM.get_camera();
                            camera.pos_x = DEFAULT_CAMERA_POS_X;
                            camera.pos_y = DEFAULT_CAMERA_POS_Y;

                            ADM.stop_mastergroup();
                            IMGUIM.set_current_file_shown(scene_file);
                        }
                    }

                    // Return so we do not process any other buttons
                    return;
                }
            }
            else {
                // Not hovered: set to normal
                if (graphics.texture_name != base_texture + "_NORMAL") {
                    graphics.texture_name = base_texture + "_NORMAL";
                    LM.write_log("Button %s changed to NORMAL", key.c_str());
                }

                // Reset hover state
                game_over_button_hover_states[entity->get_name()] = false;
            }
        }
    }


}
