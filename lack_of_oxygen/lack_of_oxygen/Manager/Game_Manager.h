/**
 * @file Game_Manager.h
 * @brief Defines the Game_Manager class for managing the game state and logic.
 * @author Simon Chan
 * @date September 16, 2024
 */

#ifndef LOF_GAME_MANAGER_H
#define LOF_GAME_MANAGER_H

// Include base Manager class
#include "Manager.h"

namespace lof {

    // Two-letter acronym for easier access to manager.
    #define GM lof::Game_Manager::get_instance()

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
        Game_Manager();                      // Private since a singleton.
        Game_Manager(Game_Manager const&);   // Don't allow copy.
        void operator=(Game_Manager const&); // Don't allow assignment.
        bool m_game_over;                    // True -> game loop should stop.
        int m_step_count;                    // Count of game loop iterations.

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
         * @return 0 if successful, -1 if Log_Manager fails to start, -2 if ECS_Manager fails to start.
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
    };

} // end of namespace lof

#endif // LOF_GAME_MANAGER_H