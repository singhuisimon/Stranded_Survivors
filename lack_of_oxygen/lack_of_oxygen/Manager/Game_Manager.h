/**
 * @file Game_Manager.h
 * @brief Defines the Game_Manager class for managing the game state and logic.
 * @author Simon Chan (93%), Chua Wen Bin Kenny (3%), Amanda Leow Boon Suan (2%), Saw Hui Shan (1%), Liliana Hanawardani (1%),  
 * @date September 21, 2024 
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */
#pragma once
#ifndef LOF_GAME_MANAGER_H
#define LOF_GAME_MANAGER_H

// Macros for accessing manager singleton instances
#define GM  lof::Game_Manager::get_instance()

// Include base Manager class
#include "Manager.h"

#include "../Entity/Entity.h"
#include <algorithm> // for std::clamp

#include <unordered_map> // For tnt logic

namespace lof {
    extern float imgui_camara_pos_x;
    extern float imgui_camera_pos_y;
   
    /**
     * @class Game_Manager
     * @brief Manages the overall game state and logic.
     *
     * This class is implemented as a singleton and is responsible for
     * coordinating game updates, managing the game loop, and controlling
     * the game's lifecycle.
     */
    class Game_Manager : public Manager {
    private:
        // Private constructor and assignment operator to enforce singleton pattern
        Game_Manager();
        Game_Manager(const Game_Manager&) = delete;
        Game_Manager& operator=(const Game_Manager&) = delete;

        bool m_game_over;   // True -> game loop should stop.
        int m_step_count;   // Count of game loop iterations.

        // Flag to prevent multiple key presses for cloning
        bool c_key_was_pressed_last_frame = false; 
        bool k_key_was_pressed_last_frame = false;    // Track K key state for saving

        // Flags to prevent camera scrolling buttons from conflicting
        int camera_up_down_scroll_flag = 0;
        int camera_left_right_scroll_flag = 0;

        // Flag for editor camera
        int editor_camera_flag = false;

        // Flag for forces applied
        int forces_flag = -1;

        // Flag for scn file
        int current_scene = 2;
        bool increasing = false;
        // Top UI variables
        float current_oxygen_level = 100.0f;  // Start at 100%
        float current_panic_level = 0.0f;  // Starts at 0%
        float ship_oxygen_level = 400.0f;     // [0..400]

        // Flags to indicate the panic trigger and for panic 
        bool panic_triggered = false;
        bool no_panic = false;

        float panic_timer = 0.0f; //time for panic level changes
        float panic_current = 0.0f; 
        float panic_increase_amount = 1.7f; //amount to increase panic every 0.5 sec
        float panic_timer_increase_delay = 0.5f; //delay between panic increases
        float panic_decrease_amount = 5.0f; //amount to decrease panic every 1 sec
        float panic_timer_decrease_delay = 1.0f; //delay between panic decreases

        float oxygen_drain_rate = 1.0f;     // Drain 5% per second
        float oxygen_update_timer = 0.0f;   // Track time for updates
        int timer_remaining = 300; // or any desired starting value

        float stored_goal_percentage = 0.0f;  // Store the goal percentage persistently

        // Stores the TNTs that are activated with it's entity name and fuse time
        std::unordered_map<std::string, float> tnt_to_destroy;

        // Pause logic
        bool m_is_paused;

        bool check_non_mineral(EntityID block_id, std::string block_name) const;

        // Flag for displaying fps in game
        bool display_fps = false;

        /**
        *@brief helper functions to increase and decrease panic 
        * 
        */
        void add_panic(float dt); 
        void drop_panic(float dt); 


        // Lava rise related variables
        float lava_timer = 0.0f;
        float tile_height = 0.0f;
        static constexpr float LAVA_RISE_INTERVAL = 4.5f; // 4.5 seconds per tile

    public:
        /**
         * @brief Get the singleton instance of the Game_Manager.
         * @return Reference to the Game_Manager instance.
         */
        static Game_Manager& get_instance();

        /**
         * @brief Start up all Game_Manager services.
         *
         * This method initializes the Game_Manager and other dependent managers.
         * It should be called before using any Game_Manager functionalities.
         *
         * @return 0 if successful,
         *         -1 if Log_Manager fails to start,
         *         -2 if Config_Manager fails to start,
         *         -3 if ECS_Manager fails to start,
         *         -4 if loading entities into ECS_Manager fails,
         *         -5 if FPS_Manager fails to start.
         */
        int start_up() override;

        /**
         * @brief Shut down all Game_Manager services.
         *
         * This method shuts down the Game_Manager and other dependent managers.
         * It should be called when the game is ending to ensure proper cleanup.
         */
        void shut_down() override;

        /**
         * @brief Run a single frame of game logic.
         * @param delta_time The time elapsed since the last update, in seconds.
         */
        void update(float delta_time);

        /**
         * @brief Set the game over status.
         * @param new_game_over The new game over status (default is true).
         */
        void set_game_over(bool new_game_over = true);

        /**
         * @brief Get the current game over status.
         * @return True if the game is over, false otherwise.
         */
        bool get_game_over() const;

        /**
         * @brief Get the current step count of the game loop.
         * @return The number of game loop iterations executed.
         */
        int get_step_count() const;

        /**
         * @brief Set the value of current_scene.
         * @param scene_num The index of the scene file in its name.
         */
        void set_current_scene(int scene_num);

        /**
         * @brief Get current value of current_scene.
         * @return The index of the current scene file in its name.
         */
        int get_current_scene();

        /**
         * @brief Returns the player's current oxygen level in the range [0..100].
         * @return The current oxygen level as a float.
         */
        float get_current_oxygen_level() const { return current_oxygen_level; }
        void  set_current_oxygen_level(float value) {
            current_oxygen_level = std::clamp(value, 0.0f, 100.0f);
        }

        //ash

        /**
        * @brief Get current panic level
        * @return the panic level
        */
        float get_current_panic_level() const { return current_panic_level; }
        void set_current_panic_level(float value) { 
            current_panic_level = std::clamp(value, 0.0f, 100.0f); 
        }
        /**
        * @brief Get panic triggered boolean
        * @return boolean that indicates whether panic triggered or not
        */
        bool get_panic_triggered() const { return panic_triggered; }
        void set_panic_triggered(bool value) { panic_triggered = value; }

        //ash
        

        /**
		 * @brief Returns the ship's current oxygen level in the range [0..400].
		 * @return The current ship oxygen level as a float.
		 */
        float get_ship_oxygen_level() const { return ship_oxygen_level; }
        void  set_ship_oxygen_level(float value) {
            ship_oxygen_level = std::clamp(value, 0.0f, 400.0f);
        }

        void reset_timer() { timer_remaining = 300; }

        float get_stored_goal_percentage() const { return stored_goal_percentage; }
        void set_stored_goal_percentage(float value) { stored_goal_percentage = value; }

        bool is_paused() const { return m_is_paused; }
        void set_paused(bool paused);
        void toggle_pause();

        void reset_lava_timer() { lava_timer = 0.0f; }
        void set_tile_height(float height) { tile_height = height; }

        void reset_panic() {
            panic_current = 0.0f; 
            panic_triggered = false; 
            no_panic = false;
            current_panic_level = 0.0f;
            panic_timer = 0.0f; 
        }

        bool is_player_dead = false;
        bool get_player_dead_state() const { return is_player_dead; }
        void set_player_dead_state(bool state) { is_player_dead = state; }
    };

} // namespace lof

#endif // LOF_GAME_MANAGER_H
