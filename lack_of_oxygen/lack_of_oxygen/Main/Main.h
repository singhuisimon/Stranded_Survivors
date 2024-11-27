/**
 * @file Main.h
 * @brief Main header file for the game engine application.
 * @details Includes necessary libraries and headers for the application entry point.
 * @author Simon Chan (90.909%), Liliana Hanawardani (9.090%)
 * @date September 21, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#ifndef LOF_MAIN_H
#define LOF_MAIN_H

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
#include "../System/Movement_System.h"
#include "../Manager/ECS_Manager.h"

// Include Utility headers
#include "../Utility/Clock.h"
#include "../Utility/Vector2D.h"
#include "../Utility/Matrix3x3.h"
#include "../Utility/Globals.h"
#include "../Utility/Win_Control.h"

// Include Manager headers
#include "../Manager/Game_Manager.h"
#include "../Manager/Log_Manager.h"
#include "../Manager/FPS_Manager.h"
#include "../Manager/Serialization_Manager.h"
#include "../Manager/Graphics_Manager.h"
//#include "../Manager/IMGUI_Manager.h"
//
//#include "../IMGUI/imgui.h"
//#include "../IMGUI/imgui_impl_glfw.h"
//#include "../IMGUI/imgui_impl_opengl3.h"

extern GLFWwindow* window;
#endif // LOF_MAIN_H