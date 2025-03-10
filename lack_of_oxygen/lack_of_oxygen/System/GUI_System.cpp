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


    void GUI_System::update(float delta_time)
    {
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

        // Update pause menu button interactions if the game is paused
        if (GM.is_paused()) {
            check_pause_menu_button_collision(delta_time);
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
        if (stored_mineral_progress * 50000 >= 50000) {
            // win condition log current stored mineral ammount
            LM.write_log("Win condition met: %f minerals collected", stored_mineral_progress * 50000.0f);

            reset_all_game_state();

            LM.write_log("Reset met: %f minerals collected", stored_mineral_progress * 50000.0f);

            // Load win screen
            const std::string SCENES = "Scenes";
            std::string scene_file = "win_screen.scn";
            std::string scene_path = ASM.get_full_path(SCENES, scene_file);

            if (SM.load_scene(scene_path.c_str())) {
                // Reset camera position
                auto& camera = GFXM.get_camera();
                camera.pos_x = DEFAULT_CAMERA_POS_X;
                camera.pos_y = DEFAULT_CAMERA_POS_Y;

                // Stop all audio
                ADM.stop_mastergroup();

                // Update scene
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
                transform->position = Vec2D(0.0f, 100.0f);
                transform->scale = Vec2D(200.0f, 80.0f);
            }
            if (auto* collision = get_component_safe<Collision_Component>(resume_button)) {
                collision->width = 200.0f;
                collision->height = 80.0f;
                collision->collidable = true;
            }
            if (auto* physics = get_component_safe<Physics_Component>(resume_button)) {
                physics->set_is_static(true);
            }
            if (auto* velocity = get_component_safe<Velocity_Component>(resume_button)) {
                velocity->velocity = Vec2D(0.0f, 0.0f);
            }

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
                transform->position = Vec2D(0.0f, 0.0f);
                transform->scale = Vec2D(200.0f, 80.0f);
            }
            if (auto* collision = get_component_safe<Collision_Component>(restart_button)) {
                collision->width = 200.0f;
                collision->height = 80.0f;
                collision->collidable = true;
            }
            if (auto* physics = get_component_safe<Physics_Component>(restart_button)) {
                physics->set_is_static(true);
            }
            if (auto* velocity = get_component_safe<Velocity_Component>(restart_button)) {
                velocity->velocity = Vec2D(0.0f, 0.0f);
            }

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
                transform->position = Vec2D(0.0f, -100.0f);
                transform->scale = Vec2D(200.0f, 80.0f);
            }
            if (auto* collision = get_component_safe<Collision_Component>(main_menu_button)) {
                collision->width = 200.0f;
                collision->height = 80.0f;
                collision->collidable = true;
            }
            if (auto* physics = get_component_safe<Physics_Component>(main_menu_button)) {
                physics->set_is_static(true);
            }
            if (auto* velocity = get_component_safe<Velocity_Component>(main_menu_button)) {
                velocity->velocity = Vec2D(0.0f, 0.0f);
            }

            pause_menu_entities["main_menu"] = main_menu_button;
        }
    }

    void GUI_System::hide_pause_menu() {
        LM.write_log("GUI_System::hide_pause_menu(): Removing pause menu UI in specific order");

        // First remove the overlay/background texture
        if (pause_menu_entities.find("overlay") != pause_menu_entities.end()) {
            EntityID overlay_id = pause_menu_entities["overlay"];
            if (overlay_id != INVALID_ENTITY_ID) {
                ecs_manager.destroy_entity(overlay_id);
                LM.write_log("Destroyed pause menu overlay (ID: %u)", overlay_id);
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
        (void)delta_time;  // Mark as intentionally unused

        // Return early if we're not paused
        if (!GM.is_paused()) return;

        // Get mouse position in world coordinates
        Vec2D world_mouse_pos = ESS.Get_World_MousePos();

        // Debug logging to verify coordinates
        LM.write_log("Pause menu check - Mouse position: (%.2f, %.2f)", world_mouse_pos.x, world_mouse_pos.y);

        // Iterate through ALL entities in this system instead of using map lookup
        for (EntityID entity_id : entities) {
            auto* entity = ecs_manager.get_entity(entity_id);
            if (!entity) continue;

            std::string entity_name = entity->get_name();

            // Debug logging
            // LM.write_log("Checking entity: %s (ID: %u)", entity_name.c_str(), entity_id);

            // Only check for pause menu buttons with consistent naming
            if (entity_name != "resume_button" &&
                entity_name != "restart_button" &&
                entity_name != "main_menu_button") continue;

            if (!ecs_manager.has_component<Transform2D>(entity_id) ||
                !ecs_manager.has_component<Graphics_Component>(entity_id) ||
                !ecs_manager.has_component<Audio_Component>(entity_id)) {
                // Debug logging
                LM.write_log("Missing required component on entity: %s", entity_name.c_str());
                continue;
            }

            auto& transform = ecs_manager.get_component<Transform2D>(entity_id);
            auto& graphics = ecs_manager.get_component<Graphics_Component>(entity_id);
            auto& audio = ecs_manager.get_component<Audio_Component>(entity_id);

            // Debug button position and size
            // LM.write_log("Button %s position: (%.2f, %.2f), size: (%.2f, %.2f)",
            //     entity_name.c_str(), transform.position.x, transform.position.y,
            //     transform.scale.x, transform.scale.y);

            // Check if mouse is hovering over the button
            bool is_hovered = ESS.Mouse_Over_AABB(
                transform.position.x,
                transform.position.y,
                transform.scale.x,
                transform.scale.y,
                world_mouse_pos.x,
                world_mouse_pos.y
            );

            // Define the base texture name for each button
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

            // Check map initialization
            if (pause_button_hover_states.find(entity_name) == pause_button_hover_states.end()) {
                pause_button_hover_states[entity_name] = false;
            }

            if (is_hovered) {
                // Debug logging for hover state
                LM.write_log("Mouse is hovering over %s", entity_name.c_str());

                // Play hover sound when first hovering
                if (!pause_button_hover_states[entity_name]) {
                    ADM.play_now(entity_id, hover_sound, audio);
                    pause_button_hover_states[entity_name] = true;
                }

                if (IM.is_mouse_button_pressed(GLFW_MOUSE_BUTTON_LEFT)) {
                    graphics.texture_name = base_texture + "_PRESSED";
                    ADM.play_now(entity_id, click_sound, audio);
                    LM.write_log("Button clicked: %s", entity_name.c_str());

                    // Handle button click actions
                    if (entity_name == "resume_button") {
                        LM.write_log("Resume button pressed - unpausing game");
                        GM.set_paused(false);
                    }
                    else if (entity_name == "restart_button") {
                        LM.write_log("Restart button pressed - reloading current scene");

                        // Unpause first
                        GM.set_paused(false);

                        // Clear dynamic entities
                        for (auto& system : ecs_manager.get_systems()) {
                            if (auto* movement_system = dynamic_cast<Movement_System*>(system.get())) {
                                movement_system->clear_dynamic_entities();
                                break;
                            }
                        }

                        // Reload current scene
                        const std::string SCENES = "Scenes";
                        std::string scene_file = "scene" + std::to_string(GM.get_current_scene()) + ".scn";
                        std::string scene_path = ASM.get_full_path(SCENES, scene_file);

                        if (SM.load_scene(scene_path.c_str())) {
                            LM.write_log("Successfully reloaded scene: %s", scene_file.c_str());

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

                            // Reset game state values
                            reset_all_game_state();

                            // Update IMGUI
                            IMGUIM.set_current_file_shown(scene_file);
                        }
                        else {
                            LM.write_log("Failed to reload scene: %s", scene_path.c_str());
                        }
                    }
                    else if (entity_name == "main_menu_button") {
                        LM.write_log("Main Menu button pressed - returning to main menu");

                        // Unpause first
                        GM.set_paused(false);

                        // Clear dynamic entities
                        for (auto& system : ecs_manager.get_systems()) {
                            if (auto* movement_system = dynamic_cast<Movement_System*>(system.get())) {
                                movement_system->clear_dynamic_entities();
                                break;
                            }
                        }

                        // Load main menu
                        const std::string SCENES = "Scenes";
                        std::string scene_file = "main_menu.scn";
                        std::string scene_path = ASM.get_full_path(SCENES, scene_file);

                        if (SM.load_scene(scene_path.c_str())) {
                            LM.write_log("Successfully loaded main menu");

                            // Reset camera position
                            auto& camera = GFXM.get_camera();
                            camera.pos_x = DEFAULT_CAMERA_POS_X;
                            camera.pos_y = DEFAULT_CAMERA_POS_Y;

                            // Stop all audio
                            ADM.stop_mastergroup();

                            // Reset game state
                            reset_all_game_state();

                            // Update current scene in Game Manager
                            GM.set_current_scene(0);
                            IMGUIM.set_current_file_shown(scene_file);
                        }
                        else {
                            LM.write_log("Failed to load main menu: %s", scene_path.c_str());
                        }
                    }

                    return; // Button was clicked, no need to check others
                }
                else {
                    // Set highlighted state when just hovering
                    graphics.texture_name = base_texture + "_HIGHLIGHTED";
                }
            }
            else {
                // Reset to normal state texture when not hovering
                graphics.texture_name = base_texture + "_NORMAL";
                pause_button_hover_states[entity_name] = false;
            }
        }
    }
} // namespace lof
