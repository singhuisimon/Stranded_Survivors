/**
 * @file Game_Manager.cpp
 * @brief Implements the Game_Manager class methods.
 * @author Simon Chan
 * @date September 16, 2024
 */

// Include header file
#include "Game_Manager.h"

// Include other managers
#include "Log_Manager.h"
#include "ECS_Manager.h"

namespace lof {

    Game_Manager::Game_Manager()
        : m_game_over(false), m_step_count(0) {
        set_type("Game_Manager");
    }

    Game_Manager& Game_Manager::get_instance() {
        static Game_Manager instance;
        return instance;
    }

    /**
     * @brief Start up all Game_Manager services.
     *
     * This method initializes the Game_Manager and other dependent managers.
     * It should be called before using any Game_Manager functionalities.
     *
     * @return 0 if successful, -1 if Log_Manager fails to start, -2 if ECS_Manager fails to start.
     */
    int Game_Manager::start_up() {
        if (is_started()) {
            return 0; // Already started
        }

        // Start up other managers
        if (LM.start_up() != 0) {
            return -1;
        }
        if (ECSM.start_up() != 0) {
            LM.write_log("Game_Manager::start_up(): ECS_Manager start_up() failed");
            return -2;
        }

        m_is_started = true;
        return 0;
    }


    void Game_Manager::shut_down() {
        if (!is_started()) {
            return;
        }

        ECSM.shut_down();
        LM.shut_down();

        m_is_started = false;
    }

    /**
     * @brief Run a single frame of game logic.
     *
     * This method updates the game state for a single frame. It should be called
     * once per frame in the main game loop.
     *
     * @param delta_time The time elapsed since the last update, in seconds.
     */
    void Game_Manager::update(float delta_time) {
        if (!is_started()) {
            LM.write_log("Game_Manager::update(): Game_Manager not started");
            return;
        }

        // Update game world state
        ECSM.update(delta_time);

        m_step_count++;
    }


    void Game_Manager::set_game_over(bool new_game_over) {
        m_game_over = new_game_over;
    }


    bool Game_Manager::get_game_over() const {
        return m_game_over;
    }


    int Game_Manager::get_step_count() const {
        return m_step_count;
    }

} // end of namespace lof