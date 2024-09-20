/**
 * @file main.cpp
 * @brief Entry point of the game engine application.
 * @details Initializes the game engine, sets up the window, and runs the main loop.
 * @author Simon Chan
 * @date September 16, 2024
 */

// Include file headers
#include "Main.h"

// Include standard headers
#include <thread>
#include <chrono>

using namespace lof;

// Settings
const unsigned int SCR_WIDTH = 800;         //< Screen width
const unsigned int SCR_HEIGHT = 600;        //< Screen height
const float FPS_DISPLAY_INTERVAL = 1.0f;    // Update FPS display every 1 second

/**
 * @brief Main function of the application.
 * @return int Exit status of the application.
 * @details Initializes GLFW, creates a window, sets up the ECS, and runs the main game loop.
 */
int main(void) {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    // Set OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lack Of Oxygen", NULL, NULL);
    if (!window) {
        LM.write_log("Failed to create GLFW window!");
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Load OpenGL function pointers with GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LM.write_log("Failed to initialize OpenGL function pointers!");
        glfwTerminate();
        return -1;
    }

    // Start up the Game_Manager
    if (GM.start_up() != 0) {
        LM.write_log("Failed to start Game_Manager!");
        glfwTerminate();
        return -1;
    }
    else {
        // Write a log message
        LM.write_log("Game engine started successfully.");
    }

    // Variables for FPS display
    float fps = 0.0f;
    float fps_timer = 0.0f;

    // Game loop
    while (!glfwWindowShouldClose(window) && !GM.get_game_over()) {
        // Start of frame timing
        FPSM.frame_start();

        // Get delta_time after frame_start()
        float delta_time = FPSM.get_delta_time(); // Get delta time in seconds

        // Update FPS timer
        fps_timer += delta_time;
        if (fps_timer >= FPS_DISPLAY_INTERVAL) {
            fps = FPSM.get_current_fps();
            fps_timer = 0.0f;

            // Display or log the FPS
            std::cout << "Current FPS: " << fps << std::endl;
        }

        // Get input (e.g., keyboard/mouse)
        glfwPollEvents();

        // Update game world state
        GM.update(delta_time);

        // Draw current scene to back buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // Here you would render your entities

        // Swap back buffer to current buffer
        glfwSwapBuffers(window);

        // End of frame timing and FPS control
        FPSM.frame_end();
    }

    // Shutdown the Game_Manager
    GM.shut_down();

    // Terminate GLFW
    glfwTerminate();
    return 0;
}