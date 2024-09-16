/**
 * @file Main.h
 * @brief Main header file for the game engine application.
 * @details Includes necessary libraries and headers for the application entry point.
 * @author Simon Chan
 * @date September 15, 2024
 */

#ifndef LOF_MAIN_H
#define LOF_MAIN_H

 // Include GLAD then GLFW (in this order)
#define GLFW_INCLUDE_NONE
#include "../Glad/glad.h"
#include <GLFW/glfw3.h>

// Include standard headers
#include <iostream>
#include <memory>
#include <vector>
#include <array>
#include <unordered_map>

// Include ECS headers
#include "../Entity/Entity.h"
#include "../Component/Component.h"
#include "../System/System.h"
#include "../Manager/ECS_Manager.h"
#include "../System/Movement_System.h"

// Include Utility headers
#include "../Utility/Clock.h"

// Include Manager headers
#include "../Manager/Log_Manager.h"
#include "../Manager/Game_Manager.h"

#endif // LOF_MAIN_H