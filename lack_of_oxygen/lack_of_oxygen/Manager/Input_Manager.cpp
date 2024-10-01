/**
 * @file Input_Manager.cpp
 * @brief Implements the Input_Manager class methods, including mouse input handling.
 * @date September 23, 2024
 * Copyright (C) 20xx DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */
 // Include header
#include "Input_Manager.h"

// Include Log_Manager for logging
#include "Log_Manager.h"

// Include GLFW for window reference
#include <GLFW/glfw3.h>

// Include iostream for console output
#include <iostream>

namespace lof {

    // Initialize the singleton instance
    Input_Manager& Input_Manager::get_instance() {
        static Input_Manager instance;
        return instance;
    }

    // Private constructor
    Input_Manager::Input_Manager() {
        // Initialize all key and mouse button states to NONE
    }

    // Set the key state
    void Input_Manager::set_key_state(int key, KeyState state) {
        if (key < 0) return; // Ignore invalid keys

        key_states[key] = state;

        // Log the key event via Log_Manager
        LM.write_log("Key %d set to state %d", key, static_cast<int>(state));

        // Also output to the console
        std::cout << "Key " << key << " set to state " << static_cast<int>(state) << std::endl;
    }

    // Set the mouse button state
    void Input_Manager::set_mouse_button_state(int button, KeyState state) {
        if (button < 0) return; // Ignore invalid buttons

        mouse_button_states[button] = state;

        // Log the mouse button event via Log_Manager
        LM.write_log("Mouse Button %d set to state %d", button, static_cast<int>(state));

        // Also output to the console
        std::cout << "Mouse Button " << button << " set to state " << static_cast<int>(state) << std::endl;
    }

    // Static key callback function
    void Input_Manager::key_callback(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int /*mods*/) {
        // Log key event
        std::cout << "Key Callback: Key " << key << " Action " << action << std::endl;

        // Ignore unknown keys
        if (key < 0) return;

        switch (action) {
        case GLFW_PRESS:
            IM.set_key_state(key, KeyState::PRESSED);
            break;
        case GLFW_RELEASE:
            IM.set_key_state(key, KeyState::RELEASED);
            break;
        case GLFW_REPEAT:
            // Treat repeat as held
            IM.set_key_state(key, KeyState::HELD);
            break;
        default:
            break;
        }
    }

    // Static mouse button callback function
    void Input_Manager::mouse_button_callback(GLFWwindow* /*window*/, int button, int action, int /*mods*/) {
        // Log mouse button event
        std::cout << "Mouse Button Callback: Button " << button << " Action " << action << std::endl;

        // Ignore unknown buttons
        if (button < 0) return;

        switch (action) {
        case GLFW_PRESS:
            IM.set_mouse_button_state(button, KeyState::PRESSED);
            break;
        case GLFW_RELEASE:
            IM.set_mouse_button_state(button, KeyState::RELEASED);
            break;
        default:
            break;
        }
    }

    // Start up the Input_Manager
    int Input_Manager::start_up() {
        // Retrieve the GLFW window from the current context
        GLFWwindow* window = glfwGetCurrentContext();
        if (!window) {
            LM.write_log("Input_Manager::start_up(): No current GLFW window context.");
            return -1;
        }

        // Set the key callback
        glfwSetKeyCallback(window, Input_Manager::key_callback);

        // Set the mouse button callback
        glfwSetMouseButtonCallback(window, Input_Manager::mouse_button_callback);

        LM.write_log("Input_Manager::start_up(): Input_Manager started and callbacks set.");
        std::cout << "Input_Manager started and callbacks set." << std::endl;
        return 0;
    }

    // Shut down the Input_Manager
    void Input_Manager::shut_down() {
        // Remove the callbacks by setting them to nullptr
        GLFWwindow* window = glfwGetCurrentContext();
        if (window) {
            glfwSetKeyCallback(window, nullptr);
            glfwSetMouseButtonCallback(window, nullptr);
            LM.write_log("Input_Manager::shut_down(): Input_Manager shut down and callbacks removed.");
            std::cout << "Input_Manager shut down and callbacks removed." << std::endl;
        }
    }

    // Update key and mouse button states each frame
    void Input_Manager::update() {
        // ------------------------ Update key states -------------------------
        for (auto& pair : key_states) {
            KeyState& state = pair.second;
            switch (state) {
            case KeyState::PRESSED:
                state = KeyState::HELD;
                break;
            case KeyState::RELEASED:
                state = KeyState::NONE;
                break;
            default:
                break;
            }
        }

        // Update mouse button states
        for (auto& pair : mouse_button_states) {
            KeyState& state = pair.second;
            switch (state) {
            case KeyState::PRESSED:
                state = KeyState::HELD;
                break;
            case KeyState::RELEASED:
                state = KeyState::NONE;
                break;
            default:
                break;
            }
        }
    }

    // Check if key was pressed this frame
    bool Input_Manager::is_key_pressed(int key) const {
        auto it = key_states.find(key);
        if (it != key_states.end()) {
            return it->second == KeyState::PRESSED;
        }
        return false;
    }

    // Check if key is held down
    bool Input_Manager::is_key_held(int key) const {
        auto it = key_states.find(key);
        if (it != key_states.end()) {
            return it->second == KeyState::HELD;
        }
        return false;
    }

    // Check if key was released this frame
    bool Input_Manager::is_key_released(int key) const {
        auto it = key_states.find(key);
        if (it != key_states.end()) {
            return it->second == KeyState::RELEASED;
        }
        return false;
    }

    // Check if mouse button was pressed this frame
    bool Input_Manager::is_mouse_button_pressed(int button) const {
        auto it = mouse_button_states.find(button);
        if (it != mouse_button_states.end()) {
            return it->second == KeyState::PRESSED;
        }
        return false;
    }

    // Check if mouse button is held down
    bool Input_Manager::is_mouse_button_held(int button) const {
        auto it = mouse_button_states.find(button);
        if (it != mouse_button_states.end()) {
            return it->second == KeyState::HELD;
        }
        return false;
    }

    // Check if mouse button was released this frame
    bool Input_Manager::is_mouse_button_released(int button) const {
        auto it = mouse_button_states.find(button);
        if (it != mouse_button_states.end()) {
            return it->second == KeyState::RELEASED;
        }
        return false;
    }

    // Reset all key and mouse button states
    void Input_Manager::reset() {
        key_states.clear();
        mouse_button_states.clear();
        LM.write_log("Input_Manager::reset(): All key and mouse button states reset.");
        std::cout << "Input_Manager::reset(): All key and mouse button states reset." << std::endl;
    }

} // namespace lof
