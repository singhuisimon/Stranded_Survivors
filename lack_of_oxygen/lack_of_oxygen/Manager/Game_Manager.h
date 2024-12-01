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
        int current_scene = 1;


        /**
         * @brief Determines the mineral value based on the block entity type
         * @param block_id The EntityID of the block being destroyed
         * @return The mineral value associated with that block type
         */
        int get_mineral_value(EntityID block_id) const;

        /**
         * @brief Updates the mineral count text in the UI
         * @param value_to_add The value to add to the current mineral count
         */
        void update_mineral_count_text(int value_to_add);

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

        void set_current_scene(int scene_num);

        int get_current_scene();
    };

} // namespace lof

#endif // LOF_GAME_MANAGER_H
