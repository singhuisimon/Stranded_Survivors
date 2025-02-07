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
        //LM.write_log("GUI_System::reset_all_game_state(): Hiding GUI elements...");
        hide_mineral_tank_gui();
        hide_oxygen_tank_gui();
        hide_oxygen_warning(50.0f);
        hide_oxygen_warning(20.0f);
        hide_oxygen_warning(5.0f);

        // Log initial state of entities before reset
        //LM.write_log("GUI_System::reset_all_game_state(): Current entity states - "
            //"mineral_e_prompt: %d, oxygen_e_prompt: %d",
           // mineral_e_prompt, oxygen_e_prompt);

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
       // LM.write_log("GUI_System::reset_all_game_state(): Resetting gameplay values...");
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
       // LM.write_log("GUI_System::reset_all_game_state(): Resetting warning states...");
        warning_50_active = false;
        warning_20_active = false;
        warning_5_active = false;
        warning_50_shown = false;
        warning_20_shown = false;
        warning_5_shown = false;
        warning_50_display_time = 0.0f;
        warning_20_display_time = 0.0f;
        warning_5_display_time = 0.0f;

       // LM.write_log("GUI_System::reset_all_game_state(): Reset complete");
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
         //   LM.write_log("Win condition met: %f minerals collected", stored_mineral_progress * 50000.0f);

            reset_all_game_state();

         //   LM.write_log("Reset met: %f minerals collected", stored_mineral_progress * 50000.0f);

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
                //IMGUIM.set_current_file_shown(scene_file);
                return;
            }
            else {
             //   LM.write_log("Failed to load scene file: %s", scene_path.c_str());
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
          //  LM.write_log("%s: Invalid entity ID", prefix);
            return;
        }

        auto* entity = ecs_manager.get_entity(id);
        if (!entity) {
          //  LM.write_log("%s: Entity %u not found in ECS", prefix, id);
            return;
        }

       // LM.write_log("%s: Entity %u exists, name: %s", prefix, id, entity->get_name().c_str());

        if (ecs_manager.has_component<Transform2D>(id)) {
            auto& transform = ecs_manager.get_component<Transform2D>(id);
          //  LM.write_log("  - Transform2D: pos(%.2f, %.2f)", transform.position.x, transform.position.y);
        }

        if (ecs_manager.has_component<Graphics_Component>(id)) {
            auto& graphics = ecs_manager.get_component<Graphics_Component>(id);
           // LM.write_log("  - Graphics: texture='%s'", graphics.texture_name.c_str());
        }

        if (ecs_manager.has_component<GUI_Component>(id)) {
            auto& gui = ecs_manager.get_component<GUI_Component>(id);
           // LM.write_log("  - GUI: progress=%.2f, isContainer=%d, isProgressBar=%d",
           //     gui.progress, gui.is_container, gui.is_progress_bar);
        }

        // Check if entity is in this system
        if (entities.find(id) != entities.end()) {
           // LM.write_log("  - Present in GUI_System");
        }
        else {
           // LM.write_log("  - NOT present in GUI_System");
        }
    }

    void GUI_System::validate_gui_state() {
       // LM.write_log("=== GUI State Validation ===");
        debug_entity("Container", container_id);
        debug_entity("Background Bar", background_bar_id);
        debug_entity("Progress Bar", progress_bar_id);

        // List all entities in this system
       // LM.write_log("Entities in GUI_System:");
        for (EntityID id : entities) {
            auto* entity = ecs_manager.get_entity(id);
            if (entity) {
               // LM.write_log("  - Entity %u (%s)", id, entity->get_name().c_str());
            }
        }
       // LM.write_log("=== End GUI State Validation ===");
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
                graphics->color = glm::vec4(1.0f);
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
                graphics->texture_name = "UI_MineralsFill_1920x1080_v2";
                graphics->color = glm::vec4(1.0f);
            }
            if (auto* transform = get_component_safe<Transform2D>(mineral_interaction_container)) {
                transform->position = Vec2D(0.0f, 0.0f);
                transform->scale = Vec2D(1980.0f, 1020.0f);
            }
        }

        // 3) Single progress bar
        mineral_progress_bar = ecs_manager.clone_entity_from_prefab("gui_progress_bar");
        if (mineral_progress_bar != INVALID_ENTITY_ID) {
            if (auto* graphics = get_component_safe<Graphics_Component>(mineral_progress_bar)) {
                graphics->model_name = "square";
                graphics->texture_name = "NoTexture";
                // White or Gold color for minerals
                graphics->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            }
            if (auto* transform = get_component_safe<Transform2D>(mineral_progress_bar)) {
                float max_width = 600.0f;
                float current_width = max_width * stored_mineral_progress;
                transform->position = Vec2D(-642.0f + (current_width / 2.0f), 60.0f);
                transform->scale = Vec2D(current_width, 42.0f);
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
                transform->position = Vec2D(-350.0f, 10.0f);
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
                transform->position = Vec2D(-350.0f, 57.0f);
                transform->scale = Vec2D(0.5f, 0.5f);
            }
        }
    }


    void GUI_System::hide_mineral_tank_gui() {
        // (5) Destroy deposit count text
        if (mineral_deposit_count_text != INVALID_ENTITY_ID) {
            //LM.write_log("Destroying mineral deposit count text entity: %d", mineral_deposit_count_text);
            ecs_manager.destroy_entity(mineral_deposit_count_text);
            mineral_deposit_count_text = INVALID_ENTITY_ID;
        }

        // (4) Destroy percentage text
        if (mineral_percentage_text != INVALID_ENTITY_ID) {
            //LM.write_log("Destroying mineral percentage text entity: %d", mineral_percentage_text);
            ecs_manager.destroy_entity(mineral_percentage_text);
            mineral_percentage_text = INVALID_ENTITY_ID;
        }

        // (3) Destroy progress bar
        if (mineral_progress_bar != INVALID_ENTITY_ID) {
            //LM.write_log("Destroying mineral progress bar entity: %d", mineral_progress_bar);
            ecs_manager.destroy_entity(mineral_progress_bar);
            mineral_progress_bar = INVALID_ENTITY_ID;
        }

        // (2) Destroy container
        if (mineral_interaction_container != INVALID_ENTITY_ID) {
            //LM.write_log("Destroying mineral container entity: %d", mineral_interaction_container);
            ecs_manager.destroy_entity(mineral_interaction_container);
            mineral_interaction_container = INVALID_ENTITY_ID;
        }

        // (1) Finally, destroy the E prompt
        if (mineral_e_prompt != INVALID_ENTITY_ID) {
            //LM.write_log("Destroying mineral E prompt entity: %d", mineral_e_prompt);
            ecs_manager.destroy_entity(mineral_e_prompt);
            mineral_e_prompt = INVALID_ENTITY_ID;
        }
    }

    void GUI_System::update_mineral_progress(float progress)
    {
        if (GM.get_current_scene() == 0) { // Main menu
            stored_mineral_progress = 0.0f;
          //  LM.write_log("GUI_System::update_mineral_progress(): Reset progress on main menu");
            return;
        }

        // 1) Clamp and store the new progress
        stored_mineral_progress = std::clamp(progress, 0.0f, 1.0f);

        // 2) Update the progress bar width/position
        if (mineral_progress_bar != INVALID_ENTITY_ID) {
            if (auto* transform = get_component_safe<Transform2D>(mineral_progress_bar)) {
                float max_width = 600.0f;
                float new_width = max_width * stored_mineral_progress;

                transform->scale.x = new_width;
                transform->position.x = -642.0f + (new_width / 2.0f);
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
                //printf("deposit count %d\n", depositCount);
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
                graphics->color = glm::vec4(1.0f);
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
                graphics->texture_name = "UI_OxygenRefill_1920x1080_v2";
                graphics->color = glm::vec4(1.0f);
            }
            if (auto* transform = get_component_safe<Transform2D>(oxygen_interaction_container)) {
                transform->position = Vec2D(0.0f, 0.0f);
                transform->scale = Vec2D(1980.0f, 1020.0f);
            }
        }

        // == Player Oxygen Bar 
        float playerOxygen = GM.get_current_oxygen_level(); // 0..100
        float playerFraction = playerOxygen / 100.0f;       // 0..1
        stored_oxygen_progress1 = playerFraction;

        // Set a consistent bar width & height for both bars
        const float BAR_MAX_WIDTH = 620.0f; // same as ship bar
        const float BAR_HEIGHT = 14.0f;
        constexpr float SHIP_MAX = 400.0f;

        //
        // 3) First progress bar (Player Oxygen)
        //
        oxygen_progress_bar1 = ecs_manager.clone_entity_from_prefab("gui_progress_bar");
        if (oxygen_progress_bar1 != INVALID_ENTITY_ID)
        {
            if (auto* graphics = get_component_safe<Graphics_Component>(oxygen_progress_bar1)) {
                graphics->model_name = "square";
                graphics->texture_name = "NoTexture";
                graphics->color = glm::vec4(0.0f, 0.68f, 1.0f, 1.0f);
            }
            if (auto* transform = get_component_safe<Transform2D>(oxygen_progress_bar1)) {
                float current_width = BAR_MAX_WIDTH * stored_oxygen_progress1;
                float bar_y = 78.5f; // Some Y offset

                transform->position = Vec2D(-657.0f + (current_width / 2.0f), bar_y);
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
                transform->position = Vec2D(-623.0f, 100.0f);
                transform->scale = Vec2D(0.5f, 0.5f);
            }
        }

        // 5) Second progress bar (Ship Oxygen) in YELLOW
        {
            float currentShipOxy = GM.get_ship_oxygen_level();
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
                    graphics->color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
                }
                if (auto* transform = get_component_safe<Transform2D>(oxygen_progress_bar2)) {
                    float bar_y_ship = 53.3f;
                    transform->position = Vec2D(-657.0f + (current_width / 2.0f), bar_y_ship);
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
                    transform->position = Vec2D(-620.0f, 25.0f);
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
            //LM.write_log("Destroying oxygen percentage text 2 entity %d", oxygen_percentage_text2);
            ecs_manager.destroy_entity(oxygen_percentage_text2);
            oxygen_percentage_text2 = INVALID_ENTITY_ID;
        }

        // (5) Second progress bar
        if (oxygen_progress_bar2 != INVALID_ENTITY_ID) {
            //LM.write_log("Destroying oxygen progress bar 2 entity %d", oxygen_progress_bar2);
            ecs_manager.destroy_entity(oxygen_progress_bar2);
            oxygen_progress_bar2 = INVALID_ENTITY_ID;
        }

        // (4) First bar text
        if (oxygen_percentage_text1 != INVALID_ENTITY_ID) {
            //LM.write_log("Destroying oxygen percentage text 1 entity %d", oxygen_percentage_text1);
            ecs_manager.destroy_entity(oxygen_percentage_text1);
            oxygen_percentage_text1 = INVALID_ENTITY_ID;
        }

        // (3) First progress bar
        if (oxygen_progress_bar1 != INVALID_ENTITY_ID) {
            //LM.write_log("Destroying oxygen progress bar 1 entity %d", oxygen_progress_bar1);
            ecs_manager.destroy_entity(oxygen_progress_bar1);
            oxygen_progress_bar1 = INVALID_ENTITY_ID;
        }

        // (2) Main container
        if (oxygen_interaction_container != INVALID_ENTITY_ID) {
            //LM.write_log("Destroying oxygen container entity %d", oxygen_interaction_container);
            ecs_manager.destroy_entity(oxygen_interaction_container);
            oxygen_interaction_container = INVALID_ENTITY_ID;
        }

        // (1) E prompt
        if (oxygen_e_prompt != INVALID_ENTITY_ID) {
            //LM.write_log("Destroying oxygen E prompt entity %d", oxygen_e_prompt);
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
            float BAR_MAX_WIDTH = 620.0f;
            float bar_y = 78.5f;         // e.g. from show_oxygen_tank_gui() for the player bar
            float new_width = BAR_MAX_WIDTH * stored_oxygen_progress1;

            if (auto* transform = get_component_safe<Transform2D>(oxygen_progress_bar1)) {
                transform->scale.x = new_width;
                transform->position.x = -657.0f + (new_width / 2.0f);
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
            float BAR_MAX_WIDTH = 620.0f;
            float bar_y = 53.3f;

            // Reversed fill => 1 - usedFraction
            float reversed_value = 1.0f - stored_oxygen_progress2;
            float new_width = BAR_MAX_WIDTH * reversed_value;

            if (auto* transform = get_component_safe<Transform2D>(oxygen_progress_bar2)) {
                transform->scale.x = new_width;
                transform->position.x = -657.0f + (new_width / 2.0f);
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
        // Determine which warning names to use
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
        glm::vec3 text_color;

        if (percent == 50.0f) {
            if (percent == 50.0f) {
                texture_name = "Purple_Oxy_Warning_Batch_14";  // Fixed typo here
                warning_message = "WARNING: OXYGEN LEVEL 50%";
                text_color = glm::vec3(1.0f, 1.0f, 1.0f);
            }
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

        // Create warning container with unique name
        EntityID container_id = ecs_manager.clone_entity_from_prefab("gui_container");
        if (container_id != INVALID_ENTITY_ID) {
            std::string unique_name = "warning_container_" + std::to_string(static_cast<int>(percent));
            ecs_manager.update_entity_name(container_id, unique_name);
            container_name = unique_name;

            auto* container_gui = get_component_safe<GUI_Component>(container_id);
            if (!container_gui) {
                hide_oxygen_warning(percent);
                return;
            }
            container_gui->is_container = true;

            if (auto* graphics = get_component_safe<Graphics_Component>(container_id)) {
                graphics->model_name = "square";
                graphics->texture_name = texture_name;
                graphics->color = glm::vec4(1.0f);
            }

            if (auto* transform = get_component_safe<Transform2D>(container_id)) {
                transform->position = Vec2D(0.0f, 200.0f);
                transform->scale = Vec2D(2000.0f, 50.0f);
            }
        }

        // Create warning text with unique name
        EntityID text_id = ecs_manager.clone_entity_from_prefab("text_object");
        if (text_id != INVALID_ENTITY_ID) {
            std::string unique_name = "warning_text_" + std::to_string(static_cast<int>(percent));
            ecs_manager.update_entity_name(text_id, unique_name);
            text_name = unique_name;

            if (auto* text = get_component_safe<Text_Component>(text_id)) {
                text->font_name = DEFAULT_FONT_NAME;
                text->text = warning_message;
                text->color = text_color;
                text->scale = glm::vec2(0.7f, 0.7f);
            }
            if (auto* transform = get_component_safe<Transform2D>(text_id)) {
                transform->position = Vec2D(0.0f, 195.0f);
                transform->scale = Vec2D(0.7f, 0.7f);
            }
        }

        if (percent == 50.0f) warning_50_shown = true;
        else if (percent == 20.0f) warning_20_shown = true;
        else if (percent == 5.0f) warning_5_shown = true;
    }

    void GUI_System::hide_oxygen_warning(float percent) {
        // Get the appropriate warning names
        std::string& text_name = (percent == 50.0f) ? warning_text_50_name :
            (percent == 20.0f) ? warning_text_20_name :
            warning_text_5_name;

        std::string& container_name = (percent == 50.0f) ? warning_container_50_name :
            (percent == 20.0f) ? warning_container_20_name :
            warning_container_5_name;

        // Look up current entity IDs by name
        EntityID text_id = ecs_manager.find_entity_by_name(text_name);
        EntityID container_id = ecs_manager.find_entity_by_name(container_name);

        // Destroy text entity if it exists
        if (text_id != INVALID_ENTITY_ID) {
            ecs_manager.destroy_entity(text_id);
        }

        // Destroy container entity if it exists
        if (container_id != INVALID_ENTITY_ID) {
            ecs_manager.destroy_entity(container_id);
        }

        // Clear the stored names
        text_name.clear();
        container_name.clear();

        // Reset the corresponding warning flags
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
} // namespace lof
