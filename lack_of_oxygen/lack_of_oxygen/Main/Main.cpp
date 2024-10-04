/**
 * @file main.cpp
 * @brief Entry point of the game engine application.
 * @details Initializes the Game_Manager, loads configurations, sets up the window, and runs the main loop.
 * @author Simon Chan (89.516%), Liliana Hanawardani (10.483%)
 * @date September 21, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

 // Include file headers
#include "Main.h"

// Include standard headers
#include <thread>
#include <chrono>
#include <iostream>
#include <iomanip>    // For std::fixed and std::setprecision
#include <sstream>    // For std::stringstream

// Include for memory leaks
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

using namespace lof;



int main(void) {

    // --------------------------- Initialization ---------------------------

    // Enable debug heap allocations and automatic leak checking at exit
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // -------------------------- GLFW Initialization --------------------------

    // Initialize GLFW
    if (!glfwInit()) {
        LM.write_log("Failed to initialize GLFW!");
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }
    else {
        LM.write_log("GLFW initialized successfully.");
    }

    // Set OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Additional settings
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_RED_BITS, 8); glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8); glfwWindowHint(GLFW_ALPHA_BITS, 8);

    // --------------------------- Create GLFW Window ---------------------------

    // Create a windowed mode window and its OpenGL context using default values
    GLFWwindow* window = glfwCreateWindow(800, 600, "Lack Of Oxygen", NULL, NULL);
    if (!window) {
        LM.write_log("Failed to create GLFW window!");
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }
    else {
        LM.write_log("GLFW window created successfully with size %ux%u.", 800, 600);
        std::cout << "GLFW window created successfully with size 800x600." << std::endl;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Load OpenGL function pointers with GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LM.write_log("Failed to initialize OpenGL function pointers!");
        std::cerr << "Failed to initialize OpenGL function pointers!" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    else {
        LM.write_log("GLAD initialized successfully.");
        std::cout << "GLAD initialized successfully." << std::endl;
    }

    // --------------------------- Start Game_Manager ---------------------------

    // Initialize Game_Manager (which initializes Log_Manager, Config_Manager, ECS_Manager, FPS_Manager, Input_Manager)
    if (GM.start_up() != 0) {
        std::cerr << "Failed to start Game_Manager!" << std::endl;
        LM.write_log("Failed to start Game_Manager!");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    else {
        LM.write_log("Game_Manager started up successfully.");
        std::cout << "Game_Manager started up successfully." << std::endl;
    }

    // --------------------------- Retrieve Configuration ---------------------------

    // Retrieve configuration values from Config_Manager
    const unsigned int SCR_WIDTH = SM.get_scr_width();
    const unsigned int SCR_HEIGHT = SM.get_scr_height();
    const float FPS_DISPLAY_INTERVAL = SM.get_fps_display_interval();

    // Log the configuration values
    LM.write_log("Configuration Values - SCR_WIDTH: %u, SCR_HEIGHT: %u, FPS_DISPLAY_INTERVAL: %.2f",
        SCR_WIDTH, SCR_HEIGHT, FPS_DISPLAY_INTERVAL);
    std::cout << "Configuration Values - SCR_WIDTH: " << SCR_WIDTH
        << ", SCR_HEIGHT: " << SCR_HEIGHT
        << ", FPS_DISPLAY_INTERVAL: " << FPS_DISPLAY_INTERVAL << std::endl;

    // If the window size from Config_Manager differs from the created window, adjust it
    if (SCR_WIDTH != 800 || SCR_HEIGHT != 600) {
        glfwSetWindowSize(window, SCR_WIDTH, SCR_HEIGHT);
        LM.write_log("GLFW window size adjusted to %ux%u based on configuration.", SCR_WIDTH, SCR_HEIGHT);
        std::cout << "GLFW window size adjusted to " << SCR_WIDTH << "x" << SCR_HEIGHT << " based on configuration." << std::endl;
    }

    // -------------------------- Game Loop Setup --------------------------

    // Variables for FPS display
    float fps = 0.0f;
    float fps_timer = 0.0f;

    LM.write_log("Entering main game loop.");
    std::cout << "Entering main game loop." << std::endl;

    // Game loop
    while (!glfwWindowShouldClose(window) && !GM.get_game_over()) {
        // Start of frame timing
        FPSM.frame_start();

        // Get delta_time after frame_start()
        float delta_time = FPSM.get_delta_time(); // Get delta time in seconds

        // Get FPS
        fps = FPSM.get_current_fps();

        // Update window title with FPS
        std::stringstream ss;
        ss << "Lack Of Oxygen, FPS: " << std::fixed << std::setprecision(2) << fps;
        glfwSetWindowTitle(window, ss.str().c_str());
        
        // Update FPS timer
        fps_timer += delta_time;
        if (fps_timer >= FPS_DISPLAY_INTERVAL) {
            fps_timer = 0.0f;

            // Display FPS in console
            std::cout << "Current FPS: " << fps << std::endl;
        }

        // Poll for and process events 
        glfwPollEvents(); 

        // Getting delta time for Game Manager/game loop
        GM.set_time(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
        // Update game world state
        GM.update(delta_time);
        GM.set_time(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() - GM.get_time());

       
        // Logic for performance viewer, calls function to calculate and print the % of delta time of each system and manager
        if (IM.is_key_held(GLFW_KEY_T)) {
            system_performance(GM.get_time(), IM.get_time(), IM.get_type());
            system_performance(GM.get_time(), GFXM.get_time(), GFXM.get_type());
            system_performance(GM.get_time(), AM.get_time(), AM.get_type());
            system_performance(GM.get_time(), ECSM.get_time(), ECSM.get_type());
            std::cout << "In ECS Manager...\n";
            for (auto& system : ECSM.get_systems()) {
                system_performance(GM.get_time(), system->get_time(), system->get_type());
            }

            std::cout << std::endl;
        }


        // Check for game_over and set window should close flag
        if (GM.get_game_over()) {
            glfwSetWindowShouldClose(window, true);
            LM.write_log("Main Loop: game_over is true. Setting GLFW window to close.");
            std::cout << "Main Loop: game_over is true. Setting GLFW window to close." << std::endl;
        }

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // End of frame timing and FPS control
        FPSM.frame_end();
    }

    LM.write_log("Exiting main game loop.");
    std::cout << "Exiting main game loop." << std::endl;

    // --------------------------- Shutdown Sequence ---------------------------

    // Shutdown the Game_Manager (which shuts down all other managers)
    GM.shut_down();
    LM.write_log("Game_Manager shut down successfully.");
    std::cout << "Game_Manager shut down successfully." << std::endl;

    // Destroy the window
    glfwDestroyWindow(window);
    std::cout << "GLFW window destroyed." << std::endl;

    // Terminate GLFW
    glfwTerminate();
    std::cout << "GLFW terminated." << std::endl;

    // Application exit
    std::cout << "Application exited successfully." << std::endl;

    return 0;
}