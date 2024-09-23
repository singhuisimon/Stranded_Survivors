/**
 * @file Input_Manager.h
 * @brief Defines the Input_Manager class for handling user inputs.
 * @date September 23, 2024
 */

#ifndef LOF_INPUT_MANAGER_H
#define LOF_INPUT_MANAGER_H

// Macros for accessing manager singleton instances
#define IM  lof::Input_Manager::get_instance()

// Include base Manager class
#include "Manager.h"

// Include GLFW for key codes
#include <GLFW/glfw3.h>

// Include standard headers
#include <unordered_map>
#include <vector>
#include <string>

namespace lof {

    /**
     * @enum KeyState
     * @brief Represents the state of a key.
     */
    enum class KeyState {
        NONE,
        PRESSED,
        HELD,
        RELEASED
    };

    /**
     * @class Input_Manager
     * @brief Manages user input by tracking key presses and releases.
     */
    class Input_Manager : public Manager {
    private:
        // Private constructor for singleton pattern
        Input_Manager();

        // Member variables
        std::unordered_map<int, KeyState> key_states; // Maps GLFW key codes to their states

        // Helper methods
        void set_key_state(int key, KeyState state);

        // GLFW key callback needs to access set_key_state
        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    public:
        /**
         * @brief Get the singleton instance of Input_Manager.
         * @return Reference to the singleton instance.
         */
        static Input_Manager& get_instance();

        // Delete copy constructor and assignment operator
        Input_Manager(const Input_Manager&) = delete;
        Input_Manager& operator=(const Input_Manager&) = delete;

        /**
         * @brief Start up the Input_Manager by setting the GLFW key callback.
         * @return 0 on success, negative value on failure.
         */
        int start_up() override;

        /**
         * @brief Shut down the Input_Manager.
         */
        void shut_down() override;

        /**
         * @brief Update the state of all keys. Should be called once per frame.
         */
        void update();

        /**
         * @brief Check if a key was pressed this frame.
         * @param key GLFW key code.
         * @return True if the key was pressed, false otherwise.
         */
        bool is_key_pressed(int key) const;

        /**
         * @brief Check if a key is currently held down.
         * @param key GLFW key code.
         * @return True if the key is held, false otherwise.
         */
        bool is_key_held(int key) const;

        /**
         * @brief Check if a key was released this frame.
         * @param key GLFW key code.
         * @return True if the key was released, false otherwise.
         */
        bool is_key_released(int key) const;

        /**
         * @brief Reset all key states. Useful when initializing or resetting the game.
         */
        void reset();
    };

} // namespace lof

#endif // LOF_INPUT_MANAGER_H
