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
const unsigned int SCR_WIDTH = 800;   ///< Screen width
const unsigned int SCR_HEIGHT = 600;  ///< Screen height
const int TARGET_FPS = 60;
const int64_t MICROSECONDS_PER_SECOND = 1000000;
const int64_t TARGET_TIME = MICROSECONDS_PER_SECOND / TARGET_FPS; // Target frame time in microseconds

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

    // Write a log message
    LM.write_log("Game engine started successfully.");

    // Game loop setup
    Clock clock;
    int64_t adjust_time = 0;

    // Game loop
    while (!glfwWindowShouldClose(window) && !GM.get_game_over()) {
        clock.delta(); // Start of frame timing

        // Get input (e.g., keyboard/mouse)
        glfwPollEvents();

        // Frame rate control
        // Measure the time taken for the main loop to complete so far
        int64_t loop_time = clock.split();

        // Calculate how long the program should sleep to maintain the target frame time.
        int64_t intended_sleep_time = TARGET_TIME - loop_time - adjust_time;

        // If the calculated sleep time is positive, the program will sleep for that duration
        if (intended_sleep_time > 0) {
            Clock::sleep(intended_sleep_time);
        }

        // Measure the actual sleep time by splitting the clock again after sleep
        // and subtracting the loop time from it.
        int64_t actual_sleep_time = clock.split() - loop_time;

        // Adjust the sleep time for the next frame, compensating for any discrepancy
        // between the intended sleep time and the actual sleep time.
        adjust_time = actual_sleep_time - intended_sleep_time;

        // Ensure that adjust_time does not become negative, as negative values
        // would cause problems in future frame timing calculations.
        if (adjust_time < 0) adjust_time = 0;

        // Update game world state
        float delta_time = static_cast<float>(clock.split()) / 1000000.0f; // Convert to seconds
        GM.update(delta_time);

        // Draw current scene to back buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // Here you would render your entities

        // Swap back buffer to current buffer
        glfwSwapBuffers(window);
    }

    // Shutdown the Game_Manager
    GM.shut_down();

    // Terminate GLFW
    glfwTerminate();
    return 0;
}