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

// Include utility
#include "../Utility/Constant.h"
#include "../Utility/Globals.h"

// Include systems
#include "../System/Audio_System.h" // Add this for Audio System access
#include "../System/GUI_System.h"  // Add this for GUI system access
#include "../System/Animation_System.h"  // For player_direction
#include "../System/Collision_System.h" // for click entity object

#include "../Utility/Entity_Selector_Helper.h"

// Include iostream for console output
#include <iostream>
#include <random>
#include <chrono>

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

        // -------------------------- Graphics Manager Start Up --------------------------
        if (GFXM.start_up() != 0) {
            LM.write_log("Game_Manager::start_up(): Graphics_Manager start_up() failed");
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
            std::cout << "Escape key pressed. Closing the game." << std::endl;
        }

        //to pause all the sound that is playing
        if (IM.is_key_pressed(GLFW_KEY_5) && !level_editor_mode) {
            for (auto& system : ECSM.get_systems()) {
                if (system->get_type() == "Audio_System") {
                    auto* audio_system = static_cast<Audio_System*>(system.get());
                    if (audio_system) {
                        audio_system->pause_resume_mastergroup();
                    }
                }
            }
        }

        //to ensure sound pause during level_editor_mode
        if (IM.is_key_pressed(GLFW_KEY_TAB)) {
            for (auto& system : ECSM.get_systems()) {
                if (system->get_type() == "Audio_System") {
                    auto* audio_system = static_cast<Audio_System*>(system.get());
                    if (audio_system) {
                        audio_system->pause_resume_mastergroup();
                    }
                }
            }
        }

        // Handle player movement and physics input
        EntityID player_id = ECSM.find_entity_by_name(DEFAULT_PLAYER_NAME);

        if (player_id != INVALID_ENTITY_ID && !level_editor_mode) {  // If player entity exists

            // Update top UI overlay position to follow player
            EntityID ui_overlay_id = ECSM.find_entity_by_name("top_ui_overlay");
            EntityID oxygen_meter_id = ECSM.find_entity_by_name("top_ui_oxygen_meter");
            EntityID panic_meter_id = ECSM.find_entity_by_name("top_ui_panik_meter");
            EntityID mineral_texture_id = ECSM.find_entity_by_name("top_ui_mineral_texture");

            EntityID oxygen_text_id = ECSM.find_entity_by_name("top_ui_oxygen_text");
            EntityID panic_text_id = ECSM.find_entity_by_name("top_ui_panic_text");
            EntityID mineral_count_text_id = ECSM.find_entity_by_name("top_ui_mineral_count_text");

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

                // Position oxygen meter (top meter)
                if (oxygen_meter_id != INVALID_ENTITY_ID &&
                    ECSM.has_component<Transform2D>(oxygen_meter_id)) {
                    auto& oxygen_transform = ECSM.get_component<Transform2D>(oxygen_meter_id);

                    // Set position and scale for oxygen meter
                    oxygen_transform.position = {
                        base_position.x - METER_WIDTH,  // Left of UI overlay
                        base_position.y                 // Top position
                    };
                    oxygen_transform.scale = Vec2D(METER_WIDTH, METER_HEIGHT);
                    oxygen_transform.prev_position = oxygen_transform.position;
                }

                // Position oxygen text
                if (oxygen_text_id != INVALID_ENTITY_ID && 
                    ECSM.has_component<Transform2D>(oxygen_text_id)) { 
                    auto& oxygen_text_transform = ECSM.get_component<Transform2D>(oxygen_text_id); 
                    auto& oxygen_transform = ECSM.get_component<Transform2D>(oxygen_meter_id);

                    // Position text to the left of the oxygen meter
                    oxygen_text_transform.position = {
                        (oxygen_transform.position.x - (oxygen_transform.scale.x / 2.0f) - (oxygen_text_transform.scale.x / 2.0f)), // Left of meter
                        oxygen_transform.position.y                                                         // Vertically centered with oxygen meter
                    };
                    oxygen_text_transform.prev_position = oxygen_text_transform.position;
                }

                // Position panic meter (bottom meter)
                if (panic_meter_id != INVALID_ENTITY_ID &&
                    ECSM.has_component<Transform2D>(panic_meter_id)) {
                    auto& panic_transform = ECSM.get_component<Transform2D>(panic_meter_id);

                    // Set position and scale for panic meter
                    panic_transform.position = {
                        base_position.x - METER_WIDTH,          // Left of UI overlay
                        base_position.y - METER_SPACING         // Below oxygen meter
                    };
                    panic_transform.scale = Vec2D(METER_WIDTH, METER_HEIGHT);
                    panic_transform.prev_position = panic_transform.position;
                }

                // Position panic text
                if (panic_text_id != INVALID_ENTITY_ID &&
                    ECSM.has_component<Transform2D>(panic_text_id)) {
                    auto& panic_text_transform = ECSM.get_component<Transform2D>(panic_text_id);
                    auto& panic_transform = ECSM.get_component<Transform2D>(panic_meter_id); 

                    // Position text to the left of the panic meter
                    panic_text_transform.position = {
                        (panic_transform.position.x - (panic_transform.scale.x / 2.0f) - (panic_text_transform.scale.x / 2.0f)),  // Left of meter
                        panic_transform.position.y                                                         // Vertically centered with panic meter
                    };
                    panic_text_transform.prev_position = panic_text_transform.position;
                }

                // Position mineral texture on the right side
                if (mineral_texture_id != INVALID_ENTITY_ID &&
                    ECSM.has_component<Transform2D>(mineral_texture_id)) {
                    auto& mineral_transform = ECSM.get_component<Transform2D>(mineral_texture_id);

                    mineral_transform.position = {
                        base_position.x,                         // Center position
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
                        (mineral_transform.position.x + (mineral_transform.scale.x * 3.0f)) ,  // Right of icon
                        mineral_transform.position.y  - TEXT_OFFSET_Y                          // Vertically centered with mineral icon
                    };
                    mineral_count_text_transform.prev_position = mineral_count_text_transform.position;
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

            if (ECSM.has_component<Physics_Component>(player_id)) {

                auto& physics = ECSM.get_component<Physics_Component>(player_id);

                if (IM.is_key_pressed(GLFW_KEY_LEFT)) {
                    if (CS.has_left_collide_detect()) {
                        EntityID block_to_remove = CS.get_left_collide_entity();
                        if (block_to_remove != INVALID_ENTITY_ID) {
                            // Update tile health
                            auto& animation = ECSM.get_component<Animation_Component>(block_to_remove);
                            if (animation.curr_tile_health > 0) {
                                //animation.curr_tile_health--;
                                if (animation.curr_tile_health <= mining_strength) {
                                    animation.curr_tile_health -= animation.curr_tile_health;
                                }
                                else {
                                    animation.curr_tile_health -= mining_strength;
                                }
                            }

                            // Destroy the block and update mineral count when health reaches 0
                            if (animation.curr_tile_health == 0) {
                                // Get mineral value before destroying the entity
                                int mineral_value = get_mineral_value(block_to_remove);

                                // Update the mineral count text
                                if (mineral_value > 0) {
                                    update_mineral_count_text(mineral_value);
                                }

                                // Destroy the entity
                                ECSM.destroy_entity(block_to_remove);
                                LM.write_log("Game_Manager::update: Removed block (Entity %u) with value %d",
                                    block_to_remove, mineral_value);
                            }
                        }
                    }
                }
                else if (IM.is_key_pressed(GLFW_KEY_RIGHT)) {
                    if (CS.has_right_collide_detect()) {
                        EntityID block_to_remove = CS.get_right_collide_entity();
                        if (block_to_remove != INVALID_ENTITY_ID) {
                            // Update tile health
                            auto& animation = ECSM.get_component<Animation_Component>(block_to_remove);
                            if (animation.curr_tile_health > 0) {
                                //animation.curr_tile_health--;
                                if (animation.curr_tile_health <= mining_strength) {
                                    animation.curr_tile_health -= animation.curr_tile_health;
                                }
                                else {
                                    animation.curr_tile_health -= mining_strength;
                                }
                            }

                            // Destroy the block and update mineral count when health reaches 0
                            if (animation.curr_tile_health == 0) {
                                // Get mineral value before destroying the entity
                                int mineral_value = get_mineral_value(block_to_remove);

                                // Update the mineral count text
                                if (mineral_value > 0) {
                                    update_mineral_count_text(mineral_value);
                                }

                                // Destroy the entity
                                ECSM.destroy_entity(block_to_remove);
                                LM.write_log("Game_Manager::update: Removed block (Entity %u) with value %d",
                                    block_to_remove, mineral_value);
                            }
                        }
                    }
                }
                else if (IM.is_key_pressed(GLFW_KEY_UP)) {
                    if (CS.has_top_collide_detect()) {
                        EntityID block_to_remove = CS.get_top_collide_entity();
                        if (block_to_remove != INVALID_ENTITY_ID) {
                            // Update tile health
                            auto& animation = ECSM.get_component<Animation_Component>(block_to_remove);
                            if (animation.curr_tile_health > 0) {
                                //animation.curr_tile_health--;
                                if (animation.curr_tile_health <= mining_strength) {
                                    animation.curr_tile_health -= animation.curr_tile_health;
                                }
                                else {
                                    animation.curr_tile_health -= mining_strength;
                                }
                            }

                            // Destroy the block and update mineral count when health reaches 0
                            if (animation.curr_tile_health == 0) {
                                // Get mineral value before destroying the entity
                                int mineral_value = get_mineral_value(block_to_remove);

                                // Update the mineral count text
                                if (mineral_value > 0) {
                                    update_mineral_count_text(mineral_value);
                                }

                                // Destroy the entity
                                ECSM.destroy_entity(block_to_remove);
                                LM.write_log("Game_Manager::update: Removed block (Entity %u) with value %d",
                                    block_to_remove, mineral_value);
                            }
                        }
                    }
                }
                else if (IM.is_key_pressed(GLFW_KEY_DOWN)) {
                    if (CS.has_bottom_collide_detect()) {
                        EntityID block_to_remove = CS.get_bottom_collide_entity();
                        if (block_to_remove != INVALID_ENTITY_ID) {
                            // Update tile health
                            auto& animation = ECSM.get_component<Animation_Component>(block_to_remove);
                            if (animation.curr_tile_health > 0) {
                                //animation.curr_tile_health--;
                                if (animation.curr_tile_health <= mining_strength) {
                                    animation.curr_tile_health -= animation.curr_tile_health;
                                }
                                else {
                                    animation.curr_tile_health -= mining_strength;
                                }
                            }

                            // Destroy the block and update mineral count when health reaches 0
                            if (animation.curr_tile_health == 0) {
                                // Get mineral value before destroying the entity
                                int mineral_value = get_mineral_value(block_to_remove);

                                // Update the mineral count text
                                if (mineral_value > 0) {
                                    update_mineral_count_text(mineral_value);
                                }

                                // Destroy the entity
                                ECSM.destroy_entity(block_to_remove);
                                LM.write_log("Game_Manager::update: Removed block (Entity %u) with value %d",
                                    block_to_remove, mineral_value);
                            }
                        }
                    }
                }

                //cheat code to increase mineral
                if (IM.is_key_held(GLFW_KEY_G)) {
                    int val_to_add = 500;
                    update_mineral_count_text(val_to_add);
                }

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

                    // Update sound effect for player moving left
                    if (physics.get_is_grounded()) {
                        if (current_scene == 1) {
                            ECSM.get_component<Audio_Component>(player_id).set_audio_state("moving left", PLAYING);
                        } else if (current_scene == 2) {
                            // Generate a random number between 1 and 3
                            int randomNumber = std::rand() % 3 + 1; // rand() % 3 gives 0, 1, or 2, so we add 1 to get 1, 2, or 3

                            // Create the file path by appending the random number to "Walking_0"
                            std::string key = "moving " + std::to_string(randomNumber);
                            LM.write_log("TESTING MOVEMENT SCENE 2 Walking Audio: %s", key.c_str());
                            //play walking sound
                            ECSM.get_component<Audio_Component>(player_id).set_audio_state(key.c_str(), PLAYING);
                        }
                    }
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

                    // Update sound effect for player moving right
                    if (physics.get_is_grounded()) {
                        if (current_scene == 1) {
                            ECSM.get_component<Audio_Component>(player_id).set_audio_state("moving right", PLAYING);
                        } else if (current_scene == 2) {
                            // Generate a random number between 1 and 3
                            int randomNumber = std::rand() % 3 + 1; // rand() % 3 gives 0, 1, or 2, so we add 1 to get 1, 2, or 3

                            // Create the file path by appending the random number to "Walking_0"
                            std::string key = "moving " + std::to_string(randomNumber);
                            LM.write_log("TESTING MOVEMENT SCENE 2 Walking Audio: %s", key.c_str());
                            //play walking sound
                            ECSM.get_component<Audio_Component>(player_id).set_audio_state(key.c_str(), PLAYING);
                        }
                    }
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

                        // Update sound effect for player moving left
                        if (physics.get_is_grounded()) {
                            if (current_scene == 1) {
                                ECSM.get_component<Audio_Component>(player_id).set_audio_state("moving left", PLAYING);
                            }
                            else if (current_scene == 2) {
                                // Generate a random number between 1 and 3
                                int randomNumber = std::rand() % 3 + 1; // rand() % 3 gives 0, 1, or 2, so we add 1 to get 1, 2, or 3

                                // Create the file path by appending the random number to "Walking_0"
                                std::string key = "moving " + std::to_string(randomNumber);
                                LM.write_log("TESTING MOVEMENT SCENE 2 Walking Audio: %s", key.c_str());
                                //play walking sound
                                ECSM.get_component<Audio_Component>(player_id).set_audio_state(key, PLAYING);
                            }
                        }
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

                        // Update sound effect for player moving right
                        if (physics.get_is_grounded()) {
                            if (current_scene == 1) {
                                ECSM.get_component<Audio_Component>(player_id).set_audio_state("moving right", PLAYING);
                            }
                            else if (current_scene == 2) {
                                // Generate a random number between 1 and 3
                                int randomNumber = std::rand() % 3 + 1; // rand() % 3 gives 0, 1, or 2, so we add 1 to get 1, 2, or 3

                                // Create the file path by appending the random number to "Walking_0"
                                std::string key = "moving " + std::to_string(randomNumber);
                                LM.write_log("TESTING MOVEMENT SCENE 2 Walking Audio: %s", key.c_str());
                                //play walking sound
                                ECSM.get_component<Audio_Component>(player_id).set_audio_state(key, PLAYING);
                            }
                        }
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

            }
        }


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
                std::cout << "Selected entity " << selectedID << " has no Transform2D component\n";
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

            // Toggle between scenes
            current_scene = (current_scene == 1) ? 2 : 1;

            // Create full path to the scene file
            const std::string SCENES = "Scenes";
            std::string scene_path = ASM.get_full_path(SCENES, "scene" + std::to_string(current_scene) + ".scn");

            // Try to load the new scene
            if (SM.load_scene(scene_path.c_str())) {
                LM.write_log("Game_Manager::update(): Successfully loaded scene%d: %s", current_scene, scene_path.c_str());

                // Reset camera position
                auto& camera = GFXM.get_camera();
                camera.pos_x = DEFAULT_CAMERA_POS_X;
                camera.pos_y = DEFAULT_CAMERA_POS_Y;

                // Reset player position if exists
                EntityID playerId = ECSM.find_entity_by_name(DEFAULT_PLAYER_NAME);
                if (playerId != INVALID_ENTITY_ID) {
                    if (ECSM.has_component<Transform2D>(playerId)) {
                        auto& transform = ECSM.get_component<Transform2D>(playerId);
                        transform.position = Vec2D(0.0f, 0.0f);
                        transform.prev_position = transform.position;
                    }
                    if (ECSM.has_component<Velocity_Component>(player_id)) {
                        auto& velocity = ECSM.get_component<Velocity_Component>(playerId);
                        velocity.velocity = Vec2D(0.0f, 0.0f);
                    }
                }

               
            }
            else {
                LM.write_log("Game_Manager::update(): Failed to load scene%d: %s", current_scene, scene_path.c_str());
                // Revert the scene number since load failed
                current_scene = (current_scene == 1) ? 2 : 1;
            }

            std::string get_file_name = "scene" + std::to_string(current_scene) + ".scn";
            IMGUIM.set_current_file_shown(get_file_name);
        }

        // Getting delta time for Input Manager
        IM.set_time(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
        // Update Input_Manager
        IM.update();
        IM.set_time(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() - IM.get_time());

        // Getting delta time for Graphics Manager
        GFXM.set_time(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
        GFXM.set_time(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() - GFXM.get_time());

        // Getting delta time for ECS Manager
        ECSM.set_time(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
        // Update game world state
        ECSM.update(delta_time);
        ECSM.set_time(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() - ECSM.get_time());

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
            // Convert back to string and update text
            text_comp.text = std::to_string(current_value);
            LM.write_log("Updated mineral count to: %d", current_value);
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

} // namespace lof
