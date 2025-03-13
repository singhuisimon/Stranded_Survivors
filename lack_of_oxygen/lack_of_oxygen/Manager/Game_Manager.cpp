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
#include "Logic_Manager.h"
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

    static bool prev_pasued = false;
    float imgui_camara_pos_x = 0.0f;
    float imgui_camera_pos_y = 0.0f;
    unsigned int mining_strength = DEFAULT_STRENGTH;
    Game_Manager::Game_Manager()
        : m_game_over(false), m_step_count(0), m_is_paused(false) {
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
        // -------------------------- Logic Manager Start Up --------------------------
        if (LGM.start_up() != 0) {
            LM.write_log("Game_Manager::start_up(): Logic_Manager start_up() failed");
            IM.shut_down();
            FPSM.shut_down();
            SM.shut_down();
            ECSM.shut_down();
            LM.shut_down();
            return -6;
        }
        else {
            LM.write_log("Game_Manager::start_up(): Logic_Manager start_up() successful");
        }
        // ---------------------------- Audio Manager Start Up ---------------------------
        if (ADM.start_up() != 0) {
            LM.write_log("Game_Manager::start_up(): Audio_Manager start_up() failed");
            LGM.shut_down();
            IM.shut_down();
            FPSM.shut_down();
            SM.shut_down();
            ECSM.shut_down();
            LM.shut_down();
            return -7;
        }
        // -------------------------- Graphics Manager Start Up --------------------------
        if (GFXM.start_up() != 0) {
            LM.write_log("Game_Manager::start_up(): Graphics_Manager start_up() failed");
            ADM.shut_down();
            LGM.shut_down();
            IM.shut_down();
            FPSM.shut_down();
            SM.shut_down();
            ECSM.shut_down();
            LM.shut_down();
            return -8;
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
        LGM.shut_down();  // Logic_Manager
        IM.shut_down();   // Input_Manager
        FPSM.shut_down(); // FPS_Manager
        SM.shut_down();   // Serialization_Manager
        ECSM.shut_down(); // ECS_Manager
        LM.shut_down();   // Log_Manager
        //ASM.shut_down();  //Assets_Manager

        m_is_started = false;
        std::cout << "Game_Manager shut down successfully." << std::endl;
    }

    EntityInfo& selectedEntityInfo = ESS.get_selected_entity_info(); // for imgui
    EntityID selectedID = INVALID_ENTITY_ID; // for imgui

    void Game_Manager::set_paused(bool paused) {
        if (paused == m_is_paused) return; // No change needed

        m_is_paused = paused;

        // Find GUI System to show/hide pause menu
        for (auto& system : ECSM.get_systems()) {
            if (auto* gui_system = dynamic_cast<GUI_System*>(system.get())) {
                if (m_is_paused) {
                    gui_system->show_pause_menu();

                    // Pause audio
                    ADM.pause_resume_mastergroup();
                }
                else {
                    gui_system->hide_pause_menu();

                    // Resume audio
                    ADM.pause_resume_mastergroup();
                }
                break;
            }
        }

        LM.write_log("Game_Manager::set_paused(): Game %s", m_is_paused ? "paused" : "resumed");
    }


    void Game_Manager::toggle_pause() {
        set_paused(!m_is_paused);
    }


    //EntityID selectedID = static_cast<EntityID>(-1); // for imgui
    void Game_Manager::update(float delta_time) {

        // Check if the game manager is started
        if (!is_started()) {
            LM.write_log("Game_Manager::update(): Game_Manager not started");
            return;
        }

        // Set display fps flag to true or false when key 'F' is pressed
        if (IM.is_key_pressed(GLFW_KEY_F) && display_fps == false) {
            display_fps = true;
        }
        else if (IM.is_key_pressed(GLFW_KEY_F) && display_fps == true) {
            display_fps = false;
        }

        // Display fps only when fps flag is true for menu and other non-game scenes
        if (current_scene != 2 && display_fps == true) {
            EntityID fps_counter_id = ECSM.find_entity_by_name("fps_counter");
            if (fps_counter_id != INVALID_ENTITY_ID && ECSM.has_component<Text_Component>(fps_counter_id)) {
                auto& text_comp = ECSM.get_component<Text_Component>(fps_counter_id);
                float current_fps = FPSM.get_current_fps();
                std::stringstream ss;
                ss << "FPS: " << std::fixed << std::setprecision(1) << current_fps;
                text_comp.text = ss.str();
            }
        }
        else {
            EntityID fps_counter_id = ECSM.find_entity_by_name("fps_counter");
            if (fps_counter_id != INVALID_ENTITY_ID && ECSM.has_component<Text_Component>(fps_counter_id)) {
                auto& text_comp = ECSM.get_component<Text_Component>(fps_counter_id);
                text_comp.text = "";
            }
        }

        // 1) Obtain a pointer to the GUI_System
        GUI_System* gui_system = nullptr;
        for (auto& sys : ECSM.get_systems()) {
            if (auto* gs = dynamic_cast<GUI_System*>(sys.get())) {
                gui_system = gs;
                break;
            }
        }
        // 2) Check if the game-over screen is currently shown
        bool isGameOverShown = false;
        if (gui_system) {
            isGameOverShown = gui_system->is_game_over_shown();
        }

        // Pause Logic
        if (!isGameOverShown) {
            if (IM.is_key_pressed(GLFW_KEY_ESCAPE)) {
                // Only toggle pause in gameplay scenes
                if (current_scene == 1 || current_scene == 2) {
                    LM.write_log("ESC pressed, current pause state: %d", m_is_paused);
                    toggle_pause();
                    LM.write_log("New pause state: %d", m_is_paused);
                }
                else {
                    set_game_over(true);
                    LM.write_log("Escape pressed outside gameplay, setting game_over = true.");
                }
            }
        }

        //to pause all the sound that is playing
        if (IM.is_key_pressed(GLFW_KEY_5) && !level_editor_mode) {
            ADM.pause_resume_mastergroup();
        }

        ////to ensure sound pause during level_editor_mode
        //if (IM.is_key_pressed(GLFW_KEY_TAB)) {
        //    ADM.pause_resume_mastergroup();
        //}

        //to ensure sound pause during level_editor_mode
        if (!game_playing) {
            ADM.pause_resume_mastergroup();
            prev_pasued = true;
        }
        else if (prev_pasued) {
            ADM.pause_resume_mastergroup();
            prev_pasued = false;
        }

        ////commented out this is for me to test - Amanda
        //if (IM.is_key_held(GLFW_KEY_J)) {
        //    current_oxygen_level--;
        //    //std::cout << "current oxygen level " << current_oxygen_level << std::endl;
        //    //increasing = false;
        //}

        //if (IM.is_key_held(GLFW_KEY_K)) {
        //    current_oxygen_level++;
        //    //std::cout << "current oxygen level " << current_oxygen_level << std::endl;
        //    //increasing = true;
        //}

        //printf("bool check: %d\n", CS.is_oxygen_increase());
        //std::cout << "current oxygen levvel outside " << oxygen_level << std::endl;
       /* if (current_scene == 2 || current_scene == 1)
        {
            ADM.update_bgm_layering(current_scene, current_oxygen_level, increasing);

        }*/

        // Handle player movement and physics input
        EntityID player_id = ECSM.find_entity_by_name(DEFAULT_PLAYER_NAME);

        // Code portion if in gameplay mode or player exists
        if (player_id != INVALID_ENTITY_ID && game_playing) {  // If player entity exists
            if (!is_paused() && !isGameOverShown) {
                oxygen_update_timer += delta_time;
                if (oxygen_update_timer >= 1.2f) {
                    oxygen_update_timer = 0.0f;
                    current_oxygen_level = std::max(0.0f, current_oxygen_level - oxygen_drain_rate);

                    if (current_oxygen_level < 50.0f) {
                        panic_triggered = true;
                        no_panic = false;
                    }
                    else {
                        panic_triggered = false;
                        no_panic = true;
                    }
                }

                //////////////////  TESTING /////////////////////////
                if (IM.is_key_pressed(GLFW_KEY_P)) {
                    add_panic(DEFAULT_FIXED_DELTA_TIME);
                }

                // Sweat particles based on panic meter
                if (current_panic_level >= 50.0f) {
                    for (auto& system : ECSM.get_systems()) {
                        if (system->get_type() == "Particle_System") {
                            auto* particle_system = static_cast<Particle_System*>(system.get());
                            if (!particle_system) {
                                LM.write_log("Mining_Script::update_mining(): Fail to get particle system");
                                std::cerr << "Failed to get particle system" << std::endl;
                                return;
                            }

                            // Emit sweat particles on player (30% rate)
                            if (particle_system->get_rand_float() < 0.3f) {
                                auto& player_direction = GFXM.get_player_direction();
                                auto& player_transform = ECSM.get_component<Transform2D>(player_id);

                                // Sweat on player
                                if (player_direction == FACE_RIGHT) {
                                    float part_x = player_transform.position.x - 30.0f + (particle_system->get_rand_float() * 60.0f);
                                    float part_y = player_transform.position.y + 35.0f;
                                    particle_system->particle_emit("sweat_player", Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f));
                                }
                                else {
                                    float part_x = player_transform.position.x - 25.0f + (particle_system->get_rand_float() * 60.0f);
                                    float part_y = player_transform.position.y + 35.0f;
                                    particle_system->particle_emit("sweat_player", Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f));
                                }

                                // Sweat on the player's helmet from POV
                                unsigned int screen_width = static_cast<float>(SM.get_scr_width()); 
                                unsigned int screen_height = static_cast<float>(SM.get_scr_height()); 
                                float part_x = -(screen_width / 2.0f) + (particle_system->get_rand_float() * screen_width);
                                float part_y = player_transform.position.y + screen_height / 3.25f;
                                particle_system->particle_emit("sweat_screen", Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f));

                            }
                        }
                    }
                }
            }

            if (panic_triggered) {
                add_panic(DEFAULT_FIXED_DELTA_TIME);
            }
            if (no_panic) {
                drop_panic(DEFAULT_FIXED_DELTA_TIME);
            }
            //update the current panic level
            current_panic_level = panic_current;

            // Display red vignette based on panic level
            EntityID red_vignette = ECSM.find_entity_by_name("red_vignette");
            if (red_vignette != INVALID_ENTITY_ID) {
                
                // Adjust alpha value of red_vignette
                auto& red_vignette_graphics = ECSM.get_component<Graphics_Component>(red_vignette);
                if (current_panic_level >= 50.0f) {
                    red_vignette_graphics.color.a = 2 * (current_panic_level - 49.0f) / 100.0f;
                }
                else {
                    red_vignette_graphics.color.a = 0.0f;
                }
            }

            // Update top UI overlay position to follow player
            EntityID ui_overlay_id = ECSM.find_entity_by_name("top_ui_overlay");

            EntityID oxygen_meter_fill_id = ECSM.find_entity_by_name("top_ui_oxygen_meter_fill");
            EntityID oxygen_meter_id = ECSM.find_entity_by_name("top_ui_oxygen_meter");

            EntityID panic_meter_fill_id = ECSM.find_entity_by_name("top_ui_panik_meter_fill");
            EntityID panic_meter_id = ECSM.find_entity_by_name("top_ui_panik_meter");

            //EntityID mineral_texture_id = ECSM.find_entity_by_name("top_ui_mineral_texture");
            //EntityID goal_text_id = ECSM.find_entity_by_name("top_ui_goal_text");

            EntityID oxygen_text_id = ECSM.find_entity_by_name("top_ui_oxygen_text");
            EntityID oxygen_percentage_text_id = ECSM.find_entity_by_name("top_ui_oxygen_percentage_text");
            EntityID panic_text_id = ECSM.find_entity_by_name("top_ui_panic_text");
            //EntityID mineral_count_text_id = ECSM.find_entity_by_name("top_ui_mineral_count_text");
            EntityID goal_percentage_count_text_id = ECSM.find_entity_by_name("top_ui_goal_percentage_text");

            if (ui_overlay_id != INVALID_ENTITY_ID) {
                auto& player_transform = ECSM.get_component<Transform2D>(player_id);
                auto& ui_transform = ECSM.get_component<Transform2D>(ui_overlay_id);

                // Define layout constants for vertical stacking
                constexpr float VERTICAL_OFFSET = 500.0f;        // Distance above player
                constexpr float METER_SPACING = 50.0f;           // Vertical space between meters
                constexpr float METER_WIDTH = 400.0f;            // Width of the meters
                constexpr float METER_HEIGHT = 40.0f;            // Height of each meter bar

                // Calculate base position for UI elements
                Vec2D base_position{
                    0.0f,
                    player_transform.position.y + VERTICAL_OFFSET
                };

                // Position oxygen meter and fill (top meter)
                if (oxygen_meter_id != INVALID_ENTITY_ID &&
                    ECSM.has_component<Transform2D>(oxygen_meter_id) &&
                    ECSM.has_component<Transform2D>(oxygen_meter_fill_id)) {
                    auto& oxygen_transform = ECSM.get_component<Transform2D>(oxygen_meter_id);
                    auto& oxygen_fill_transform = ECSM.get_component<Transform2D>(oxygen_meter_fill_id);
                    auto& oxygen_fill_graphics = ECSM.get_component<Graphics_Component>(oxygen_meter_fill_id);

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

                // Display fps if fps flag is true
                if (display_fps == true) {
                    EntityID game_fps_counter_id = ECSM.find_entity_by_name("fps_counter_in_game");
                    if (game_fps_counter_id != INVALID_ENTITY_ID && ECSM.has_component<Text_Component>(game_fps_counter_id)) {
                        auto& fps_text_comp = ECSM.get_component<Text_Component>(game_fps_counter_id);
                        //auto& fps_transform_comp = ECSM.get_component<Transform2D>(game_fps_counter_id);

                        // Write the current fps to the text object
                        float current_fps = FPSM.get_current_fps();
                        std::stringstream ss;
                        ss << "FPS: " << std::fixed << std::setprecision(1) << current_fps;
                        fps_text_comp.text = ss.str();

                    }
                }
                else {
                    EntityID game_fps_counter_id = ECSM.find_entity_by_name("fps_counter_in_game");
                    if (game_fps_counter_id != INVALID_ENTITY_ID && ECSM.has_component<Text_Component>(game_fps_counter_id)) {
                        auto& fps_text_comp = ECSM.get_component<Text_Component>(game_fps_counter_id);

                        // Undo the text for fps
                        fps_text_comp.text = "";
                    }
                }

                // ------------------------- TIMER UPDATE CHANGES -------------------------
                //// 1) Accumulate delta_time into an accumulator and decrease timer by 1 when >= 1s
                //static float timer_accumulator = 0.0f; // You can make this a class member if you like
                //timer_accumulator += delta_time;
                //if (timer_accumulator >= 1.0f) {
                //    timer_accumulator = 0.0f;

                //    // Only decrease if you haven't hit zero
                //    if (timer_remaining > 0) {
                //        timer_remaining -= 1;
                //    }
                //}

                //if (timer_count_text_id != INVALID_ENTITY_ID &&
                //    ECSM.has_component<Transform2D>(timer_count_text_id) &&
                //    ECSM.has_component<Transform2D>(timer_icon_id))
                //{
                //    // If it has a Text_Component, update the visible text to show the integer countdown
                //    if (ECSM.has_component<Text_Component>(timer_count_text_id)) {
                //        auto& timer_text_comp = ECSM.get_component<Text_Component>(timer_count_text_id);
                //        timer_text_comp.text = std::to_string(timer_remaining);
                //    }
                //}
                // ------------------------- END TIMER UPDATE CHANGES -------------------------

                if (goal_percentage_count_text_id != INVALID_ENTITY_ID &&
                    ECSM.has_component<Transform2D>(goal_percentage_count_text_id) &&
                    ECSM.has_component<Text_Component>(goal_percentage_count_text_id)) {

                    // Update the text value based on mineral progress
                    auto& text_comp = ECSM.get_component<Text_Component>(goal_percentage_count_text_id);

                    // Find the GUI system to get the current stored mineral progress
                    for (auto& system : ECSM.get_systems()) {
                        if (auto* gui_system = dynamic_cast<GUI_System*>(system.get())) {
                            if (gui_system->get_current_hopper_percentage() > 0.0f) {
                                // Calculate and store the percentage in Game Manager
                                float new_percentage = (gui_system->get_current_hopper_percentage() * 50000.0f / 50000.0f) * 100.0f;
                                GM.set_stored_goal_percentage(new_percentage);
                            }

                            // Use the stored percentage from Game Manager
                            std::stringstream ss;
                            ss << std::setw(2) << std::setfill('0') << static_cast<int>(GM.get_stored_goal_percentage()) << "%";
                            text_comp.text = ss.str();
                            break;
                        }
                    }
                }
            }
        }

        // == End PLAYER CODE PORTION ==

        if (current_scene == 2) {
            // Check for oxygen level first - if it reaches zero, show game over screen
            if (current_oxygen_level <= 0.0f) {
                // Player is out of oxygen - show game over screen
                set_player_dead_state(true);

                // Stop all audio first
                ADM.stop_mastergroup();

                //reset panic 
                reset_panic();

                // Find GUI System and show game over screen
                for (auto& systems_gui : ECSM.get_systems()) {
                    if (auto* gui_system = dynamic_cast<GUI_System*>(systems_gui.get())) {
                        // First reset all GUI states
                        gui_system->reset_all_game_state();

                        // Then show the game over screen
                        gui_system->show_game_over_menu();
                        LM.write_log("Game over screen displayed - player ran out of oxygen");
                        break;
                    }
                }
            }

            // Lava update logic
            EntityID lava_pool_id = ECSM.find_entity_by_name("lava_pool");
            if (lava_pool_id != INVALID_ENTITY_ID && ECSM.has_component<Transform2D>(lava_pool_id)) {
                // Don't process lava in level editor mode
                if (!level_editor_mode && game_playing) {
                    // Update lava timer
                    lava_timer += delta_time;

                    // Debug log to verify lava timer is working
                    LM.write_log("Lava timer: %.2f of %.2f", lava_timer, LAVA_RISE_INTERVAL);

                    // Check if it's time to move the lava pool up
                    if (lava_timer >= LAVA_RISE_INTERVAL) {
                        auto& transform = ECSM.get_component<Transform2D>(lava_pool_id);

                        // Log current position before moving
                        LM.write_log("Current lava Y before moving: %.2f", transform.position.y);

                        // Move lava up by exactly one tile height
                        transform.position.y += tile_height;
                        transform.prev_position = transform.position;

                        // Reset timer but keep remainder for precise timing
                        lava_timer -= LAVA_RISE_INTERVAL;

                        LM.write_log("Game_Manager::update(): Moving lava pool up to Y=%.2f (tile height: %.2f)",
                            transform.position.y, tile_height);
                    }
                }

                // Emit lava splatter particles
                //if (static_cast<int>(lava_timer) < 1) {
                    for (auto& system : ECSM.get_systems()) {
                        if (system->get_type() == "Particle_System") {
                            auto* particle_system = static_cast<Particle_System*>(system.get());
                            if (!particle_system) {
                                LM.write_log("Game_Manager::update(): Fail to get particle system");
                                std::cerr << "Failed to get particle system" << std::endl;
                                return;
                            }

                            if (particle_system->get_rand_float() < 0.05f) {
                                // Get lava transform and animation components
                                auto& lava_transform = ECSM.get_component<Transform2D>(lava_pool_id); 

                                // Randomize particle emit count
                                float part_x = lava_transform.position.x - (lava_transform.scale.x / 2.0f) + (particle_system->get_rand_float() * lava_transform.scale.x);
                                float part_y = lava_transform.position.y + (lava_transform.scale.y / 2.0f);
                                particle_system->particle_emit("lava", Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f), particle_system->get_rand_float() * 5.0f);
                            }


                        }
                    }
                //}
            }
            else {
                // Log warning if lava pool entity doesn't exist
                LM.write_log("Lava pool entity not found or missing Transform2D component");
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

            /*if (IM.is_key_held(GLFW_KEY_UP) && !(IM.is_key_held(GLFW_KEY_DOWN)))
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
            }*/
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

                //reset panic
                reset_panic();

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

        IMGUIM.update_buttons_and_batches();

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

    //int Game_Manager::get_mineral_value(EntityID block_id) const {
    //    if (!ECSM.has_component<Animation_Component>(block_id)) {
    //        return 0;
    //    }

    //    auto* entity = ECSM.get_entity(block_id);
    //    if (!entity) {
    //        return 0;
    //    }

    //    const std::string& name = entity->get_name();
    //    LM.write_log("Checking mineral value for entity with name: %s", name.c_str());

    //    // Match the prefab names with their corresponding values
    //    if (name.find("quartz") != std::string::npos) {
    //        LM.write_log("Found quartz mineral, value: 100");
    //        return 100;
    //    }
    //    if (name.find("emerald") != std::string::npos) {
    //        LM.write_log("Found emerald mineral, value: 800");
    //        return 800;
    //    }
    //    if (name.find("sapphire") != std::string::npos) {
    //        LM.write_log("Found sapphire mineral, value: 1600");
    //        return 1600;
    //    }
    //    if (name.find("amethyst") != std::string::npos) {
    //        LM.write_log("Found amethyst mineral, value: 2400");
    //        return 2400;
    //    }
    //    if (name.find("citrine") != std::string::npos) {
    //        LM.write_log("Found citrine mineral, value: 3200");
    //        return 3200;
    //    }
    //    if (name.find("alexandrite") != std::string::npos) {
    //        LM.write_log("Found alexandrite mineral, value: 4000");
    //        return 4000;
    //    }

    //    LM.write_log("No mineral value found for this entity");
    //    return 0;
    //}

    //void Game_Manager::update_mineral_count_text(int value_to_add) {
    //    EntityID text_entity = ECSM.find_entity_by_name("top_ui_mineral_count_text");
    //    if (text_entity == INVALID_ENTITY_ID) {
    //        LM.write_log("Could not find mineral count text entity");
    //        return;
    //    }

    //    if (!ECSM.has_component<Text_Component>(text_entity)) {
    //        LM.write_log("Mineral count entity does not have Text_Component");
    //        return;
    //    }

    //    try {
    //        auto& text_comp = ECSM.get_component<Text_Component>(text_entity);
    //        // Convert current text to integer, add new value
    //        int current_value = std::stoi(text_comp.text);
    //        current_value += value_to_add;

    //        // Format the number with leading zeros (6 digits)
    //        std::stringstream ss;
    //        ss << std::setw(6) << std::setfill('0') << current_value;
    //        text_comp.text = ss.str();

    //        LM.write_log("Updated mineral count to: %06d", current_value);
    //    }
    //    catch (const std::exception& e) {
    //        LM.write_log("Error updating mineral count: %s", e.what());
    //    }
    //}

    void Game_Manager::set_current_scene(int scene_num) {
        current_scene = scene_num;

        //reset panic for gameplay when changing scenes
        if (scene_num == 1 || scene_num == 2) {
            reset_panic();
        }
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


    void Game_Manager::add_panic(float dt) {
        panic_timer += dt; 
        if ((panic_timer >= panic_timer_increase_delay) && (panic_current < 100.0f)) {
            panic_current += panic_increase_amount; 
            panic_timer = 0.0f; //reset the timer

            if (panic_current > 100.0f) panic_current = 100.0f; 
        }
        else if (panic_current >= 100.0f) {
            panic_triggered = false;
            panic_current = 100.0f;
        }
        //std::cout << "Panici current level: " << panic_current << std::endl;
    }

    void Game_Manager::drop_panic(float dt) {
        panic_timer += dt; 
        if (panic_timer >= panic_timer_decrease_delay && panic_current > 0) {
            panic_current -= panic_decrease_amount; 
            panic_timer = 0.0f; //reset the timer
            if (panic_current <= 0.0f) {
                panic_current = 0.0f;
                no_panic = false;
            }
        }
    }

} // namespace lof
