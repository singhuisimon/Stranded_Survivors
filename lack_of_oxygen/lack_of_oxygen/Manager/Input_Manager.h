/**
 * @file Input_Manager.h
 * @brief Defines the Input_Manager class for handling user inputs, including keyboard and mouse.
 * @date September 23, 2024
 * Copyright (C) 20xx DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#ifndef LOF_INPUT_MANAGER_H
#define LOF_INPUT_MANAGER_H

 // Macros for accessing manager singleton instances
#define IM  lof::Input_Manager::get_instance()

// Include base Manager class
#include "Manager.h"

// Include GLFW for key and mouse button codes
#include <GLFW/glfw3.h>

// Include standard headers
#include <unordered_map>
#include <vector>
#include <string>

namespace lof {

    /**
     * @enum KeyState
     * @brief Represents the state of a key or mouse button.
     */
    enum class KeyState {
        NONE,       ///< No interaction.
        PRESSED,    ///< Pressed this frame.
        HELD,       ///< Held down.
        RELEASED    ///< Released this frame.
    };

    /**
     * @class Input_Manager
     * @brief Manages user input by tracking key presses, releases, and mouse interactions.
     */
    class Input_Manager : public Manager {
    private:
        // Private constructor for singleton pattern
        Input_Manager();

        // Member variables
        std::unordered_map<int, KeyState> key_states;    ///< Maps GLFW key codes to their states.
        std::unordered_map<int, KeyState> mouse_button_states; ///< Maps GLFW mouse button codes to their states.

        // Helper methods
        void set_key_state(int key, KeyState state);
        void set_mouse_button_state(int button, KeyState state);

        // GLFW callbacks need to access set_key_state and set_mouse_button_state
        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

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
         * @brief Start up the Input_Manager by setting GLFW callbacks.
         * @return 0 on success, negative value on failure.
         */
        int start_up() override;

        /**
         * @brief Shut down the Input_Manager.
         */
        void shut_down() override;

        /**
         * @brief Update the state of all keys and mouse buttons. Should be called once per frame.
         */
        void update();

        // Key input methods

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

        // Mouse input methods

        /**
         * @brief Check if a mouse button was pressed this frame.
         * @param button GLFW mouse button code.
         * @return True if the button was pressed, false otherwise.
         */
        bool is_mouse_button_pressed(int button) const;

        /**
         * @brief Check if a mouse button is currently held down.
         * @param button GLFW mouse button code.
         * @return True if the button is held, false otherwise.
         */
        bool is_mouse_button_held(int button) const;

        /**
         * @brief Check if a mouse button was released this frame.
         * @param button GLFW mouse button code.
         * @return True if the button was released, false otherwise.
         */
        bool is_mouse_button_released(int button) const;

        /**
         * @brief Reset all key and mouse button states. Useful when initializing or resetting the game.
         */
        void reset();
    };

} // namespace lof

#endif // LOF_INPUT_MANAGER_H
