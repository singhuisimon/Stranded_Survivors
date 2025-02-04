/**
 * @file Game_Manager.cpp
 * @brief Implements the Game_Manager class helper functions.
 * @author Simon Chan (75%), Chua Wen Bin Kenny (12%), Amanda Leow (6%), Saw Hui Shan (4%), Liliana Hanawardani (3%)
 * @date September 21, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

 // Include header file
#include "Game_Manager.h"

// Include other managers
#include "Log_Manager.h"
#include "ECS_Manager.h"
#include "FPS_Manager.h"
#include "Serialization_Manager.h"
#include "Input_Manager.h"
#include "Graphics_Manager.h"
#include "Audio_Manager.h"

// Include utility
#include "../Utility/Constant.h"
#include "../Utility/Globals.h"

// Include systems
#include "../System/Audio_System.h" // Add this for Audio System access
#include "../System/GUI_System.h"  // Add this for GUI system access
#include "../System/Animation_System.h"  // For player_direction
#include "../System/Collision_System.h" // for click entity object
#include "../System/Particle_System.h" // To create particles 

#include "../Utility/Entity_Selector_Helper.h"

// Include iostream for console output
#include <iostream>
#include <random>
#include <chrono>
#include <iomanip>  // for std::setw and std::setfill
#include <sstream>  // for std::stringstream

namespace lof {

    float imgui_camara_pos_x = 0.0f;
    float imgui_camera_pos_y = 0.0f;
    unsigned int mining_strength = DEFAULT_STRENGTH;
    Game_Manager::Game_Manager()
        : m_game_over(false), m_step_count(0) {
        set_type("Game_Manager");
    }

    Game_Manager& Game_Manager::get_instance() {
        static Game_Manager instance;
        return instance;
    }

    int Game_Manager::start_up() {
        if (is_started()) {
            return 0; // Already started
        }

        // -------------------------- Log Manager Start Up --------------------------
        if (LM.start_up() != 0) {
            // If Log_Manager fails to start, there's no way to log the error using LM
            return -1;
        }
        else {
            LM.write_log("Game_Manager::start_up(): Log_Manager start_up() successful");
        }

        // -------------------------- ECS Manager Start Up --------------------------
        if (ECSM.start_up() != 0) {
            LM.write_log("Game_Manager::start_up(): ECS_Manager start_up() failed");
            LM.shut_down();
            return -2;
        }
        else {
            LM.write_log("Game_Manager::start_up(): ECS_Manager start_up() successful");
        }

        // -------------------------- Serialization Manager Start Up -----------------
        if (SM.start_up() != 0) {
            LM.write_log("Game_Manager::start_up(): Serialization_Manager start_up() failed");
            ECSM.shut_down();
            LM.shut_down();
            return -3;
        }
        else {
            LM.write_log("Game_Manager::start_up(): Serialization_Manager start_up() successful");
        }

        // -------------------------- FPS Manager Start Up --------------------------
        if (FPSM.start_up() != 0) {
            LM.write_log("Game_Manager::start_up(): FPS_Manager start_up() failed");
            SM.shut_down();
            ECSM.shut_down();
            LM.shut_down();
            return -4;
        }
        else {
            LM.write_log("Game_Manager::start_up(): FPS_Manager start_up() successful");
        }

        // -------------------------- Input Manager Start Up --------------------------
        if (IM.start_up() != 0) {
            LM.write_log("Game_Manager::start_up(): Input_Manager start_up() failed");
            FPSM.shut_down();
            SM.shut_down();
            ECSM.shut_down();
            LM.shut_down();
            return -5;
        }
        else {
            LM.write_log("Game_Manager::start_up(): Input_Manager start_up() successful");
        }
        // ---------------------------- Audio Manager Start Up ---------------------------
        if (ADM.start_up() != 0) {
            LM.write_log("Game_Manager::start_up(): Audio_Manager start_up() failed");
            IM.shut_down();
            FPSM.shut_down();
            SM.shut_down();
            ECSM.shut_down();
            LM.shut_down();
            return -6;
        }
        // -------------------------- Graphics Manager Start Up --------------------------
        if (GFXM.start_up() != 0) {
            LM.write_log("Game_Manager::start_up(): Graphics_Manager start_up() failed");
            ADM.shut_down();
            IM.shut_down();
            FPSM.shut_down();
            SM.shut_down();
            ECSM.shut_down();
            LM.shut_down();
            return -7;
        }
        else {
            LM.write_log("Game_Manager::start_up(): Graphics_Manager start_up() successful");
        }

        m_is_started = true;
        LM.write_log("Game_Manager::start_up(): Game_Manager started");
        std::cout << "Game_Manager started successfully." << std::endl;

        return 0;
    }

    void Game_Manager::shut_down() {
        if (!is_started()) {
            return;
        }

        // Shut down managers in reverse order of startup
        GFXM.shut_down(); // Graphics_Manager
        ADM.shut_down();  // Audio_Manager
        IM.shut_down();   // Input_Manager
        FPSM.shut_down(); // FPS_Manager
        SM.shut_down();   // Serialization_Manager
        ECSM.shut_down(); // ECS_Manager
        LM.shut_down();   // Log_Manager

        m_is_started = false;
        std::cout << "Game_Manager shut down successfully." << std::endl;
    }

  

    EntityInfo& selectedEntityInfo = ESS.get_selected_entity_info(); // for imgui
    EntityID selectedID = INVALID_ENTITY_ID; // for imgui

    //EntityID selectedID = static_cast<EntityID>(-1); // for imgui
    void Game_Manager::update(float delta_time) {

        // Check if the game manager is started
        if (!is_started()) {
            LM.write_log("Game_Manager::update(): Game_Manager not started");
            return;
        }

        //std::cout << "This is seleteed entity id no: " << selectedEntityID << "\n";
        try {
            // Simulate a crash when the 'P' key is pressed
            if (IM.is_key_pressed(GLFW_KEY_P)) {
                LM.write_log("Game_Manager::update(): Simulated crash. 'P' key was pressed.");
                throw std::runtime_error("Simulated crash: 'P' key was pressed.");
            }
        }
        catch (const std::exception& e) {
            LM.write_log("Game_Manager::update(): Exception caught: %s", e.what());
        }

        // Check for game over condition based on input, before IM update
        if (IM.is_key_pressed(GLFW_KEY_ESCAPE)) {
            set_game_over(true);
            LM.write_log("Game_Manager::update(): Escape key pressed. Setting game_over to true.");
            //std::cout << "Escape key pressed. Closing the game." << std::endl;
        }

        //to pause all the sound that is playing
        if (IM.is_key_pressed(GLFW_KEY_5) && !level_editor_mode) {
            ADM.pause_resume_mastergroup();
        }

        //to ensure sound pause during level_editor_mode
        if (IM.is_key_pressed(GLFW_KEY_TAB)) {
            ADM.pause_resume_mastergroup();
        }

        //commented out this is for me to test - Amanda
        /*if (IM.is_key_held(GLFW_KEY_J)) {
            oxygen_level--;
            std::cout << "current oxygen level " << oxygen_level << std::endl;
            increasing = false;
        }

        if (IM.is_key_held(GLFW_KEY_K)) {
            oxygen_level++;
            std::cout << "current oxygen level " << oxygen_level << std::endl;
            increasing = true;
        }
        std::cout << "current oxygen levvel outside " << oxygen_level << std::endl;*/
        ADM.update_bgm_layering(current_scene, oxygen_level, increasing);

        // Handle player movement and physics input
        EntityID player_id = ECSM.find_entity_by_name(DEFAULT_PLAYER_NAME);

        // Code portion if in gameplay mode or player exists
        if (player_id != INVALID_ENTITY_ID && !level_editor_mode) {  // If player entity exists

            // Add oxygen update logic here, before the UI positioning
            oxygen_update_timer += delta_time;
            if (oxygen_update_timer >= 1.0f) { // Every second
                oxygen_update_timer = 0.0f;
                current_oxygen_level = std::max(0.0f, current_oxygen_level - oxygen_drain_rate);
                // Update panic level inversely to oxygen level
                current_panic_level = 100.0f - current_oxygen_level;
            }

            // Update top UI overlay position to follow player
            EntityID ui_overlay_id = ECSM.find_entity_by_name("top_ui_overlay");

            EntityID oxygen_meter_fill_id = ECSM.find_entity_by_name("top_ui_oxygen_meter_fill");
            EntityID oxygen_meter_id = ECSM.find_entity_by_name("top_ui_oxygen_meter");

            EntityID panic_meter_fill_id = ECSM.find_entity_by_name("top_ui_panik_meter_fill");
            EntityID panic_meter_id = ECSM.find_entity_by_name("top_ui_panik_meter");

            EntityID mineral_texture_id = ECSM.find_entity_by_name("top_ui_mineral_texture");
            EntityID timer_icon_id = ECSM.find_entity_by_name("top_ui_timer");
            EntityID goal_text_id = ECSM.find_entity_by_name("top_ui_goal_text");

            EntityID oxygen_text_id = ECSM.find_entity_by_name("top_ui_oxygen_text");
            EntityID oxygen_percentage_text_id = ECSM.find_entity_by_name("top_ui_oxygen_percentage_text");
            EntityID panic_text_id = ECSM.find_entity_by_name("top_ui_panic_text");
            EntityID mineral_count_text_id = ECSM.find_entity_by_name("top_ui_mineral_count_text");
            EntityID timer_count_text_id = ECSM.find_entity_by_name("top_ui_timer_count_text");
            EntityID goal_percentage_count_text_id = ECSM.find_entity_by_name("top_ui_goal_percentage_text");

            if (ui_overlay_id != INVALID_ENTITY_ID) {
                auto& player_transform = ECSM.get_component<Transform2D>(player_id);
                auto& ui_transform = ECSM.get_component<Transform2D>(ui_overlay_id);

                // Define layout constants for vertical stacking
                constexpr float VERTICAL_OFFSET = 500.0f;        // Distance above player
                constexpr float METER_SPACING = 50.0f;           // Vertical space between meters
                constexpr float METER_WIDTH = 400.0f;            // Width of the meters
                constexpr float METER_HEIGHT = 40.0f;            // Height of each meter bar
                constexpr float TEXT_OFFSET_Y = 10.0f;           // Vertical offset from the UI element

                // Calculate base position for UI elements
                Vec2D base_position{
                    0.0f,
                    player_transform.position.y + VERTICAL_OFFSET
                };

                // Update main UI overlay position
                ui_transform.position = base_position;
                ui_transform.prev_position = ui_transform.position;

                // Position oxygen meter and fill (top meter)
                if (oxygen_meter_id != INVALID_ENTITY_ID &&
                    ECSM.has_component<Transform2D>(oxygen_meter_id) &&
                    ECSM.has_component<Transform2D>(oxygen_meter_fill_id)) {
                    auto& oxygen_transform = ECSM.get_component<Transform2D>(oxygen_meter_id);
                    auto& oxygen_fill_transform = ECSM.get_component<Transform2D>(oxygen_meter_fill_id);
                    auto& oxygen_fill_graphics = ECSM.get_component<Graphics_Component>(oxygen_meter_fill_id);

                    // Position and scale for meter stays the same
                    oxygen_transform.position = {
                        base_position.x - 573.0f,
                        base_position.y
                    };
                    oxygen_transform.scale = Vec2D(METER_WIDTH, METER_HEIGHT);
                    oxygen_transform.prev_position = oxygen_transform.position;

                    // Calculate the new width of the fill bar
                    float new_width = METER_WIDTH * (current_oxygen_level / 100.0f);

                    // Update fill position and scale
                    oxygen_fill_transform.scale = Vec2D(new_width, METER_HEIGHT);

                    // Anchor to left side by offsetting position based on the current width
                    oxygen_fill_transform.position = {
                        oxygen_transform.position.x + 3.0f + (new_width - METER_WIDTH) / 2.0f,
                        oxygen_transform.position.y
                    };
                    oxygen_fill_transform.prev_position = oxygen_fill_transform.position;

                    // Update the fill bar texture based on oxygen level
                    if (current_oxygen_level > 50.0f) {
                        // Blue bar for 100-50%
                        oxygen_fill_graphics.texture_name = "O2_Fill_full_blue_Batch_7";
                    }
                    else if (current_oxygen_level > 20.0f) {
                        // Purple bar for 50-20%
                        oxygen_fill_graphics.texture_name = "O2_Fill_full_purple_Batch_7";
                    }
                    else {
                        // Red bar for 20-0%
                        oxygen_fill_graphics.texture_name = "O2_Fill_full_red_Batch_7";
                    }
                }

                // Position oxygen text and oxygen percentage
                if (oxygen_text_id != INVALID_ENTITY_ID &&
                    ECSM.has_component<Transform2D>(oxygen_text_id) &&
                    ECSM.has_component<Transform2D>(oxygen_percentage_text_id)) {
                    auto& oxygen_text_transform = ECSM.get_component<Transform2D>(oxygen_text_id);
                    auto& oxygen_percentage_text_transform = ECSM.get_component<Transform2D>(oxygen_percentage_text_id);
                    auto& oxygen_transform = ECSM.get_component<Transform2D>(oxygen_meter_id);

                    // Position oxygen text to the left of the oxygen meter
                    oxygen_text_transform.position = {
                        (oxygen_transform.position.x - 288.0f), // Left of meter
                        oxygen_transform.position.y             // Vertically centered with oxygen meter
                    };
                    oxygen_text_transform.prev_position = oxygen_text_transform.position;

                    // Position oxygen percentage to the middle of oxygen meter
                    oxygen_percentage_text_transform.position = {
                        oxygen_transform.position.x,
                        oxygen_text_transform.position.y
                    };
                    oxygen_percentage_text_transform.prev_position = oxygen_percentage_text_transform.position;

                    // Update oxygen text
                    if (ECSM.has_component<Text_Component>(oxygen_percentage_text_id)) {
                        auto& text = ECSM.get_component<Text_Component>(oxygen_percentage_text_id);
                        text.text = std::to_string(static_cast<int>(current_oxygen_level)) + "%";
                    }
                }

                // Position panic meter and fill (bottom meter)
                if (panic_meter_id != INVALID_ENTITY_ID &&
                    ECSM.has_component<Transform2D>(panic_meter_id) &&
                    ECSM.has_component<Transform2D>(panic_meter_fill_id)) {
                    auto& panic_transform = ECSM.get_component<Transform2D>(panic_meter_id);
                    auto& panic_fill_transform = ECSM.get_component<Transform2D>(panic_meter_fill_id);

                    // Set position and scale for panic meter
                    panic_transform.position = {
                        base_position.x - 575.0f,          // Left of UI overlay
                        base_position.y - METER_SPACING    // Below oxygen meter
                    };
                    panic_transform.scale = Vec2D(METER_WIDTH, METER_HEIGHT);
                    panic_transform.prev_position = panic_transform.position;

                    // Calculate the new width of the panic fill bar
                    float new_width = METER_WIDTH * (current_panic_level / 100.0f);

                    // Update fill position and scale
                    panic_fill_transform.scale = Vec2D(new_width, METER_HEIGHT);
                    // Anchor to left side by offsetting position based on the current width
                    panic_fill_transform.position = {
                        panic_transform.position.x + 3.0f + (new_width - METER_WIDTH) / 2.0f,  // Adjust x position to stay anchored left
                        panic_transform.position.y
                    };
                    panic_fill_transform.prev_position = panic_fill_transform.position;
                }

                // Position panic text
                if (panic_text_id != INVALID_ENTITY_ID &&
                    ECSM.has_component<Transform2D>(panic_text_id)) {
                    auto& panic_text_transform = ECSM.get_component<Transform2D>(panic_text_id);
                    auto& panic_transform = ECSM.get_component<Transform2D>(panic_meter_id);

                    // Position text to the left of the panic meter
                    panic_text_transform.position = {
                        (panic_transform.position.x - 300.0f),  // Left of meter
                        panic_transform.position.y              // Vertically centered with panic meter
                    };
                    panic_text_transform.prev_position = panic_text_transform.position;
                }

                // Position mineral texture on the right side
                if (mineral_texture_id != INVALID_ENTITY_ID &&
                    ECSM.has_component<Transform2D>(mineral_texture_id)) {
                    auto& mineral_transform = ECSM.get_component<Transform2D>(mineral_texture_id);

                    mineral_transform.position = {
                        base_position.x + 450.0f,                         // Center position
                        base_position.y - METER_SPACING / 2.0f   // Vertically centered between meters
                    };
                    mineral_transform.prev_position = mineral_transform.position;
                }

                // Position mineral count text
                if (mineral_count_text_id != INVALID_ENTITY_ID &&
                    ECSM.has_component<Transform2D>(mineral_count_text_id) &&
                    ECSM.has_component<Transform2D>(mineral_texture_id)) {
                    auto& mineral_count_text_transform = ECSM.get_component<Transform2D>(mineral_count_text_id);
                    auto& mineral_transform = ECSM.get_component<Transform2D>(mineral_texture_id);

                    // Position text to the right of the mineral texture
                    mineral_count_text_transform.position = {
                        (mineral_transform.position.x + (mineral_transform.scale.x * 2.0f) - 20.0f) ,  // Right of icon
                        mineral_transform.position.y                        // Vertically centered with mineral icon
                    };
                    mineral_count_text_transform.prev_position = mineral_count_text_transform.position;
                }

                // Position timer icon on the left side
                if (timer_icon_id != INVALID_ENTITY_ID &&
                    ECSM.has_component<Transform2D>(timer_icon_id)) {
                    auto& timer_icon_transform = ECSM.get_component<Transform2D>(timer_icon_id);

                    timer_icon_transform.position = {
                        base_position.x,                         // Center position
                        base_position.y - METER_SPACING / 1.5f   // Vertically centered between meters
                    };
                    timer_icon_transform.prev_position = timer_icon_transform.position;
                }

                // ------------------------- TIMER UPDATE CHANGES -------------------------
                // 1) Accumulate delta_time into an accumulator and decrease timer by 1 when >= 1s
                static float timer_accumulator = 0.0f; // You can make this a class member if you like
                timer_accumulator += delta_time;
                if (timer_accumulator >= 1.0f) {
                    timer_accumulator = 0.0f;

                    // Only decrease if you haven't hit zero
                    if (timer_remaining > 0) {
                        timer_remaining -= 1;
                    }
                }

                if (timer_count_text_id != INVALID_ENTITY_ID &&
                    ECSM.has_component<Transform2D>(timer_count_text_id) &&
                    ECSM.has_component<Transform2D>(timer_icon_id))
                {
                    auto& timer_count_text_transform = ECSM.get_component<Transform2D>(timer_count_text_id);
                    auto& timer_icon_transform = ECSM.get_component<Transform2D>(timer_icon_id);

                    // Position the timer text
                    timer_count_text_transform.position = {
                        timer_icon_transform.position.x + (timer_icon_transform.scale.x * 1.5f), // Right of icon
                        base_position.y - METER_SPACING / 2.0f                                   // Vertically centered
                    };
                    timer_count_text_transform.prev_position = timer_count_text_transform.position;

                    // If it has a Text_Component, update the visible text to show the integer countdown
                    if (ECSM.has_component<Text_Component>(timer_count_text_id)) {
                        auto& timer_text_comp = ECSM.get_component<Text_Component>(timer_count_text_id);
                        timer_text_comp.text = std::to_string(timer_remaining);
                    }
                }
                // ------------------------- END TIMER UPDATE CHANGES -------------------------

                // Position goal text on the right side
                if (goal_text_id != INVALID_ENTITY_ID &&
                    ECSM.has_component<Transform2D>(goal_text_id)) {
                    auto& goal_text_transform = ECSM.get_component<Transform2D>(goal_text_id);

                    goal_text_transform.position = {
                        base_position.x + 775.0f,                         // Center position
                        base_position.y - METER_SPACING / 2.0f   // Vertically centered between meters
                    };
                    goal_text_transform.prev_position = goal_text_transform.position;
                }

                
                if (goal_percentage_count_text_id != INVALID_ENTITY_ID &&
                    ECSM.has_component<Transform2D>(goal_percentage_count_text_id) &&
                    ECSM.has_component<Text_Component>(goal_percentage_count_text_id)) {

                    // Get the goal percentage transform
                    auto& goal_percentage_count_transform = ECSM.get_component<Transform2D>(goal_percentage_count_text_id);
                    auto& goal_text_transform = ECSM.get_component<Transform2D>(goal_text_id);

                    // Position text
                    goal_percentage_count_transform.position = {
                        goal_text_transform.position.x + 120.0f,  // Right of icon
                        goal_text_transform.position.y           // Vertically centered with goal text
                    };
                    goal_percentage_count_transform.prev_position = goal_percentage_count_transform.position;

                    // Update the text value based on mineral progress
                    auto& text_comp = ECSM.get_component<Text_Component>(goal_percentage_count_text_id);

                    // Find the GUI system to get the current stored mineral progress
                    for (auto& system : ECSM.get_systems()) {
                        if (auto* gui_system = dynamic_cast<GUI_System*>(system.get())) {
                            // Calculate new percentage only if interacting with hopper
                            if (gui_system->get_current_hopper_percentage() > 0.0f) {
                                // Update stored percentage
                                stored_goal_percentage = (gui_system->get_current_hopper_percentage() * 50000.0f / 50000.0f) * 100.0f;
                            }

                            // Always use the stored percentage for display
                            std::stringstream ss;
                            ss << std::setw(2) << std::setfill('0') << static_cast<int>(stored_goal_percentage) << "%";
                            text_comp.text = ss.str();
                            break;
                        }
                    }
                }
            }

            //cheat code in mining
            if (IM.is_key_pressed(GLFW_KEY_H)) {
                if (mining_strength == DEFAULT_STRENGTH) {
                    mining_strength = GOD_STRENGTH;
                }
                else {
                    mining_strength = DEFAULT_STRENGTH;
                }
                std::cout << "mining strength: " << mining_strength << std::endl;
            }

            // To check movement and mining 
            if (ECSM.has_component<Physics_Component>(player_id) && ECSM.has_component<Audio_Component>(player_id)) {

                // Get particle system
                for (auto& system : ECSM.get_systems()) {
                    if (system->get_type() == "Particle_System") {
                        auto* particle_system = static_cast<Particle_System*>(system.get());
                        if (!particle_system) {
                            LM.write_log("Game_Manager::update(): Fail to get particle system");
                            std::cerr << "Failed to get particle system" << std::endl;
                            return;
                        }

                        auto& physics = ECSM.get_component<Physics_Component>(player_id);

                        auto& audio_player = ECSM.get_component<Audio_Component>(player_id);
                        if (IM.is_key_pressed(GLFW_KEY_LEFT)) {

                            // Emit mining sparks particles
                            for (int i = 0; i < 10; ++i) {
                                // Randomize particle emit location in front of player
                                auto& player_transform = ECSM.get_component<Transform2D>(player_id);
                                float part_x = player_transform.position.x - (player_transform.scale.x / 2.0f);
                                float part_y = player_transform.position.y;
                                particle_system->particle_emit("mining", Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f));
                            }

                            if (CS.has_left_collide_detect()) {
                                EntityID block_to_remove = CS.get_left_collide_entity();
                                if (block_to_remove != INVALID_ENTITY_ID) {
                                    // Update tile health
                                    auto& animation = ECSM.get_component<Animation_Component>(block_to_remove);
                                    if (animation.curr_tile_health > 0) {
                                        if (animation.curr_tile_health <= mining_strength) {
                                            animation.curr_tile_health -= animation.curr_tile_health;
                                        }
                                        else {
                                            animation.curr_tile_health -= mining_strength;
                                        }
                                    }

                                    // Get block's position and size
                                    auto& block_transform = ECSM.get_component<Transform2D>(block_to_remove);

                                    // Emit particles, destroy the block and update mineral count when health reaches 0
                                    if (animation.curr_tile_health != 0) {
                                        // Randomize particle emit count
                                        int rand_part_cnt = 2 + static_cast<int>(std::floorf(particle_system->get_rand_float() * 3.0f));
                                        for (int i = 0; i < rand_part_cnt; ++i) {
                                            // Randomize particle emit location within the tile
                                            float part_x = block_transform.position.x - (block_transform.scale.x / 2.0f) + (particle_system->get_rand_float() * block_transform.scale.x);
                                            float part_y = block_transform.position.y - (block_transform.scale.y / 2.0f) + (particle_system->get_rand_float() * block_transform.scale.y);
                                            particle_system->particle_emit(animation.animations["0"], Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f));
                                        }
                                    }
                                    else {
                                        // Get mineral value before destroying the entity
                                        int mineral_value = get_mineral_value(block_to_remove);

                                        // Update the mineral count text
                                        if (mineral_value > 0) {
                                            update_mineral_count_text(mineral_value);
                                        }

                                        // Emit final particles after destroying tile
                                        for (int i = 0; i < 6; ++i) {
                                            // Randomize particle emit location within the tile
                                            float part_x = block_transform.position.x - (block_transform.scale.x / 2.0f) + (particle_system->get_rand_float() * block_transform.scale.x);
                                            float part_y = block_transform.position.y - (block_transform.scale.y / 2.0f) + (particle_system->get_rand_float() * block_transform.scale.y);
                                            particle_system->particle_emit(animation.animations["0"], Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f));
                                        }

                                        // Destroy the entity
                                        ECSM.destroy_entity(block_to_remove);
                                        LM.write_log("Game_Manager::update: Removed block (Entity %u) with value %d",
                                            block_to_remove, mineral_value);
                                    }

                                    // Determine sound based on mineral value
                                    std::string sound_key = (get_mineral_value(block_to_remove) > 0) ? "mining mineral" : "mining normal";
                                    ADM.play_now(player_id, sound_key, audio_player);

                                }
                            }
                        }
                        else if (IM.is_key_pressed(GLFW_KEY_RIGHT)) {

                            // Emit mining sparks particles
                            for (int i = 0; i < 10; ++i) {
                                // Randomize particle emit location in front of player
                                auto& player_transform = ECSM.get_component<Transform2D>(player_id);
                                float part_x = player_transform.position.x + (player_transform.scale.x / 2.0f);
                                float part_y = player_transform.position.y;
                                particle_system->particle_emit("mining", Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f));
                            }

                            if (CS.has_right_collide_detect()) {
                                EntityID block_to_remove = CS.get_right_collide_entity();
                                if (block_to_remove != INVALID_ENTITY_ID) {
                                    // Update tile health
                                    auto& animation = ECSM.get_component<Animation_Component>(block_to_remove);
                                    if (animation.curr_tile_health > 0) {
                                        if (animation.curr_tile_health <= mining_strength) {
                                            animation.curr_tile_health -= animation.curr_tile_health;
                                        }
                                        else {
                                            animation.curr_tile_health -= mining_strength;
                                        }
                                    }

                                    // Get block's position and size
                                    auto& block_transform = ECSM.get_component<Transform2D>(block_to_remove);

                                    // Emit particles, destroy the block and update mineral count when health reaches 0
                                    if (animation.curr_tile_health != 0) {
                                        // Randomize particle emit count
                                        int rand_part_cnt = 2 + static_cast<int>(std::floorf(particle_system->get_rand_float() * 3.0f));
                                        for (int i = 0; i < rand_part_cnt; ++i) {
                                            // Randomize particle emit location within the tile
                                            float part_x = block_transform.position.x - (block_transform.scale.x / 2.0f) + (particle_system->get_rand_float() * block_transform.scale.x);
                                            float part_y = block_transform.position.y - (block_transform.scale.y / 2.0f) + (particle_system->get_rand_float() * block_transform.scale.y);
                                            particle_system->particle_emit(animation.animations["0"], Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f));
                                        }
                                    }
                                    else {
                                        // Get mineral value before destroying the entity
                                        int mineral_value = get_mineral_value(block_to_remove);

                                        // Update the mineral count text
                                        if (mineral_value > 0) {
                                            update_mineral_count_text(mineral_value);
                                        }

                                        // Emit final particles after destroying tile
                                        for (int i = 0; i < 6; ++i) {
                                            // Randomize particle emit location within the tile
                                            float part_x = block_transform.position.x - (block_transform.scale.x / 2.0f) + (particle_system->get_rand_float() * block_transform.scale.x);
                                            float part_y = block_transform.position.y - (block_transform.scale.y / 2.0f) + (particle_system->get_rand_float() * block_transform.scale.y);
                                            particle_system->particle_emit(animation.animations["0"], Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f));
                                        }

                                        // Destroy the entity
                                        ECSM.destroy_entity(block_to_remove);
                                        LM.write_log("Game_Manager::update: Removed block (Entity %u) with value %d",
                                            block_to_remove, mineral_value);
                                    }

                                    // Determine sound based on mineral value
                                    std::string sound_key = (get_mineral_value(block_to_remove) > 0) ? "mining mineral" : "mining normal";
                                    ADM.play_now(player_id, sound_key, audio_player);
                                }
                            }
                        }
                        else if (IM.is_key_pressed(GLFW_KEY_UP)) {

                            // Emit mining sparks particles
                            for (int i = 0; i < 10; ++i) {
                                // Randomize particle emit location in front of player
                                auto& player_transform = ECSM.get_component<Transform2D>(player_id);
                                float part_x = player_transform.position.x;
                                float part_y = player_transform.position.y + (player_transform.scale.y * 0.75f);
                                particle_system->particle_emit("mining", Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f));
                            }

                            if (CS.has_top_collide_detect()) {
                                EntityID block_to_remove = CS.get_top_collide_entity();
                                if (block_to_remove != INVALID_ENTITY_ID) {
                                    // Update tile health
                                    auto& animation = ECSM.get_component<Animation_Component>(block_to_remove);
                                    if (animation.curr_tile_health > 0) {
                                        if (animation.curr_tile_health <= mining_strength) {
                                            animation.curr_tile_health -= animation.curr_tile_health;
                                        }
                                        else {
                                            animation.curr_tile_health -= mining_strength;
                                        }
                                    }

                                    // Get block's position and size
                                    auto& block_transform = ECSM.get_component<Transform2D>(block_to_remove);

                                    // Emit particles, destroy the block and update mineral count when health reaches 0
                                    if (animation.curr_tile_health != 0) {
                                        // Randomize particle emit count
                                        int rand_part_cnt = 2 + static_cast<int>(std::floorf(particle_system->get_rand_float() * 3.0f));
                                        for (int i = 0; i < rand_part_cnt; ++i) {
                                            // Randomize particle emit location within the tile
                                            float part_x = block_transform.position.x - (block_transform.scale.x / 2.0f) + (particle_system->get_rand_float() * block_transform.scale.x);
                                            float part_y = block_transform.position.y - (block_transform.scale.y / 2.0f) + (particle_system->get_rand_float() * block_transform.scale.y);
                                            particle_system->particle_emit(animation.animations["0"], Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f));
                                        }
                                    }
                                    else {
                                        // Get mineral value before destroying the entity
                                        int mineral_value = get_mineral_value(block_to_remove);

                                        // Update the mineral count text
                                        if (mineral_value > 0) {
                                            update_mineral_count_text(mineral_value);
                                        }

                                        // Emit final particles after destroying tile
                                        for (int i = 0; i < 6; ++i) {
                                            // Randomize particle emit location within the tile
                                            float part_x = block_transform.position.x - (block_transform.scale.x / 2.0f) + (particle_system->get_rand_float() * block_transform.scale.x);
                                            float part_y = block_transform.position.y - (block_transform.scale.y / 2.0f) + (particle_system->get_rand_float() * block_transform.scale.y);
                                            particle_system->particle_emit(animation.animations["0"], Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f));
                                        }

                                        // Destroy the entity
                                        ECSM.destroy_entity(block_to_remove);
                                        LM.write_log("Game_Manager::update: Removed block (Entity %u) with value %d",
                                            block_to_remove, mineral_value);
                                    }

                                    // Determine sound based on mineral value
                                    std::string sound_key = (get_mineral_value(block_to_remove) > 0) ? "mining mineral" : "mining normal";
                                    ADM.play_now(player_id, sound_key, audio_player);
                                }
                            }
                        }
                        else if (IM.is_key_pressed(GLFW_KEY_DOWN)) {

                            // Emit mining sparks particles
                            for (int i = 0; i < 10; ++i) {
                                // Randomize particle emit location in front of player
                                auto& player_transform = ECSM.get_component<Transform2D>(player_id);
                                float part_x = player_transform.position.x;
                                float part_y = player_transform.position.y - (player_transform.scale.y * 0.75f);
                                particle_system->particle_emit("mining", Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f));
                            }

                            if (CS.has_bottom_collide_detect()) {
                                EntityID block_to_remove = CS.get_bottom_collide_entity();
                                if (block_to_remove != INVALID_ENTITY_ID) {
                                    // Update tile health
                                    auto& animation = ECSM.get_component<Animation_Component>(block_to_remove);
                                    if (animation.curr_tile_health > 0) {
                                        if (animation.curr_tile_health <= mining_strength) {
                                            animation.curr_tile_health -= animation.curr_tile_health;
                                        }
                                        else {
                                            animation.curr_tile_health -= mining_strength;
                                        }
                                    }

                                    // Get block's position and size
                                    auto& block_transform = ECSM.get_component<Transform2D>(block_to_remove);

                                    // Emit particles, destroy the block and update mineral count when health reaches 0
                                    if (animation.curr_tile_health != 0) {
                                        // Randomize particle emit count
                                        int rand_part_cnt = 2 + static_cast<int>(std::floorf(particle_system->get_rand_float() * 3.0f));
                                        for (int i = 0; i < rand_part_cnt; ++i) {
                                            // Randomize particle emit location within the tile
                                            float part_x = block_transform.position.x - (block_transform.scale.x / 2.0f) + (particle_system->get_rand_float() * block_transform.scale.x);
                                            float part_y = block_transform.position.y - (block_transform.scale.y / 2.0f) + (particle_system->get_rand_float() * block_transform.scale.y);
                                            particle_system->particle_emit(animation.animations["0"], Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f));
                                        }
                                    }
                                    else {
                                        // Get mineral value before destroying the entity
                                        int mineral_value = get_mineral_value(block_to_remove);

                                        // Update the mineral count text
                                        if (mineral_value > 0) {
                                            update_mineral_count_text(mineral_value);
                                        }

                                        // Emit final particles after destroying tile
                                        for (int i = 0; i < 6; ++i) {
                                            // Randomize particle emit location within the tile
                                            float part_x = block_transform.position.x - (block_transform.scale.x / 2.0f) + (particle_system->get_rand_float() * block_transform.scale.x);
                                            float part_y = block_transform.position.y - (block_transform.scale.y / 2.0f) + (particle_system->get_rand_float() * block_transform.scale.y);
                                            particle_system->particle_emit(animation.animations["0"], Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f));
                                        }

                                        // Destroy the entity
                                        ECSM.destroy_entity(block_to_remove);
                                        LM.write_log("Game_Manager::update: Removed block (Entity %u) with value %d",
                                            block_to_remove, mineral_value);
                                    }

                                    // Determine sound based on mineral value
                                    std::string sound_key = (get_mineral_value(block_to_remove) > 0) ? "mining mineral" : "mining normal";
                                    ADM.play_now(player_id, sound_key, audio_player);
                                }
                            }
                        }

                        //cheat code to increase mineral
                        if (IM.is_key_held(GLFW_KEY_G)) {
                            int val_to_add = 500;
                            update_mineral_count_text(val_to_add);
                        }

                        //just for testing
                        /*if (IM.is_key_held(GLFW_KEY_F)) {
                            int val_to_deduct = -20;
                            if (std::stoi(ECSM.get_component<Text_Component>(mineral_count_text_id).text) >= -val_to_deduct) {
                                update_mineral_count_text(val_to_deduct);
                                ADM.play_now(player_id, "deposit mineral", audio_player);
                            }
                        }*/

                        //just for testing
                        //remember to remove the % inside the top_ui_oxygen_percentage_text entity for scene 1 & 2.
                        /*if (IM.is_key_held(GLFW_KEY_T)) {
                            int oxygen_to_add = 2;

                            if (ECSM.has_component<Text_Component>(oxygen_percentage_text_id)) {
                                auto& oxygen_text = ECSM.get_component<Text_Component>(oxygen_percentage_text_id);
                                int current_value = std::stoi(oxygen_text.text);
                                current_value += oxygen_to_add;
                                oxygen_text.text = std::to_string(current_value);
                                ADM.play_now(player_id, "refilling oxygen", audio_player);
                            }
                        }*/

                        //just for testing
                        //remember to remove the % inside the top_ui_oxygen_percentage_text entity for scene 1 & 2.
                        /*if (IM.is_key_held(GLFW_KEY_Y)) {
                            int oxygen_to_deduct = -1;
                            if (ECSM.has_component<Text_Component>(oxygen_percentage_text_id)) {
                                auto& oxygen_text = ECSM.get_component<Text_Component>(oxygen_percentage_text_id);
                                if (std::stoi(oxygen_text.text) >= -oxygen_to_deduct) {
                                    int current_value = std::stoi(oxygen_text.text);
                                    current_value += oxygen_to_deduct;
                                    oxygen_text.text = std::to_string(current_value);
                                    ADM.stop_now(player_id, "refilling oxygen", audio_player.get_filepath("refilling oxygen"));
                                }
                            }
                        }*/

                        //just for testing
                        /*if (IM.is_key_released(GLFW_KEY_F)) {
                            ADM.stop_now(player_id, "deposit mineral", audio_player.get_filepath("deposit mineral"));
                        }*/

                        //just for testing
                        //remember to remove the s inside the top_ui_timer_count_text for scene 1 & 2
                        //if (IM.is_key_held(GLFW_KEY_U)) {
                        //    int time_deduction = -1;
                        //    if (ECSM.has_component<Text_Component>(timer_count_text_id)) {
                        //        auto& oxygen_text = ECSM.get_component<Text_Component>(timer_count_text_id);
                        //        if (std::stoi(oxygen_text.text) > 0) {
                        //            int current_value = std::stoi(oxygen_text.text);
                        //            current_value += time_deduction;
                        //            oxygen_text.text = std::to_string(current_value);
                        //            //ADM.stop_now(player_id, "refilling oxygen", audio_player.get_filepath("refilling oxygen"));
                        //        }
                        //    }
                        //}

                        /*if (ECSM.has_component<Text_Component>(timer_count_text_id)) {
                            auto& timer_text = ECSM.get_component<Text_Component>(timer_count_text_id);
                            if (std::stoi(timer_text.text) <= 0) {
                                std::cout << "playing siren" << std::endl;
                                ADM.play_now(player_id, "lava siren", audio_player);
                            }
                        }*/

                        /*if (IM.is_key_held(GLFW_KEY_I)) {
                            game_over = true;

                        }*/

                        /*if (game_over) {
                            ADM.stop_now(player_id, "lava siren", audio_player.get_filepath("lava siren"));
                        }*/

                        // Get and set mining status for animation
                        if (IM.is_key_held(GLFW_KEY_LEFT)) {
                            auto& mining_status = GFXM.get_mining_status();
                            mining_status = MINE_LEFT;
                            int& direction = GFXM.get_player_direction();
                            direction = FACE_LEFT;

                        }
                        else if (IM.is_key_held(GLFW_KEY_UP)) {
                            auto& mining_status = GFXM.get_mining_status();
                            mining_status = MINE_UP;

                        }
                        else if (IM.is_key_held(GLFW_KEY_DOWN)) {
                            auto& mining_status = GFXM.get_mining_status();
                            mining_status = MINE_DOWN;

                        }
                        else if (IM.is_key_held(GLFW_KEY_RIGHT)) {
                            auto& mining_status = GFXM.get_mining_status();
                            mining_status = MINE_RIGHT;
                            int& direction = GFXM.get_player_direction();
                            direction = FACE_RIGHT;

                        }
                        else {
                            auto& mining_status = GFXM.get_mining_status();
                            mining_status = NO_ACTION;
                        }

                        // Handle horizontal movement
                        if (IM.is_key_held(GLFW_KEY_SPACE)) {
                            physics.set_jump_requested(true); //this will set the flag to true inside the physics_component 
                        }
                        else {
                            physics.set_jump_requested(false);
                        }

                        //activate and deactivate the forces. 
                        if (IM.is_key_held(GLFW_KEY_A) && !(IM.is_key_held(GLFW_KEY_D))) {
                            // Updates forces
                            physics.force_helper.deactivate_force(MOVE_RIGHT);
                            physics.force_helper.activate_force(MOVE_LEFT);
                            forces_flag = MOVE_LEFT;

                            // Update player animation flag
                            int& direction = GFXM.get_player_direction();
                            direction = FACE_LEFT;
                            int& moving_status = GFXM.get_moving_status();
                            moving_status = RUN_LEFT;

                            //std::cout << "moving left current scene number is " << current_scene << std::endl;
                        }
                        else if (IM.is_key_held(GLFW_KEY_D) && !(IM.is_key_held(GLFW_KEY_A))) {
                            // Update forces
                            physics.force_helper.deactivate_force(MOVE_LEFT);
                            physics.force_helper.activate_force(MOVE_RIGHT);
                            forces_flag = MOVE_RIGHT;

                            // Update player animation flag
                            int& direction = GFXM.get_player_direction();
                            direction = FACE_RIGHT;
                            int& moving_status = GFXM.get_moving_status();
                            moving_status = RUN_RIGHT;

                        }
                        else if (IM.is_key_held(GLFW_KEY_D) && IM.is_key_held(GLFW_KEY_A)) {
                            if (forces_flag == MOVE_LEFT) {
                                // Update forces
                                physics.force_helper.activate_force(MOVE_LEFT);
                                forces_flag = MOVE_LEFT;

                                // Update player animation flag
                                int& direction = GFXM.get_player_direction();
                                direction = FACE_LEFT;
                                int& moving_status = GFXM.get_moving_status();
                                moving_status = RUN_LEFT;
                            }
                            else {
                                // Update forces
                                physics.force_helper.deactivate_force(MOVE_LEFT);
                                physics.force_helper.activate_force(MOVE_RIGHT);
                                forces_flag = MOVE_RIGHT;

                                // Update player animation flag
                                int& direction = GFXM.get_player_direction();
                                direction = FACE_RIGHT;
                                int& moving_status = GFXM.get_moving_status();
                                moving_status = RUN_RIGHT;
                            }
                        }
                        else {
                            // Reset forces and player animation
                            physics.force_helper.deactivate_force(MOVE_LEFT);
                            physics.force_helper.deactivate_force(MOVE_RIGHT);
                            forces_flag = -1;

                            int& moving_status = GFXM.get_moving_status();
                            moving_status = NO_ACTION;

                        }

                        //audio logic is here.
                        if (forces_flag != -1) {
                            if (physics.get_is_grounded()) {
                                if (forces_flag == MOVE_RIGHT || forces_flag == MOVE_LEFT) {
                                    if (current_scene == 1) {
                                        std::string audio_key = forces_flag == MOVE_RIGHT ? "moving right" : "moving left";
                                        ADM.play_now(player_id, audio_key, audio_player);
                                    }
                                    else if (current_scene == 2) {
                                        ADM.play_now(player_id, "moving", audio_player);
                                    }

                                    // Emit walking dirt particles
                                    auto& player_transform = ECSM.get_component<Transform2D>(player_id);
                                    float part_x = player_transform.position.x - (player_transform.scale.x / 2.0f) + (particle_system->get_rand_float() * player_transform.scale.x);
                                    float part_y = player_transform.position.y - (player_transform.scale.y * 0.45f);
                                    particle_system->particle_emit("walking", Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f));
                                }
                            }
                            else {
                                //placeholder for other audio logic here for airvent and wormhole
                            }
                        }
                        else {
                            if (IM.is_key_released(GLFW_KEY_D) || IM.is_key_released(GLFW_KEY_A)) {
                                if (current_scene == 1) {
                                    ADM.stop_now(player_id, "moving right", audio_player.get_filepath("moving right"));
                                    ADM.stop_now(player_id, "moving left", audio_player.get_filepath("moving left"));
                                }
                                else if (current_scene == 2) {
                                    ADM.stop_now(player_id, "moving", audio_player.get_filepath("moving"));
                                }
                            }
                        }
                    }
                }
            }
        }
        // == End PLAYER CODE PORTION ==

#if _DEBUG
        // Change render mode with 1 (FILL), 2 (LINE), 3 (POINT) 
        if (IM.is_key_pressed(GLFW_KEY_1) && !level_editor_mode) {
            LM.write_log("Graphics_Manager::update(): '1' key pressed, render mode is now FILL.");
            GLenum& mode = GFXM.get_render_mode();
            mode = GL_FILL;
        }
        else if (IM.is_key_pressed(GLFW_KEY_2)&& !level_editor_mode) {
            LM.write_log("Graphics_Manager::update(): '2' key pressed, render mode is now LINE.");
            GLenum& mode = GFXM.get_render_mode();
            mode = GL_LINE;
        }
        else if (IM.is_key_pressed(GLFW_KEY_3)&& !level_editor_mode) {
            LM.write_log("Graphics_Manager::update(): '3' key pressed, render mode is now POINT.");
            GLenum& mode = GFXM.get_render_mode();
            mode = GL_POINT;
        }

        // Toggle debug mode using 'B" or 'N'
        if (IM.is_key_pressed(GLFW_KEY_B)) {
            LM.write_log("Graphics_Manager::update(): 'B' key pressed, Debug Mode is now ON.");
            GLboolean& mode = GFXM.get_debug_mode();
            mode = GL_TRUE;
        }
        else if (IM.is_key_pressed(GLFW_KEY_N)) {
            LM.write_log("Graphics_Manager::update(): 'N' key pressed, Debug Mode is now OFF.");
            GLboolean& mode = GFXM.get_debug_mode();
            mode = GL_FALSE;
        }
#endif

        // -------------------------imgui to scale or rotate the selected entities--------------------------------------//
#if 1
        ESS.Check_Selected_Entity();

        // Check if the left mouse button was pressed
        //EntityInfo& selectedEntityInfo = ESS.get_selected_entity_info();
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {

            if (selectedEntityInfo.isSelected) {
                select_entity = true;
                selectedID = selectedEntityInfo.selectedEntity;

            /*    std::cout << "Selected Entity ID : " << selectedEntityInfo.selectedEntity << "\n";
                std::cout << "mouse position x: " << selectedEntityInfo.mousePos.x << " ,mouse position y: " << selectedEntityInfo.mousePos.y << "\n";
                std::cout << "bool if is selected (1 is selected, 0 is not): " << selectedEntityInfo.isSelected << "\n";
                LM.write_log("Selected Entity ID system: %d", selectedEntityInfo.selectedEntity);*/

            }
            else {
                select_entity = false;
                selectedID = static_cast<EntityID>(-1);

            }
        }

        
        if (level_editor_mode && selectedID != -1 && selectedID < ECSM.get_entities().size())
        {
            // First check if entity has required components
            if (!ECSM.has_component<Transform2D>(selectedID)) {
                //std::cout << "Selected entity " << selectedID << " has no Transform2D component\n";
                return;
            }

            auto& transform = ECSM.get_component<Transform2D>(selectedID);
            GLfloat rot_change = transform.orientation.y * static_cast<GLfloat>(delta_time);
            GLfloat scale_change = DEFAULT_SCALE_CHANGE * static_cast<GLfloat>(delta_time);

            // Check if entity has collision component before using it
            bool has_collision = ECSM.has_component<Collision_Component>(selectedID);
            Collision_Component* collision = nullptr;
            if (has_collision) {
                collision = &ECSM.get_component<Collision_Component>(selectedID);
            }

            if (IM.is_key_held(GLFW_KEY_UP) && !(IM.is_key_held(GLFW_KEY_DOWN)))
            {
                std::cout << selectedID << " scaling up in level editor\n";
                transform.scale.x += scale_change;
                transform.scale.y += scale_change;

                if (collision) {
                    collision->width += scale_change;
                    collision->height += scale_change;
                }
            }
            else if (IM.is_key_held(GLFW_KEY_DOWN) && !(IM.is_key_held(GLFW_KEY_UP)))
            {

                if (transform.scale.x > 0.0f) {
                    transform.scale.x -= scale_change;
                    if (collision) {
                        collision->width -= scale_change;
                    }
                }
                else {
                    transform.scale.x = 0.0f;
                    if (collision) {
                        collision->width = 0.0f;
                    }
                }

                if (transform.scale.y > 0.0f) {
                    transform.scale.y -= scale_change;
                    if (collision) {
                        collision->height -= scale_change;
                    }
                }
                else {
                    transform.scale.y = 0.0f;
                    if (collision) {
                        collision->height = 0.0f;
                    }
                }
            }
            else if (IM.is_key_held(GLFW_KEY_LEFT) && !(IM.is_key_held(GLFW_KEY_RIGHT)))
            {
                transform.orientation.x += rot_change;
            }
            else if (IM.is_key_held(GLFW_KEY_RIGHT) && !(IM.is_key_held(GLFW_KEY_LEFT)))
            {
                transform.orientation.x -= rot_change;
            }
        }
        // -------------------------imgui to scale or rotate the selected entities--------------------------------------//
#endif
        if (IM.is_key_pressed(GLFW_KEY_TAB)) {
            auto& camera = GFXM.get_camera();
            if (camera.is_free_cam == GL_FALSE) {
                camera.is_free_cam = GL_TRUE;
            }
            else {
                camera.is_free_cam = GL_FALSE;
            }

            int& editor_mode = GFXM.get_editor_mode();
            if (editor_mode == 1) {
                editor_mode = 0;
            }
            else {
                editor_mode = 1;
            }
        }

        // Camera up-down scrolling when I or K pressed
        if (IM.is_key_held(GLFW_KEY_I) && !(IM.is_key_held(GLFW_KEY_K))) {
            camera_up_down_scroll_flag = GLFW_KEY_I;
            auto& camera = GFXM.get_camera();
            if (camera.is_free_cam == GL_TRUE) {
                camera.pos_y += (DEFAULT_CAMERA_SPEED * static_cast<GLfloat>(delta_time));
                imgui_camera_pos_y = camera.pos_y;
                LM.write_log("Render_System::update(): 'Keypad 8' key held, camera position is now %f.", camera.pos_y);
            }
        }
        else if (IM.is_key_held(GLFW_KEY_K) && !(IM.is_key_held(GLFW_KEY_I))) {
            camera_up_down_scroll_flag = GLFW_KEY_K;
            auto& camera = GFXM.get_camera();
            if (camera.is_free_cam == GL_TRUE) {
                camera.pos_y -= (DEFAULT_CAMERA_SPEED * static_cast<GLfloat>(delta_time));
                imgui_camera_pos_y = camera.pos_y;
                LM.write_log("Render_System::update(): 'Keypad 2' key held, camera position is now %f.", camera.pos_y);
            }
        }
        else if (IM.is_key_held(GLFW_KEY_I) && IM.is_key_held(GLFW_KEY_K)) {
            auto& camera = GFXM.get_camera();
            if (camera_up_down_scroll_flag == GLFW_KEY_I) {
                camera.pos_y += (DEFAULT_CAMERA_SPEED * static_cast<GLfloat>(delta_time));
                imgui_camera_pos_y = camera.pos_y;
                LM.write_log("Render_System::update(): 'Keypad 8' key held, camera position is now %f.", camera.pos_y);
            }
            else {
                camera.pos_y -= (DEFAULT_CAMERA_SPEED * static_cast<GLfloat>(delta_time));
                imgui_camera_pos_y = camera.pos_y;
                LM.write_log("Render_System::update(): 'Keypad 2' key held, camera position is now %f.", camera.pos_y);
            }
        }
        else {
            camera_up_down_scroll_flag = 0;
        }

        // Camera left-right scrolling when J or L pressed
        if (IM.is_key_held(GLFW_KEY_J) && !(IM.is_key_held(GLFW_KEY_L))) {
            camera_left_right_scroll_flag = GLFW_KEY_J;
            auto& camera = GFXM.get_camera();
            if (camera.is_free_cam == GL_TRUE) {
                camera.pos_x -= (DEFAULT_CAMERA_SPEED * static_cast<GLfloat>(delta_time));
                imgui_camara_pos_x = camera.pos_x;
                LM.write_log("Render_System::update(): 'Keypad 8' key held, camera position is now %f.", camera.pos_y);
            }
        }
        else if (IM.is_key_held(GLFW_KEY_L) && !(IM.is_key_held(GLFW_KEY_J))) {
            camera_left_right_scroll_flag = GLFW_KEY_L;
            auto& camera = GFXM.get_camera();
            if (camera.is_free_cam == GL_TRUE) {
                camera.pos_x += (DEFAULT_CAMERA_SPEED * static_cast<GLfloat>(delta_time));
                imgui_camara_pos_x = camera.pos_x;
                LM.write_log("Render_System::update(): 'Keypad 2' key held, camera position is now %f.", camera.pos_y);
            }
        }
        else if (IM.is_key_held(GLFW_KEY_J) && IM.is_key_held(GLFW_KEY_L)) {
            auto& camera = GFXM.get_camera();
            if (camera_left_right_scroll_flag == GLFW_KEY_J) {
                camera.pos_x -= (DEFAULT_CAMERA_SPEED * static_cast<GLfloat>(delta_time));
                imgui_camara_pos_x = camera.pos_x;
                LM.write_log("Render_System::update(): 'Keypad 8' key held, camera position is now %f.", camera.pos_y);
            }
            else {
                camera.pos_x += (DEFAULT_CAMERA_SPEED * static_cast<GLfloat>(delta_time));
                imgui_camara_pos_x = camera.pos_x;
                LM.write_log("Render_System::update(): 'Keypad 2' key held, camera position is now %f.", camera.pos_y);
            }
        }
        else {
            camera_left_right_scroll_flag = 0;
        }

        if (IM.is_key_pressed(GLFW_KEY_0) && !level_editor_mode) {
            LM.write_log("Game_Manager::update(): Toggling between scenes");

            // Cycle through scenes: main_menu -> scene1 -> scene2 -> back to main_menu
            if (current_scene == 0) {
                current_scene = 1; // Switch to scene1
            }
            else if (current_scene == 1) {
                current_scene = 2; // Switch to scene2
            }
            else {
                current_scene = 0; // Switch back to main_menu
            }

            for (auto& system : ECSM.get_systems()) {
                if (auto* movement_system = dynamic_cast<Movement_System*>(system.get())) {
                    movement_system->clear_dynamic_entities();
                    break;
                }
            }

            // Define scene file names
            const std::string SCENES = "Scenes";
            std::string scene_file;

            if (current_scene == 0) {
                scene_file = "main_menu.scn";
            }
            else {
                scene_file = "scene" + std::to_string(current_scene) + ".scn";
            }

            // Create full path to the scene file
            std::string scene_path = ASM.get_full_path(SCENES, scene_file);

            // Try to load the new scene
            if (SM.load_scene(scene_path.c_str())) {
                LM.write_log("Game_Manager::update(): Successfully loaded %s", scene_file.c_str());

                // Reset camera position only if not in main menu
                auto& camera = GFXM.get_camera();
                if (current_scene != 0) {
                    camera.pos_x = DEFAULT_CAMERA_POS_X;
                    camera.pos_y = DEFAULT_CAMERA_POS_Y;
                }

                // Stop all audio currently playing
                ADM.stop_mastergroup();

                // Reset player position only if in scene1 or scene2
                if (current_scene != 0) {
                    EntityID playerId = ECSM.find_entity_by_name(DEFAULT_PLAYER_NAME);
                    if (playerId != INVALID_ENTITY_ID) {
                        if (ECSM.has_component<Transform2D>(playerId)) {
                            auto& transform = ECSM.get_component<Transform2D>(playerId);
                            transform.position = Vec2D(0.0f, 0.0f);
                            transform.prev_position = transform.position;
                        }
                        if (ECSM.has_component<Velocity_Component>(playerId)) {
                            auto& velocity = ECSM.get_component<Velocity_Component>(playerId);
                            velocity.velocity = Vec2D(0.0f, 0.0f);
                        }
                    }
                }
            }
            else {
                LM.write_log("Game_Manager::update(): Failed to load %s", scene_file.c_str());

                // Revert scene number since load failed
                if (current_scene == 0) {
                    current_scene = 2;
                }
                else if (current_scene == 1) {
                    current_scene = 0;
                }
                else {
                    current_scene = 1;
                }
            }

            IMGUIM.set_current_file_shown(scene_file);
        }


        // Getting delta time for Input Manager
        //IM.set_time(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
        auto start_time = std::chrono::steady_clock::now();

        // Update Input_Manager
        IM.update();

        auto end_time = std::chrono::steady_clock::now();
        IM.set_time(std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count());

        //No Graphics Manager Update
        //// Getting delta time for Graphics Manager
        //GFXM.set_time(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
        //GFXM.set_time(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() - GFXM.get_time());

        // Getting delta time for ECS Manager
        //ECSM.set_time(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
        start_time = std::chrono::steady_clock::now();

        // Update game world state
        ECSM.update(delta_time);
        end_time = std::chrono::steady_clock::now();
        ECSM.set_time(std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count());

        m_step_count++;
    }

    void Game_Manager::set_game_over(bool new_game_over) {
        m_game_over = new_game_over;
        LM.write_log("Game_Manager::set_game_over(): game_over set to %s", new_game_over ? "true" : "false");
        std::cout << "Game_Manager::set_game_over(): game_over set to " << (new_game_over ? "true" : "false") << std::endl;
    }

    bool Game_Manager::get_game_over() const {
        return m_game_over;
    }

    int Game_Manager::get_step_count() const {
        return m_step_count;
    }

    int Game_Manager::get_mineral_value(EntityID block_id) const {
        if (!ECSM.has_component<Animation_Component>(block_id)) {
            return 0;
        }

        auto* entity = ECSM.get_entity(block_id);
        if (!entity) {
            return 0;
        }

        const std::string& name = entity->get_name();
        LM.write_log("Checking mineral value for entity with name: %s", name.c_str());

        // Match the prefab names with their corresponding values
        if (name.find("quartz") != std::string::npos) {
            LM.write_log("Found quartz mineral, value: 100");
            return 100;
        }
        if (name.find("emerald") != std::string::npos) {
            LM.write_log("Found emerald mineral, value: 800");
            return 800;
        }
        if (name.find("sapphire") != std::string::npos) {
            LM.write_log("Found sapphire mineral, value: 1600");
            return 1600;
        }
        if (name.find("amethyst") != std::string::npos) {
            LM.write_log("Found amethyst mineral, value: 2400");
            return 2400;
        }
        if (name.find("citrine") != std::string::npos) {
            LM.write_log("Found citrine mineral, value: 3200");
            return 3200;
        }
        if (name.find("alexandrite") != std::string::npos) {
            LM.write_log("Found alexandrite mineral, value: 4000");
            return 4000;
        }

        LM.write_log("No mineral value found for this entity");
        return 0;
    }

    void Game_Manager::update_mineral_count_text(int value_to_add) {
        EntityID text_entity = ECSM.find_entity_by_name("top_ui_mineral_count_text");
        if (text_entity == INVALID_ENTITY_ID) {
            LM.write_log("Could not find mineral count text entity");
            return;
        }

        if (!ECSM.has_component<Text_Component>(text_entity)) {
            LM.write_log("Mineral count entity does not have Text_Component");
            return;
        }

        try {
            auto& text_comp = ECSM.get_component<Text_Component>(text_entity);
            // Convert current text to integer, add new value
            int current_value = std::stoi(text_comp.text);
            current_value += value_to_add;

            // Format the number with leading zeros (6 digits)
            std::stringstream ss;
            ss << std::setw(6) << std::setfill('0') << current_value;
            text_comp.text = ss.str();

            LM.write_log("Updated mineral count to: %06d", current_value);
        }
        catch (const std::exception& e) {
            LM.write_log("Error updating mineral count: %s", e.what());
        }
    }

    void Game_Manager::set_current_scene(int scene_num) {
        current_scene = scene_num;
    }

    int Game_Manager::get_current_scene() {
        return current_scene;
    }

    bool Game_Manager::check_non_mineral(EntityID block_id, std::string block_name) const {
        if (!ECSM.has_component<Animation_Component>(block_id)) {
            return false;
        }

        auto* entity = ECSM.get_entity(block_id);
        if (!entity) {
            return false;
        }

        const std::string& name = entity->get_name();
        LM.write_log("Checking mineral value for entity with name: %s", name.c_str());

        // Match the prefab names with their corresponding values
        if (name.find(block_name) != std::string::npos) {
            LM.write_log("Found quartz mineral, value: 100");
            return true;
        }

        LM.write_log("Block isn't %s but %s", block_name.c_str(), name.c_str());
        return false;
    }

} // namespace lof
