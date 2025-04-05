/**
 * @file GUI_System.cpp
 * @brief Define the GUI system
 * @author Simon Chan (95%), Saw Hui Shan (5%)
 * @date January 15, 2025
 * Copyright (C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
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
#include "../Scripts/Mining_Script.h"
#include "../Scripts/Tutorial_Script.h"
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
        hide_wormhole_gui();

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
        wormhole_e_prompt = INVALID_ENTITY_ID;

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

        // Reset 
        // text
        EntityID timer_text_entity = ECSM.find_entity_by_name("top_ui_timer_count_text");
        if (timer_text_entity != INVALID_ENTITY_ID && ECSM.has_component<Text_Component>(timer_text_entity)) {
            auto& text_comp = ECSM.get_component<Text_Component>(timer_text_entity);
            text_comp.text = "300";
            GM.reset_timer();
        }

        EntityID player = ecs_manager.find_entity_by_name(DEFAULT_PLAYER_NAME);
        if (player != INVALID_ENTITY_ID) {

            // Reset velocity
            if (ecs_manager.has_component<Velocity_Component>(player)) {
                auto& velocity = ecs_manager.get_component<Velocity_Component>(player);
                velocity.velocity = Vec2D(0.0f, 0.0f);
            }
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

        gameover_audio_played = false;

        LM.write_log("GUI_System::reset_all_game_state(): Reset complete");
    }

    static bool fade_complete = false;

    void GUI_System::update(float delta_time) {

        if (fade_complete) {
            ADM.resume_group(GroupType::TYPE_BGM);
            ADM.resume_group(GroupType::TYPE_SFX);
            fade_complete = false;
        }
        
        // Process any active fades first
        if (fade_active) {
            fade_complete = update_screen_fade(delta_time);
            // If fade is still active and not complete, we still want to process other updates
            // but with this flag we know a fade is in progress
        }

        // If we don't have a fade overlay but should, recreate it
        if (fade_active && ecs_manager.find_entity_by_name(fade_overlay_name) == INVALID_ENTITY_ID) {
            create_fade_overlay();
        }

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
            wormhole_e_prompt = INVALID_ENTITY_ID;
            return;  // Skip all GUI updates on win screen
        }

        // Check game over first
        if (game_over_shown) {
            check_game_over_button_collision(delta_time);

            // Still process E prompt bobbing animations during game over using name-based approach
            // Mineral E prompt bobbing
            EntityID mineral_prompt_entity = ecs_manager.find_entity_by_name("mineral_e_prompt");
            if (mineral_prompt_entity != INVALID_ENTITY_ID) {
                e_prompt_animation_timer += delta_time;
                if (auto* transform = get_component_safe<Transform2D>(mineral_prompt_entity)) {
                    float offset = std::sin(e_prompt_animation_timer * E_PROMPT_SPEED) * E_PROMPT_AMPLITUDE;
                    transform->position.y = original_e_prompt_y + offset;
                    transform->position.x = mineral_e_prompt_x; // keep X constant
                }
            }

            // Oxygen E prompt bobbing
            if (!oxygen_e_prompt_name.empty()) {
                EntityID oxygen_prompt_entity = ecs_manager.find_entity_by_name(oxygen_e_prompt_name);
                if (oxygen_prompt_entity != INVALID_ENTITY_ID) {
                    oxygen_e_prompt_animation_timer += delta_time;
                    if (auto* transform = get_component_safe<Transform2D>(oxygen_prompt_entity)) {
                        float offset = std::sin(oxygen_e_prompt_animation_timer * E_PROMPT_SPEED) * E_PROMPT_AMPLITUDE;
                        transform->position.y = original_e_prompt_y + offset;
                        transform->position.x = oxygen_e_prompt_x; // keep X constant
                    }
                }
            }

            return; // Skip remaining updates when game over is shown
        }
        else {
            // Update pause menu button interactions if the game is paused
            if (GM.is_paused()) {
                check_pause_menu_button_collision(delta_time);

                // Process minimal animations during pause using name-based approach
                // Mineral E prompt bobbing
                EntityID mineral_prompt_entity = ecs_manager.find_entity_by_name("mineral_e_prompt");
                if (mineral_prompt_entity != INVALID_ENTITY_ID) {
                    e_prompt_animation_timer += delta_time;
                    if (auto* transform = get_component_safe<Transform2D>(mineral_prompt_entity)) {
                        float offset = std::sin(e_prompt_animation_timer * E_PROMPT_SPEED) * E_PROMPT_AMPLITUDE;
                        transform->position.y = original_e_prompt_y + offset;
                        transform->position.x = mineral_e_prompt_x; // keep X constant
                    }
                }

                // Oxygen E prompt bobbing
                if (!oxygen_e_prompt_name.empty()) {
                    EntityID oxygen_prompt_entity = ecs_manager.find_entity_by_name(oxygen_e_prompt_name);
                    if (oxygen_prompt_entity != INVALID_ENTITY_ID) {
                        oxygen_e_prompt_animation_timer += delta_time;
                        if (auto* transform = get_component_safe<Transform2D>(oxygen_prompt_entity)) {
                            float offset = std::sin(oxygen_e_prompt_animation_timer * E_PROMPT_SPEED) * E_PROMPT_AMPLITUDE;
                            transform->position.y = original_e_prompt_y + offset;
                            transform->position.x = oxygen_e_prompt_x; // keep X constant
                        }
                    }
                }

                // Skip remaining updates when paused
                return;
            }
        }

        // Normal gameplay updates (not paused, not game over)

        // == Mineral E prompt bobbing (name-based approach) ==
        EntityID mineral_prompt_entity = ecs_manager.find_entity_by_name("mineral_e_prompt");
        if (mineral_prompt_entity != INVALID_ENTITY_ID) {
            e_prompt_animation_timer += delta_time;
            if (auto* transform = get_component_safe<Transform2D>(mineral_prompt_entity)) {
                float offset = std::sin(e_prompt_animation_timer * E_PROMPT_SPEED) * E_PROMPT_AMPLITUDE;
                transform->position.y = original_e_prompt_y + offset;
                transform->position.x = mineral_e_prompt_x; // keep X constant
            }
        }

        // == Oxygen E prompt bobbing (name-based approach) ==
        if (!oxygen_e_prompt_name.empty()) {
            EntityID oxygen_prompt_entity = ecs_manager.find_entity_by_name(oxygen_e_prompt_name);
            if (oxygen_prompt_entity != INVALID_ENTITY_ID) {
                oxygen_e_prompt_animation_timer += delta_time;
                if (auto* transform = get_component_safe<Transform2D>(oxygen_prompt_entity)) {
                    float offset = std::sin(oxygen_e_prompt_animation_timer * E_PROMPT_SPEED) * E_PROMPT_AMPLITUDE;
                    transform->position.y = original_e_prompt_y + offset;
                    transform->position.x = oxygen_e_prompt_x; // keep X constant
                }
            }
        }

        // Animation for wormhole E prompt (name-based approach)
        if (!wormhole_e_prompt_name.empty()) {
            EntityID wormhole_entity = ecs_manager.find_entity_by_name(wormhole_e_prompt_name);
            if (wormhole_entity != INVALID_ENTITY_ID) {
                wormhole_e_prompt_animation_timer += delta_time;
                if (auto* transform = get_component_safe<Transform2D>(wormhole_entity)) {
                    float offset = std::sin(wormhole_e_prompt_animation_timer * E_PROMPT_SPEED) * E_PROMPT_AMPLITUDE;
                    transform->position.y = wormhole_e_prompt_y + offset;
                    transform->position.x = wormhole_e_prompt_x; // Use updated x-position
                }
            }
            else {
                // Entity doesn't exist anymore
                wormhole_e_prompt = INVALID_ENTITY_ID;
                wormhole_e_prompt_name.clear();
            }
        }
        else {
            // Name is empty, so reset the ID too
            wormhole_e_prompt = INVALID_ENTITY_ID;
        }

        // Then update your win condition code:
        if (stored_mineral_progress * 50000 >= 50000) {
            LM.write_log("Win condition met: %f minerals collected", stored_mineral_progress * 50000.0f);

            // Set flag to prevent UI from reappearing during transition
            win_transition_active = true;

            // Immediately hide all UI elements
            hide_mineral_tank_gui();
            hide_oxygen_tank_gui();
            hide_oxygen_warning(50.0f);
            hide_oxygen_warning(20.0f);
            hide_oxygen_warning(5.0f);
            hide_wormhole_gui();

            // Clear all prompt names to prevent updates during transition
            oxygen_e_prompt_name.clear();
            wormhole_e_prompt_name.clear();

            // Clear all entity variables to invalidate them
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
            wormhole_e_prompt = INVALID_ENTITY_ID;

            // Reset all game state
            reset_all_game_state();

            // Set special flag for win screen transition
            transitioning_to_win_screen = true;

            // Ensure fade duration is set to full 3 seconds for win screen
            fade_duration = 3.0f;

            // Start fade transition to win screen
            start_screen_fade(true, "win_screen.scn", 4);

            // Log the transition
            LM.write_log("Started fade transition to win screen");

            // IMPORTANT: Return here to let the fade transition handle the scene loading
            return;
        }


        // --------------------------------------------------------
        // Update oxygen bars automatically every second
        // --------------------------------------------------------
        // Only update if the game is not paused and player is not dead
        // Add check for GM.get_player_dead_state() here
        if (!GM.is_paused() && !GM.get_player_dead_state() && oxygen_interaction_container != INVALID_ENTITY_ID) {
        //if (!GM.is_paused() && oxygen_interaction_container != INVALID_ENTITY_ID) {
            
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

        // Only check oxygen warnings if player is not dead
        if (!GM.get_player_dead_state()) {
            // Check oxygen level and update warnings
            float current_oxygen = GM.get_current_oxygen_level();
            
            // Determine if oxygen is decreasing or increasing
            oxygen_decreasing = (current_oxygen < previous_oxygen_level);

            // Only show warnings when oxygen is decreasing
            if (oxygen_decreasing) {
                // Handle 50% warning - only when decreasing
                if (current_oxygen <= 50.0f && previous_oxygen_level > 50.0f && !warning_50_active) {
                    show_oxygen_warning(50.0f);
                    warning_50_display_time = 0.0f;
                    warning_50_active = true;
                    warning_50_shown = true;
                }

                // Handle 20% warning - only when decreasing
                if (current_oxygen <= 20.0f && previous_oxygen_level > 20.0f && !warning_20_active) {
                    show_oxygen_warning(20.0f);
                    warning_20_display_time = 0.0f;
                    warning_20_active = true;
                    warning_20_shown = true;
                }

                // Handle 5% warning - only when decreasing
                if (current_oxygen <= 5.0f && previous_oxygen_level > 5.0f && !warning_5_active) {
                    show_oxygen_warning(5.0f);
                    warning_5_display_time = 0.0f;
                    warning_5_active = true;
                    warning_5_shown = true;
                }
            }

            // Handle warning timers regardless of oxygen trend
            if (warning_50_shown) {
                warning_50_display_time += delta_time;
                if (warning_50_display_time >= WARNING_DURATION) {
                    hide_oxygen_warning(50.0f);
                    warning_50_active = false;
                    warning_50_shown = false;
                }
            }

            if (warning_20_shown) {
                warning_20_display_time += delta_time;
                if (warning_20_display_time >= WARNING_DURATION) {
                    hide_oxygen_warning(20.0f);
                    warning_20_active = false;
                    warning_20_shown = false;
                }
            }

            if (warning_5_shown) {
                warning_5_display_time += delta_time;
                if (warning_5_display_time >= WARNING_DURATION) {
                    hide_oxygen_warning(5.0f);
                    warning_5_active = false;
                    warning_5_shown = false;
                }
            }

            // Store current oxygen level for next frame comparison
            previous_oxygen_level = current_oxygen;
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
        // Skip if win transition is active
        if (win_transition_active) {
            return;
        }

        // Check current scene
        if (GM.get_current_scene() == 4) {  // Win screen
            return;  // Don't show GUI on win screen
        }

        if (GM.get_current_scene() != 2)
        {
            return;
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

        // 5) Update top UI goal percentage text
        EntityID goal_text_id = ecs_manager.find_entity_by_name("top_ui_goal_percentage_text");
        if (goal_text_id != INVALID_ENTITY_ID) {
            if (auto* text = get_component_safe<Text_Component>(goal_text_id)) {
                int percentage = static_cast<int>(stored_mineral_progress * 100);
                std::stringstream ss;
                ss << std::setw(2) << std::setfill('0') << percentage << "%";
                text->text = ss.str();

                // Store in Game Manager - critical for fixing the reset issue
                GM.set_stored_goal_percentage(static_cast<float>(percentage));
            }
        }
    }


    // ---------------------------------------------------------
    // OXYGEN TANK GUI
    // ---------------------------------------------------------
    void GUI_System::show_oxygen_tank_gui()
    {
        if (GM.get_current_scene() != 2)
        {
            return;
        }

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

    // ---------------------------------------------------------
    // Wormhole E Prompt
    // ---------------------------------------------------------\

#if 0
    void GUI_System::show_wormhole_tank_gui() {
        // Check if the entity already exists
        if (!wormhole_e_prompt_name.empty()) {
            EntityID existing_entity = ecs_manager.find_entity_by_name(wormhole_e_prompt_name);
            if (existing_entity != INVALID_ENTITY_ID) {
                // Entity already exists, so skip creating a new one
                return;
            }
        }

        // Generate unique name for wormhole
        wormhole_e_prompt_name = "wormhole_e_prompt";

        // Create E prompt
        EntityID e_prompt = ecs_manager.clone_entity_from_prefab("gui_container", wormhole_e_prompt_name);
        if (e_prompt != INVALID_ENTITY_ID) {
            if (auto* graphics = get_component_safe<Graphics_Component>(e_prompt)) {
                graphics->model_name = "square";
                graphics->texture_name = "E_Gold_02_Batch_14";
                graphics->color = glm::vec4(1.0f);
            }
            if (auto* transform = get_component_safe<Transform2D>(e_prompt)) {
                transform->position = Vec2D(wormhole_e_prompt_x, original_e_prompt_y); // Use updated x-position
             
                transform->scale = Vec2D(50.0f, 50.0f);
            }
            wormhole_e_prompt_animation_timer = 0.0f;
        }

        std::cout << "update the position for x_axis " << wormhole_e_prompt_x << "\n";
    }

#endif
    void GUI_System::show_wormhole_gui() {
        // Check if the entity already exists
        if (!wormhole_e_prompt_name.empty()) {
            EntityID existing_entity = ecs_manager.find_entity_by_name(wormhole_e_prompt_name);
            if (existing_entity != INVALID_ENTITY_ID) {
                // Entity already exists, so just update the stored ID and return
                wormhole_e_prompt = existing_entity;

                // Update position directly
                if (auto* transform = get_component_safe<Transform2D>(existing_entity)) {
                    transform->position.x = wormhole_e_prompt_x;
                }
                return;
            }
        }

        // Generate unique name for wormhole
        wormhole_e_prompt_name = "wormhole_e_prompt";

        // Create E prompt
        EntityID e_prompt = ecs_manager.clone_entity_from_prefab("gui_container", wormhole_e_prompt_name);
        if (e_prompt != INVALID_ENTITY_ID) {
            if (auto* graphics = get_component_safe<Graphics_Component>(e_prompt)) {
                graphics->model_name = "square";
                graphics->texture_name = "E_Gold_02_Batch_14";
                graphics->color = glm::vec4(1.0f);
            }
            if (auto* transform = get_component_safe<Transform2D>(e_prompt)) {
                transform->position = Vec2D(wormhole_e_prompt_x, wormhole_e_prompt_y);
                transform->scale = Vec2D(50.0f, 50.0f);
            }

            // Store the entity ID for future reference
            wormhole_e_prompt = e_prompt;
            wormhole_e_prompt_animation_timer = 0.0f;
        }

        //std::cout << "update the position for x_axis " << wormhole_e_prompt_x << "\n";
    }


#if 0
    void GUI_System::hide_wormhole_gui() {
        const std::vector<std::string*> entity_names = {
            &wormhole_e_prompt_name
        };

        for (auto* name_ptr : entity_names) {
            if (!name_ptr->empty()) {
                EntityID entity = ecs_manager.find_entity_by_name(*name_ptr);
                if (entity != INVALID_ENTITY_ID) {
                    ecs_manager.destroy_entity(entity);
                }
                // Always clear the name, even if the entity is not found
                name_ptr->clear();
            }
        }
    }

#endif
    void GUI_System::hide_wormhole_gui() {
        const std::vector<std::string*> entity_names = {
            &wormhole_e_prompt_name
        };

        for (auto* name_ptr : entity_names) {
            if (!name_ptr->empty()) {
                EntityID entity = ecs_manager.find_entity_by_name(*name_ptr);
                if (entity != INVALID_ENTITY_ID) {
                    ecs_manager.destroy_entity(entity);

                    // Reset the wormhole_e_prompt ID after destroying the entity
                    if (name_ptr == &wormhole_e_prompt_name) {
                        wormhole_e_prompt = INVALID_ENTITY_ID;
                    }
                }

                // Always clear the name, even if the entity is not found
                name_ptr->clear();
            }
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
                graphics->texture_name = "pause_screen_updated";
                graphics->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            }
            if (auto* transform = get_component_safe<Transform2D>(overlay)) {
                transform->position = Vec2D(0.0f, -50.0f);
                transform->scale = Vec2D(1980.0f, 1180.0f);
            }
            pause_menu_entities["overlay"] = overlay;
        }

        // Create Resume button - positioned higher
        EntityID resume_button = ecs_manager.clone_entity_from_prefab("gui_container", "resume_button");
        if (resume_button != INVALID_ENTITY_ID) {
            if (auto* graphics = get_component_safe<Graphics_Component>(resume_button)) {
                graphics->model_name = "square";
                graphics->texture_name = "Resume_Game_Batch_30_NORMAL";
                graphics->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            }
            if (auto* transform = get_component_safe<Transform2D>(resume_button)) {
                transform->position = Vec2D(0.0f, 0.0f);  // Moved up significantly
                transform->scale = Vec2D(240.0f, 80.0f);
            }

            // Add empty Audio Component - needed for hover detection
            Audio_Component audio_comp;
            audio_comp.add_sound("button_hover", "sfx_button_hover", AudioType::UI, 1, static_cast<float>(0.7), 1.0, false, true, false);
            ecs_manager.add_component(resume_button, audio_comp);

            pause_menu_entities["resume"] = resume_button;
        }

        // Create Restart button - keeping original spacing but moved up
        EntityID restart_button = ecs_manager.clone_entity_from_prefab("gui_container", "restart_button");
        if (restart_button != INVALID_ENTITY_ID) {
            if (auto* graphics = get_component_safe<Graphics_Component>(restart_button)) {
                graphics->model_name = "square";
                graphics->texture_name = "Restart_Batch_14_NORMAL";
                graphics->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            }
            if (auto* transform = get_component_safe<Transform2D>(restart_button)) {
                transform->position = Vec2D(0.0f, -100.0f);  // Moved up to 0
                transform->scale = Vec2D(240.0f, 80.0f);
            }

            // Add empty Audio Component - needed for hover detection
            Audio_Component audio_comp;
            audio_comp.add_sound("button_hover", "sfx_button_hover", AudioType::UI, 1, static_cast<float>(0.7), 1.0, false, true, false);
            ecs_manager.add_component(restart_button, audio_comp);

            pause_menu_entities["restart"] = restart_button;
        }

        // Create Main Menu button - keeping original spacing but moved up
        EntityID main_menu_button = ecs_manager.clone_entity_from_prefab("gui_container", "main_menu_button");
        if (main_menu_button != INVALID_ENTITY_ID) {
            if (auto* graphics = get_component_safe<Graphics_Component>(main_menu_button)) {
                graphics->model_name = "square";
                graphics->texture_name = "Main_Menu_Batch_14_NORMAL";
                graphics->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            }
            if (auto* transform = get_component_safe<Transform2D>(main_menu_button)) {
                transform->position = Vec2D(0.0f, -200.0f);  // Moved up relative to previous positions
                transform->scale = Vec2D(240.0f, 80.0f);
            }

            // Add empty Audio Component - needed for hover detection
            Audio_Component audio_comp;
            audio_comp.add_sound("button_hover", "sfx_button_hover", AudioType::UI, 1, static_cast<float>(0.7), 1.0, false, true, false);
            ecs_manager.add_component(main_menu_button, audio_comp);

            pause_menu_entities["main_menu"] = main_menu_button;
        }

        // Create Quit button - keeping original spacing but moved up
        EntityID quit_button = ecs_manager.clone_entity_from_prefab("gui_container", "quit_button");
        if (quit_button != INVALID_ENTITY_ID) {
            if (auto* graphics = get_component_safe<Graphics_Component>(quit_button)) {
                graphics->model_name = "square";
                graphics->texture_name = "Quit_Game_Batch_30_NORMAL";
                graphics->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            }
            if (auto* transform = get_component_safe<Transform2D>(quit_button)) {
                transform->position = Vec2D(0.0f, -300.0f);  // Moved up relative to previous positions
                transform->scale = Vec2D(240.0f, 80.0f);
            }

            // Add empty Audio Component - needed for hover detection
            Audio_Component audio_comp;
            audio_comp.add_sound("button_hover", "sfx_button_hover", AudioType::UI, 1, static_cast<float>(0.7), 1.0, false, true, false);
            ecs_manager.add_component(quit_button, audio_comp);

            pause_menu_entities["quit"] = quit_button;
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
            "main_menu",
            "quit"
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
            "main_menu_button",
            "quit_button"
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

    //Static variable to hold timer for clicking sound for quit button
    static float timer = 0.0f;
    static bool stop_playing = false;

    void GUI_System::check_pause_menu_button_collision(float delta_time) {
        (void)delta_time;  // Unused parameter

        // Return early if we are not actually paused
        if (!GM.is_paused()) return;

        // Skip if fade transition is in progress
        if (fade_active) return;

        // NEW: Skip if game is transitioning (Game_Manager flag)
        if (GM.is_transitioning()) return;

        // Get the mining script
        auto mining_script = std::dynamic_pointer_cast<Mining_Script>(LGM.get_script("mining_script"));
        auto tutorial_script = std::dynamic_pointer_cast<Tutorial_Script>(LGM.get_script("tutorial_script"));

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

            if (entity_name != "resume_button" &&
                entity_name != "restart_button" &&
                entity_name != "main_menu_button" &&
                entity_name != "quit_button")
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
            float button_screen_y = center_y - transform.position.y - 75.0f;

            float screen_left = button_screen_x - half_width;
            float screen_right = button_screen_x + half_width;
            float screen_top = button_screen_y - half_height;
            float screen_bottom = button_screen_y + half_height;

            // Check if mouse is over the button in screen coordinates
            bool is_hovered =
                (screen_mouse_x >= screen_left && screen_mouse_x <= screen_right) &&
                (screen_mouse_y >= screen_top && screen_mouse_y <= screen_bottom);

            // Define texture and sounds
            std::string base_texture;
            std::string hover_sound = "button_hover";
            std::string click_sound = "main_menu";
            audio.add_sound(click_sound, "sfx_mainmenu_button", AudioType::UI, 5, 1.0, 1.0, false, true, false);

            if (entity_name == "resume_button") {
                base_texture = "Resume_Game_Batch_30";
            }
            else if (entity_name == "restart_button") {
                base_texture = "Restart_Batch_14";
            }
            else if (entity_name == "main_menu_button") {
                base_texture = "Main_Menu_Batch_14";
            }
            else if (entity_name == "quit_button") {
                base_texture = "Quit_Game_Batch_30";
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
                    //ADM.play_now(entity_id, click_sound, audio);

                    // Handle the actual button click
                    if (entity_name == "resume_button") {
                        ADM.play_now(entity_id, click_sound, audio);
                        GM.set_paused(false);
                        hide_pause_menu();
                    }
                    else if (entity_name == "restart_button") {
                        LM.write_log("Restart button pressed - starting fade transition to scene 2");

                        // Play click sound
                        ADM.play_now(entity_id, click_sound, audio);

                        mining_script->clear_tnt_to_destroy();

                        // Unpause first
                        GM.set_paused(false);

                        // Set player dead false
                        GM.set_player_dead_state(false);

                        // Force hide pause menu first
                        hide_pause_menu();

                        // Reset all GUI states first - similar to your TNT code
                        reset_all_game_state();

                        // Start the fade transition to scene 2
                        start_screen_fade(true, "scene2.scn", 2);
                    }
                    else if (entity_name == "main_menu_button") {
                        LM.write_log("Main Menu button pressed - starting fade transition to main menu");

                        // Play click sound
                        ADM.play_now(entity_id, click_sound, audio);

                        // Unpause first
                        GM.set_paused(false);

                        mining_script->clear_tnt_to_destroy();

                      

                        // Set player dead false
                        GM.set_player_dead_state(false);

                        // Force hide pause menu first
                        hide_pause_menu();

                        // Reset all GUI states
                        reset_all_game_state();

                        // Start the fade transition to main menu
                        start_screen_fade(true, "main_menu.scn", 0);
                    }
                    else if (entity_name == "quit_button") {

                        if (!stop_playing) {  //Only play sound once
                            ADM.play_now(entity_id, click_sound, audio);
                            stop_playing = true;
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

        if (stop_playing) {

            //Time per frame
            float delta_time = FPSM.get_delta_time();

            //Progress in timer
            timer += delta_time;

            //Switching direction
            if (timer >= 0.4f) {

                LM.write_log("Quit button pressed - ending game");
                GM.set_game_over(true);

            }
        }
    }

    void GUI_System::set_wormhole_e_prompt_x(float new_x_position) {
        wormhole_e_prompt_x = new_x_position;

    }

    void GUI_System::set_wormhole_e_prompt_y(float new_y_position) {
        wormhole_e_prompt_y = new_y_position + 102.0f;

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

            //std::cout << game_over_shown << " game over in gui system^^^^^^^^^^^^^^^^^^^^^^\n";
        }

        std::string click_sound = "main_menu";

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
                transform->scale = Vec2D(240.0f, 80.0f);
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
                audio_comp.add_sound("button_hover", "sfx_button_hover", AudioType::UI, 1, static_cast<float>(0.7), 1.0, false, true, false);
                audio_comp.add_sound(click_sound, "sfx_mainmenu_button", AudioType::UI, 1, 1.0, 1.0, false, true, false);
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
                transform->scale = Vec2D(240.0f, 80.0f);
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
                audio_comp.add_sound("button_hover", "sfx_button_hover", AudioType::UI, 1, static_cast<float>(0.7), 1.0, false, true, false);
                audio_comp.add_sound(click_sound, "sfx_mainmenu_button", AudioType::UI, 1, 1.0, 1.0, false, true, false);
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

        // Reset the flag FIRST
        game_over_shown = false;
        gameover_audio_played = false;

        // Get ALL possible button entities by name to ensure we catch everything
        std::vector<std::string> possible_names = {
            "game_over_overlay",
            "restart_button",
            "main_menu_button",
            "game_over_text"
        };

        // Find and destroy all game over UI elements by name
        for (const auto& name : possible_names) {
            EntityID entity_id = ecs_manager.find_entity_by_name(name);
            if (entity_id != INVALID_ENTITY_ID) {
                ecs_manager.destroy_entity(entity_id);
                LM.write_log("Destroyed game over UI element: %s (ID: %u)", name.c_str(), entity_id);
            }
        }

        // Also find and destroy all stored entities in the map
        for (const auto& [key, entity_id] : game_over_entities) {
            if (entity_id != INVALID_ENTITY_ID) {
                if (ecs_manager.get_entity(entity_id)) { // Check if entity still exists
                    ecs_manager.destroy_entity(entity_id);
                    LM.write_log("Destroyed stored game over entity: %s (ID: %u)", key.c_str(), entity_id);
                }
            }
        }

        // Clear the entities map and hover states
        game_over_entities.clear();
        game_over_button_hover_states.clear();

        LM.write_log("Game over UI cleanup completed");
    }

    void GUI_System::check_game_over_button_collision(float delta_time) {
        (void)delta_time;  // Unused parameter

        // Return early if game over screen is not displayed
        if (!game_over_shown) return;

        // Skip if fade transition is in progress
        if (fade_active) return;

        // Get the current mouse position in screen coordinates
        double screen_mouse_x, screen_mouse_y;
        IM.get_mouse_position(screen_mouse_x, screen_mouse_y);

        // Retrieve window dimensions
        int window_width = WC.get_win_width();
        int window_height = WC.get_win_height();

        // Calculate the center of the screen
        float center_x = static_cast<float>(window_width) / 2.0f;
        float center_y = static_cast<float>(window_height) / 2.0f;

        EntityID player = ecs_manager.find_entity_by_name(DEFAULT_PLAYER_NAME);
        if (player) {
            if (ecs_manager.has_component<Audio_Component>(player) && !gameover_audio_played) {
                auto& audio_comp = ecs_manager.get_component<Audio_Component>(player);
                ADM.play_now(player, "game over", audio_comp);
                gameover_audio_played = true;
            }
        }

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

            // Check hover
            bool is_hovered =
                (screen_mouse_x >= screen_left && screen_mouse_x <= screen_right) &&
                (screen_mouse_y >= screen_top && screen_mouse_y <= screen_bottom);

            // Base texture name
            std::string base_texture;
            if (key == "restart") {
                base_texture = "Restart_Batch_14";
            }
            else if (key == "main_menu") {
                base_texture = "Main_Menu_Batch_14";
            }
            std::string hover_sound = "button_hover";
            std::string click_sound = "main_menu";

            // Ensure we have a recorded hover state
            if (game_over_button_hover_states.find(entity->get_name()) == game_over_button_hover_states.end()) {
                game_over_button_hover_states[entity->get_name()] = false;
            }

            if (is_hovered) {

                //std::cout << "yes !!! is hovered====================\n";
                // Change texture to highlighted if not pressed
                if (graphics.texture_name != base_texture + "_HIGHLIGHTED" &&
                    graphics.texture_name != base_texture + "_PRESSED")
                {
                    graphics.texture_name = base_texture + "_HIGHLIGHTED";
                }

                // Play hover sound if newly hovering
                if (!game_over_button_hover_states[entity->get_name()]) {
                    ADM.play_now(button_id, hover_sound, audio);
                    game_over_button_hover_states[entity->get_name()] = true;
                }

                // Check for click
                if (IM.is_mouse_button_held(GLFW_MOUSE_BUTTON_LEFT)) {
                    LM.write_log("CLICK DETECTED on button: %s", key.c_str());
                    graphics.texture_name = base_texture + "_PRESSED";
                    ADM.play_now(button_id, click_sound, audio);

                    // Handle specific button
                    if (key == "restart") {
                        LM.write_log("Game over - Restart button pressed - starting fade transition to scene 2");
                        //std::cout << GM.get_player_dead_state() << " in GUI system restart b4 set\n";
                        // Set player dead false
                        GM.set_player_dead_state(false);
                        //std::cout << GM.get_player_dead_state() << " in GUI system restart ----------------- after set" << "\n";
                        // Set restart true
                        //GM.set_restarting(true);
                        
                        // Hide game over menu
                        hide_game_over_menu();
                        
                        game_over_shown = false;  // Explicitly reset the flag

                        // Reset all game state
                        reset_all_game_state();

                        // Start the fade transition to scene 2
                        start_screen_fade(true, "scene2.scn", 2);
                    }
                    else if (key == "main_menu") {
                        LM.write_log("Game over - Main Menu button pressed - starting fade transition to main menu");

                        // Set player dead false
                        GM.set_player_dead_state(false);
    
                        // Hide game over menu
                        hide_game_over_menu();

                        // Reset all game state
                        reset_all_game_state();

                        // Start the fade transition to main menu
                        start_screen_fade(true, "main_menu.scn", 0);
                    }

                    // Return so we do not process any other buttons
                    return;
                }
            }
            else {
                // Not hovered: set to normal
                if (graphics.texture_name != base_texture + "_NORMAL") {
                    graphics.texture_name = base_texture + "_NORMAL";
                }

                // Reset hover state
                game_over_button_hover_states[entity->get_name()] = false;
            }
        }
    }

    void GUI_System::start_screen_fade(bool fade_type, const std::string& dest_scene, int dest_scene_num) {
        // Notify Game_Manager that transition is starting
        GM.set_transitioning(true);

        // Skip fade for credits transitions if configured that way
        if (skip_fade_for_credits && dest_scene == "credit.scn") {
            LM.write_log("GUI_System::start_screen_fade(): Skipping fade for credits transition");
            direct_scene_transition(dest_scene, dest_scene_num);
            return;
        }

        // For win screen transition, set the special flag
        if (dest_scene == "win_screen.scn") {
            transitioning_to_win_screen = true;
            LM.write_log("Setting win screen transition flag");
        }

        LM.write_log("GUI_System::start_screen_fade(): Starting %s transition to scene: %s (#%d)",
            fade_type ? "fade in" : "fade out", dest_scene.c_str(), dest_scene_num);

        // For fade out, we need to ensure we have an overlay first
        if (!fade_type) {
            // If this is a fade out, make sure we have an overlay at full opacity
            EntityID fade_entity = ecs_manager.find_entity_by_name(fade_overlay_name);
            if (fade_entity == INVALID_ENTITY_ID) {
                create_fade_overlay();
                fade_entity = ecs_manager.find_entity_by_name(fade_overlay_name);

                if (fade_entity != INVALID_ENTITY_ID &&
                    ecs_manager.has_component<Graphics_Component>(fade_entity)) {
                    auto& graphics = ecs_manager.get_component<Graphics_Component>(fade_entity);
                    graphics.color.a = 1.0f;  // Start fully opaque for fade out
                }
            }
        }
        else {
            // For fade in, clear any existing overlay
            remove_fade_overlay();
            create_fade_overlay();  // And create a fresh one
        }

        // Setup fade parameters
        fade_active = true;
        fade_in = fade_type;
        fade_timer = 0.0f;
        destination_scene = dest_scene;
        destination_scene_number = dest_scene_num;

        ADM.stop_groups(GroupType::TYPE_BGM);
        ADM.stop_groups(GroupType::TYPE_SFX);
        
        // Set initial opacity based on fade direction
        EntityID fade_entity = ecs_manager.find_entity_by_name(fade_overlay_name);
        if (fade_entity != INVALID_ENTITY_ID && ecs_manager.has_component<Graphics_Component>(fade_entity)) {
            auto& graphics = ecs_manager.get_component<Graphics_Component>(fade_entity);
            graphics.color.a = fade_in ? 0.0f : 1.0f;  // Start transparent for fade in, opaque for fade out
        }
    }


    bool GUI_System::update_screen_fade(float delta_time) {
        if (!fade_active) return false;

        // Update fade timer
        fade_timer += delta_time;

        // Calculate fade progress (0.0 to 1.0)
        float progress = std::min(fade_timer / fade_duration, 1.0f);

        // Find the fade overlay entity by name
        EntityID fade_entity = ecs_manager.find_entity_by_name(fade_overlay_name);

        // Update fade overlay opacity
        if (fade_entity != INVALID_ENTITY_ID && ecs_manager.has_component<Graphics_Component>(fade_entity)) {
            auto& graphics = ecs_manager.get_component<Graphics_Component>(fade_entity);

            // Set alpha based on fade direction
            if (fade_in) {
                graphics.color.a = progress;  // Increasing opacity for fade in
            }
            else {
                graphics.color.a = 1.0f - progress;  // Decreasing opacity for fade out
            }
        }
        else if (fade_active) {
            // Recreate overlay if it was somehow lost but fade is still active
            create_fade_overlay();

            fade_entity = ecs_manager.find_entity_by_name(fade_overlay_name);
            if (fade_entity != INVALID_ENTITY_ID && ecs_manager.has_component<Graphics_Component>(fade_entity)) {
                auto& graphics = ecs_manager.get_component<Graphics_Component>(fade_entity);
                graphics.color.a = fade_in ? progress : (1.0f - progress);
            }
        }

        // Check if fade is complete
        if (progress >= 1.0f) {
            // If fading in, proceed to scene transition
            if (fade_in && !destination_scene.empty()) {
                LM.write_log("GUI_System::update_screen_fade(): Fade complete, loading scene: %s", destination_scene.c_str());

                // First clear dynamic entities
                for (auto& system : ecs_manager.get_systems()) {
                    if (auto* movement_system = dynamic_cast<Movement_System*>(system.get())) {
                        movement_system->clear_dynamic_entities();
                        break;
                    }
                }

                // Create full path to the scene file and load it
                const std::string SCENES = "Scenes";
                std::string scene_path = ASM.get_full_path(SCENES, destination_scene);

                // Store the destination scene info (important for fade out)
                std::string loaded_scene = destination_scene;
                int loaded_scene_number = destination_scene_number;

                // IMPORTANT: Keep the fade overlay active and fully opaque during scene transition
                // Instead of removing it, ensure it's at full opacity
                if (fade_entity != INVALID_ENTITY_ID && ecs_manager.has_component<Graphics_Component>(fade_entity)) {
                    auto& graphics = ecs_manager.get_component<Graphics_Component>(fade_entity);
                    graphics.color.a = 1.0f;  // Ensure full opacity during transition
                }

                // Reset fade state for next phase
                fade_active = false;
                fade_timer = 0.0f;
                destination_scene = "";
                destination_scene_number = -1;

                // Now load the scene while keeping the overlay visible
                if (SM.load_scene(scene_path.c_str())) {
                    // Reset camera position
                    auto& camera = GFXM.get_camera();
                    camera.pos_x = DEFAULT_CAMERA_POS_X;
                    camera.pos_y = DEFAULT_CAMERA_POS_Y;

                    // Manage audio during transition
                    //ADM.stop_mastergroup();
                    ADM.stop_groups(GroupType::TYPE_BGM);
                    ADM.stop_groups(GroupType::TYPE_SFX);

                    // Update current scene in Game Manager
                    GM.set_current_scene(loaded_scene_number);

                    // Update IMGUI Manager's current file
                    IMGUIM.set_current_file_shown(loaded_scene);

                    // Reset panic for gameplay
                    GM.reset_panic();

                    // Reset player position if it exists
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

                    // IMPORTANT: Recreate the fade overlay after loading the scene
                    // The original overlay was likely destroyed during scene loading
                    create_fade_overlay();

                    // Get the new overlay and ensure it's fully opaque
                    EntityID new_fade_entity = ecs_manager.find_entity_by_name(fade_overlay_name);
                    if (new_fade_entity != INVALID_ENTITY_ID &&
                        ecs_manager.has_component<Graphics_Component>(new_fade_entity)) {
                        auto& graphics = ecs_manager.get_component<Graphics_Component>(new_fade_entity);
                        graphics.color.a = 1.0f;  // Start fully opaque for fade out
                    }

                    // Handle fade out duration based on transition type
                    if (transitioning_to_win_screen) {
                        // Use full duration for win screen transition
                        fade_duration = 3.0f;
                        transitioning_to_win_screen = false;  // Reset the flag
                        LM.write_log("Win screen transition: Using %.1f second fade-out", fade_duration);
                    }
                    else {
                        // Use faster fade-out for other transitions
                        fade_duration = 1.5f;
                    }

                    // Start fade out transition
                    start_screen_fade(false, "", -1);
                }
                else {
                    LM.write_log("GUI_System::update_screen_fade(): Failed to load scene: %s", loaded_scene.c_str());
                    // Clean up fade overlay on failure
                    remove_fade_overlay();
                }

                return false;
            }
        
            // Reset fade state after completion
            fade_active = false;
            fade_timer = 0.0f;

            // If this was a fade out, remove the overlay
            if (!fade_in) {
                remove_fade_overlay();
                // Reset win transition flag when fade out is complete
                win_transition_active = false;

                // Mark transition as complete
                GM.set_transitioning(false);
            }

            return true;  // Fade is complete
        }

        return false;  // Fade still in progress
    }

    void GUI_System::create_fade_overlay() {
        // First make sure we don't have an existing overlay
        remove_fade_overlay();

        // Create a full-screen overlay for fade effect
        EntityID fade_entity = ecs_manager.create_entity(fade_overlay_name);

        if (fade_entity != INVALID_ENTITY_ID) {
            // Add Transform2D component
            Transform2D transform;
            transform.position = Vec2D(0.0f, 0.0f);  // Center of screen
            transform.scale = Vec2D(2000.0f, 1400.0f);  // Slightly larger than screen size (1920x1080)
            ecs_manager.add_component(fade_entity, transform);

            // Add Graphics_Component
            Graphics_Component graphics;
            graphics.model_name = "square";
            graphics.texture_name = "Transition_Batch_14";  // Use your transition texture
            graphics.color = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);  // Start fully transparent
            ecs_manager.add_component(fade_entity, graphics);

            // Add GUI component to ensure it gets rendered on top
            GUI_Component gui_comp;
            gui_comp.is_container = true;
            ecs_manager.add_component(fade_entity, gui_comp);

            // Add it to our entity list so the system can update it
            add_entity(fade_entity);

            LM.write_log("GUI_System::create_fade_overlay(): Created fade overlay with name '%s'", fade_overlay_name.c_str());
        }
        else {
            LM.write_log("GUI_System::create_fade_overlay(): Failed to create fade overlay entity");
        }
    }

    void GUI_System::remove_fade_overlay() {
        // Find by name instead of ID
        EntityID fade_entity = ecs_manager.find_entity_by_name(fade_overlay_name);
        if (fade_entity != INVALID_ENTITY_ID) {
            // Remove from our system first
            if (has_entity(fade_entity)) {
                remove_entity(fade_entity);
            }

            // Then destroy the entity
            ecs_manager.destroy_entity(fade_entity);
            LM.write_log("GUI_System::remove_fade_overlay(): Removed fade overlay '%s'", fade_overlay_name.c_str());
        }
    }

    bool GUI_System::direct_scene_transition(const std::string& scene_file, int scene_num) {
        LM.write_log("GUI_System::direct_scene_transition(): Loading scene %s (#%d) without fade",
            scene_file.c_str(), scene_num);

        // Clear dynamic entities first
        for (auto& system : ecs_manager.get_systems()) {
            if (auto* movement_system = dynamic_cast<Movement_System*>(system.get())) {
                movement_system->clear_dynamic_entities();
                break;
            }
        }

        // Create full path to the scene file
        const std::string SCENES = "Scenes";
        std::string scene_path = ASM.get_full_path(SCENES, scene_file);

        // Make sure any active fade is canceled and removed
        fade_active = false;
        fade_timer = 0.0f;
        remove_fade_overlay();

        // Load the scene
        if (SM.load_scene(scene_path.c_str())) {
            // Reset camera position
            auto& camera = GFXM.get_camera();
            camera.pos_x = DEFAULT_CAMERA_POS_X;
            camera.pos_y = DEFAULT_CAMERA_POS_Y;

            // Ensure no fade overlays are present in the new scene
            EntityID existing_fade = ecs_manager.find_entity_by_name(fade_overlay_name);
            if (existing_fade != INVALID_ENTITY_ID) {
                ecs_manager.destroy_entity(existing_fade);
            }

            // Update current scene in Game Manager
            GM.set_current_scene(scene_num);

            // Update IMGUI Manager's current file
            IMGUIM.set_current_file_shown(scene_file);

            // Reset player position if it exists
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

            LM.write_log("GUI_System::direct_scene_transition(): Successfully loaded scene");
            return true;
        }
        else {
            LM.write_log("GUI_System::direct_scene_transition(): Failed to load scene: %s", scene_path.c_str());
            return false;
        }
    }
}
