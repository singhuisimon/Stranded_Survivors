/**
 * @file Input_Manager.cpp
 * @brief Implements the Input_Manager class methods.
 * @date September 23, 2024
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
        set_type("Input_Manager");
        set_time(0);
        // Initialize all key states to NONE
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

    // Static key callback function
    void Input_Manager::key_callback(GLFWwindow* , int key, int , int action, int /*mods*/) {
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
        LM.write_log("Input_Manager::start_up(): Input_Manager started and key callback set.");
        std::cout << "Input_Manager started and key callback set." << std::endl;
        return 0;
    }

    // Shut down the Input_Manager
    void Input_Manager::shut_down() {
        // Remove the key callback by setting it to nullptr
        GLFWwindow* window = glfwGetCurrentContext();
        if (window) {
            glfwSetKeyCallback(window, nullptr);
            LM.write_log("Input_Manager::shut_down(): Input_Manager shut down and key callback removed.");
            std::cout << "Input_Manager shut down and key callback removed." << std::endl;
        }
    }

    // Update key states each frame
    void Input_Manager::update() {
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

    // Reset all key states
    void Input_Manager::reset() {
        key_states.clear();
        LM.write_log("Input_Manager states reset.");
        std::cout << "Input_Manager states reset." << std::endl;
    }

} // namespace lof
