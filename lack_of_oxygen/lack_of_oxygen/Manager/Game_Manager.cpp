/**
 * @file Game_Manager.cpp
 * @brief Implements the Game_Manager class methods.
 * @date September 21, 2024
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

// Include iostream for console output
#include <iostream>

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

        // Start up Log_Manager first
        if (LM.start_up() != 0) {
            // If Log_Manager fails to start, there's no way to log the error using LM
            return -1;
        }
        else {
            LM.write_log("Game_Manager::start_up(): Log_Manager start_up() successful");
        }

        // Start up Config_Manager
        if (SM.start_up() != 0) {
            LM.write_log("Game_Manager::start_up(): Config_Manager start_up() failed");
            LM.shut_down();
            return -2;
        }
        else {
            LM.write_log("Game_Manager::start_up() : Config_Manager start_up() successful");
        }

        // Start up ECS_Manager
        if (ECSM.start_up() != 0) {
            LM.write_log("Game_Manager::start_up(): ECS_Manager start_up() failed");
            SM.shut_down();
            LM.shut_down();
            return -3;
        }
        else {
            LM.write_log("Game_Manager::start_up(): ECS_Manager start_up() successful");
        }

        // Load entities into ECS_Manager
        if (ECSM.load_entities(SM.get_entities_config(), SM.get_models()) != 0) {
            LM.write_log("Game_Manager::start_up(): Failed to load entities into ECS_Manager");
            ECSM.shut_down();
            SM.shut_down();
            LM.shut_down();
            return -4;
        }

        // Start up FPS_Manager
        if (FPSM.start_up() != 0) {
            LM.write_log("Game_Manager::start_up(): FPS_Manager start_up() failed");
            ECSM.shut_down();
            SM.shut_down();
            LM.shut_down();
            return -5;
        }
        else {
            LM.write_log("Game_Manager::start_up(): FPS_Manager start_up() successful");
        }

        // Start up Input_Manager
        if (IM.start_up() != 0) {
            LM.write_log("Game_Manager::start_up(): Input_Manager start_up() failed");
            FPSM.shut_down();
            ECSM.shut_down();
            SM.shut_down();
            LM.shut_down();
            return -6;
        }
        else {
            LM.write_log("Game_Manager::start_up(): Input_Manager start_up() successful");
        }

        // Start up Graphics_Manager
        if (GFXM.start_up() != 0) {
            LM.write_log("Game_Manager::start_up(): Graphics_Manager start_up() failed");
            FPSM.shut_down();
            ECSM.shut_down();
            SM.shut_down();
            LM.shut_down();
            IM.shut_down();
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
        IM.shut_down(); // Input_Manager
        FPSM.shut_down();
        ECSM.shut_down();
        SM.shut_down();
        LM.shut_down();

        m_is_started = false;
        std::cout << "Game_Manager shut down successfully." << std::endl;
    }

    void Game_Manager::update(float delta_time) {
        if (!is_started()) {
            LM.write_log("Game_Manager::update(): Game_Manager not started");
            return;
        }

        // Update Input_Manager (FOR TESTING: IM.UPDATE() NEW POSTIION)
        IM.update(); 

        // Update Graphics Manager (FOR TESTING: The update now is to check if 'P' is press to change the render mode)
        GFXM.update(); 

        // Update game world state
        ECSM.update(delta_time);

        // Check for game over condition based on input
        if (IM.is_key_pressed(GLFW_KEY_ESCAPE)) {
            set_game_over(true);
            LM.write_log("Game_Manager::update(): Escape key pressed. Setting game_over to true.");
            std::cout << "Escape key pressed. Closing the game." << std::endl;
        }

        //// Update Input_Manager (FOR TESTING: IM.UPDATE() ORIGINIAL POSTIION)
        //IM.update();

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