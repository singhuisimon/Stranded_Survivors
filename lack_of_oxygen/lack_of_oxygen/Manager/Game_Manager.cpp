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
#include "Audio_Manager.h"
#include "../Utility/Constant.h"
#include "../Utility/Path_Helper.h"

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
            return -6;
        }
        else {
            LM.write_log("Game_Manager::start_up(): Graphics_Manager start_up() successful");
        }
        // -------------------------- Audio Manager Start Up --------------------------
        if (AM.start_up() != 0) {
            LM.write_log("Game_Manager::start_up(): Audio_Manager start_up() failed");
            GFXM.shut_down();
            IM.shut_down();
            FPSM.shut_down();
            SM.shut_down();
            ECSM.shut_down();
            LM.shut_down();
            return -7;
        }
        else {
            LM.write_log("Game_Manager::start_up(): Audio_Manager start_up() successful"); 
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
        AM.shut_down();   // Audio_Manager
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

        // Clone a game object from a prefab when 'C' key is pressed
        bool c_key_pressed = IM.is_key_pressed(GLFW_KEY_C);
        if (IM.is_key_pressed(GLFW_KEY_C) && !c_key_was_pressed_last_frame) {
            // Clone the entity from the prefab
            EntityID new_entity = ECSM.clone_entity_from_prefab("dummy_object");
            if (new_entity != INVALID_ENTITY_ID) {
                // Generate random position
                static std::default_random_engine generator;
                static std::uniform_real_distribution<float> distribution(-2500.0f, 2500.0f);

                float random_x = distribution(generator);
                float random_y = distribution(generator);

                // Get the Transform2D component and set its position
                if (ECSM.has_component<Transform2D>(new_entity)) {
                    Transform2D& transform = ECSM.get_component<Transform2D>(new_entity);
                    transform.position.x = random_x;
                    transform.position.y = random_y;

                    LM.write_log("Game_Manager::update:Cloned entity %u at random position (%f, %f)", new_entity, random_x, random_y);
                }
                else {
                    LM.write_log("Game_Manager::update:Cloned entity %u does not have a Transform2D component.", new_entity);
                }
            }
            else {
                LM.write_log("Game_Manager::update:Failed to clone entity from prefab 'dummy_object'.");
            }
        }
        c_key_was_pressed_last_frame = c_key_pressed;

        // Save game state when 'K' key is pressed
        bool k_key_pressed = IM.is_key_pressed(GLFW_KEY_K);
        if (k_key_pressed && !k_key_was_pressed_last_frame) {
            // Generate a filename with timestamp
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << "save_game_" << time << ".json";
            std::string filename = ss.str();

            // Replace illegal filename characters
            std::replace(filename.begin(), filename.end(), ':', '_');
            std::replace(filename.begin(), filename.end(), ' ', '_');

            // Get save file path using Path_Helper
            std::string save_path = Path_Helper::get_save_file_path(filename);

            // Attempt to save the game
            if (SM.save_game_state(save_path.c_str())) {
                LM.write_log("Game_Manager::update(): Successfully saved game state to %s", save_path.c_str());
                std::cout << "Game saved successfully to: " << filename << std::endl;
            }
            else {
                LM.write_log("Game_Manager::update(): Failed to save game state to %s", save_path.c_str());
                std::cout << "Failed to save game!" << std::endl;
            }
        }
        k_key_was_pressed_last_frame = k_key_pressed;

        // GUI System control
        if (IM.is_key_pressed(GLFW_KEY_G)) {
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
        if (IM.is_key_pressed(GLFW_KEY_H)) {
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
        
        //Play Audio BGM
        //check if key 0 is press if so play track1
        if (IM.is_key_pressed(GLFW_KEY_0)) {
            AM.play_bgm(TRACK1);
        }
        
        //check if key 9 is press if so play track2
        if (IM.is_key_pressed(GLFW_KEY_9)) {
            AM.play_bgm(TRACK2);
        }

        //check if key L is press if so stop music
        if (IM.is_key_pressed(GLFW_KEY_L)) {
            AM.stop_bgm();
        }

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

        // Getting delta time for Audio Manager
        AM.set_time(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
        // Update Audio Manager
        AM.update();
        AM.set_time(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() - AM.get_time());

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
