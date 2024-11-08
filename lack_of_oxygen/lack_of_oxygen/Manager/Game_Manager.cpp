/**
 * @file Game_Manager.cpp
 * @brief Implements the Game_Manager class helper functions.
 * @author Simon Chan (82.901%), Amanda Leow (11.91%), Liliana Hanawardani (5.181%)
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
#include "../Utility/Path_Helper.h"
#include "../Utility/Globals.h"

// Include systems
#include "../System/GUI_System.h"  // Add this for GUI system access


// Include iostream for console output
#include <iostream>
#include <random>
#include <chrono>

namespace lof {

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

    void Game_Manager::update(float delta_time) {
        if (!is_started()) {
            LM.write_log("Game_Manager::update(): Game_Manager not started");
            return;
        }

        // Check if the left mouse button was pressed
        if (IM.is_mouse_button_pressed(GLFW_MOUSE_BUTTON_LEFT)) {
            // Handle left mouse button press
            std::cout << "Left mouse button pressed." << std::endl;
        }

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

        // GUI System control
        if (IM.is_key_pressed(GLFW_KEY_G) && !level_editor_mode) {
            // Find GUI System
            for (auto& system : ECSM.get_systems()) {
                if (system->get_type() == "GUI_System") {
                    auto* gui_system = static_cast<GUI_System*>(system.get());
                    if (gui_system) {
                        // Toggle loading screen
                        static bool loading_screen_visible = false;
                        if (!loading_screen_visible) {
                            loading_screen_visible = true;
                            gui_system->show_loading_screen();
                            LM.write_log("Game_Manager::update(): Showing loading screen");
                        }
                        else {
                            loading_screen_visible = false;
                            gui_system->hide_loading_screen();
                            LM.write_log("Game_Manager::update(): Hiding loading screen");
                        }
                    }
                    break;
                }
            }
        }

        // Test progress bar updates with H key
        if (IM.is_key_pressed(GLFW_KEY_H) && !level_editor_mode) {
            static float test_progress = 0.0f;
            test_progress += 0.1f;
            if (test_progress > 1.0f) test_progress = 0.0f;

            // Find GUI System and update progress
            for (auto& system : ECSM.get_systems()) {
                if (system->get_type() == "GUI_System") {
                    auto* gui_system = static_cast<GUI_System*>(system.get());
                    if (gui_system) {
                        gui_system->set_progress(test_progress);
                        LM.write_log("Game_Manager::update(): Updated progress bar to %.2f", test_progress);
                    }
                    break;
                }
            }
        }

        std::cout << "DEBUG: -------------------------------GAME MANAGER START PHYSICS INPUT -------------------------------------\n";
        // Handle player movement and physics input
        EntityID player_id = ECSM.find_entity_by_name(DEFAULT_PLAYER_NAME);

        std::cout << "player_id: " << player_id << '\n';
        if (player_id != 0) {  // If player entity exists
            if (ECSM.has_component<Physics_Component>(player_id)) {

                auto& physics = ECSM.get_component<Physics_Component>(player_id);


                if (IM.is_key_held(GLFW_KEY_SPACE)) {
                    std::cout << "space_key pressed\n";
                    physics.set_jump_requested(true);
                }
                // Handle horizontal movement
                if (IM.is_key_held(GLFW_KEY_A)) {
                    std::cout << "A pressed\n";
                    physics.force_helper.activate_force(MOVE_LEFT);
                }
                else {
                    physics.force_helper.deactivate_force(MOVE_LEFT);
                }
                if (IM.is_key_held(GLFW_KEY_D)) {
                    std::cout << "D pressed\n";
                    physics.force_helper.activate_force(MOVE_RIGHT);
                }
                else {
                    physics.force_helper.deactivate_force(MOVE_RIGHT);
                }

                const auto& forces = physics.force_helper.get_forces();
                if (forces.empty()) {
                    std::cout << "No forces present in the force_helper!!!!\n";
                }
                else {


                // Debug print to display all forces inside force_helper.forces
                std::cout << "Forces currently applied to the player:" << std::endl;
                for ( auto& force : physics.force_helper.get_forces()) {
                    std::cout << "Force Type: " << Force::ftype_to_string(force.type)
                        << ", Direction: (" << force.direction.x << ", " << force.direction.y << ")"
                        << ", Magnitude: " << force.magnitude
                        << ", Is Active: " << (force.is_active ? "Yes" : "No") << std::endl;
                }
                }

            }
        }
        std::cout << "DEBUG: -------------------------------GAME MANAGER END PHYSICS INPUT -------------------------------------\n";


        // Getting delta time for Input Manager
        IM.set_time(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
        // Update Input_Manager
        IM.update();
        IM.set_time(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() - IM.get_time());

        // Getting delta time for Graphics Manager
        GFXM.set_time(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
        // Update Graphics Manager
        GFXM.update();
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

} // namespace lof
