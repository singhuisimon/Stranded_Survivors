/**
 * @file Constant.h
 * @brief Defines constant values used across the project.
 * @author Simon Chan (92.307%), Wai Lwin Thit (11.538%), Liliana Hanawardani (11.538%), Amanda Leow Boon Suan (7.692)
 * @date October 01, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */
#pragma once
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

 // Include OpenGL headers
#define GLFW_INCLUDE_NONE
#include "../Glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm-0.9.9.8/glm/glm.hpp>
#include <glm-0.9.9.8/glm/gtc/type_ptr.hpp>

// Include FMOD headers
#include "fmod.hpp"
#include "fmod_studio.hpp"

namespace lof {

	// ------------------------------ Serialization_Manager.cpp --------------------------------
	// Screen Configuration Constants
	constexpr int DEFAULT_SCREEN_WIDTH = 800;
	constexpr int DEFAULT_SCREEN_HEIGHT = 600;

	// FPS Display Constants
	constexpr float DEFAULT_FPS_DISPLAY_INTERVAL = 1.0f;

	// ----------------------------- FPS_Manager.cpp -------------------------------------------
	// FPS_Manager Constants
	constexpr int DEFAULT_TARGET_FPS = 120;
	constexpr int64_t DEFAULT_MICROSECONDS_PER_SECOND = 1000000;
	constexpr int64_t DEFAULT_TARGET_TIME = DEFAULT_MICROSECONDS_PER_SECOND / DEFAULT_TARGET_FPS;

	constexpr int64_t DEFAULT_ADJUST_TIME = 0;
	constexpr float DEFAULT_DELTA_TIME = 0.0f;
	constexpr int64_t DEFAULT_LAST_FRAME_START_TIME = 0;

	// ------------------------- System.cpp and Manager.cpp Files -------------------------------
	// System Performance Constants
	constexpr int64_t DEFAULT_START_TIME = 0;

	// ----------------------------------- FPS.cpp ----------------------------------------------
	// System Performance Constants
	constexpr float UPON_PERCENTAGE = 100.0f;
	constexpr int DEBUG_LEFT_WIDTH = 17;
	constexpr int DEBUG_RIGHT_WIDTH = 5;

	// ------------------------------ Matrix3x3.cpp --------------------------------------------
	constexpr float PI_VALUE = 3.1415926f;
	constexpr float PI_VALUE_DEGREES = 180.0f;
	
	
	// -------------------------- Time Conversion Constants-----------------------------------
	constexpr int MILLISECONDS_PER_SECOND = 1000;
	constexpr int SECONDS_PER_MINUTE = 60;
	constexpr int MINUTES_PER_HOUR = 60;
	constexpr int HOURS_PER_DAY = 24;


	// ----------------------------- Component.h -------------------------------------------
	//Physics component constants 
	constexpr float DEFAULT_GRAVITY = -980.f;
	constexpr float DEFAULT_DAMPING_FACTOR = 0.9f;
	constexpr float DEFAULT_MAX_VELOCITY = 20000.0f;
	constexpr float DEFAULT_JUMP_FORCE = 2000.0f;

	// Graphics component constants
	constexpr const char* DEFAULT_MODEL_NAME = "square";
	constexpr glm::vec3 DEFAULT_COLOR = { 0.0f, 0.0f, 0.0f };
	constexpr unsigned int DEFAULT_SHADER_REF = 0;
	constexpr glm::mat3 DEFAULT_MDL_TO_NDC_MAT = { glm::mat3(0.0f) }; 

	// Animation component constants
	constexpr const char* DEFAULT_ANIMATION_IDX = "0";
	constexpr const char* DEFAULT_ANIMATION_NAME = "NoAnimation";

	//Audio component constants
	enum AudioType {
		BGM = 0,    ///<background music
		SFX = 1,		///<Sound Effect
		NIL = 2
	};

	enum PlayState {
		PLAYING = 0,    ///<Audio is to be played
		STOPPED = 1,    ///<Audio is to be stopped
		PAUSED = 2,     ///<Audio is to be paused
		RESUMED = 3,	///<Audio is to be resumed
		NONE = 4		///<Audio is not played at all nor need to stop
	};

	// ----------------------------- Movement_System.cpp -------------------------------------------
    //Movement_System constants

	constexpr float DEFAULT_SPEED = 1000.0f;
	constexpr float GRAVITY_ACCELERATOR = 10.0f;

	// ------------------------------ Audio_Manager.cpp --------------------------------

	constexpr int TRACK1 = 1;
	constexpr int TRACK2 = 2;

	// ------------------------------ Graphics_Manager.cpp -----------------------------
	constexpr unsigned int DEFAULT_FRAME_INDEX = 0;
	constexpr float DEFAULT_TEXTURE_SIZE = 254.0f;

	// Camera
	constexpr float DEFAULT_CAMERA_SPEED = 500.0f;
	constexpr float DEFAULT_CAMERA_POS_Y = 0.0f;
	constexpr float DEFAULT_ORIENTATION = 0.0f;

	// Fonts
	constexpr int DEFAULT_GLYPH_HEIGHT = 48;
	constexpr int DEFAULT_GLYPH_WIDTH = 0;
	constexpr const char* DEFAULT_FONT_NAME = "PressStart2P";
	constexpr float DEFAULT_TEXT_POSITION = 0.0f;

	// ------------------------------ Render_System.cpp --------------------------------
	// Drawing constants
	constexpr GLfloat DEFAULT_ROTATION = 90.0f;
	constexpr GLfloat DEFAULT_LINE_WIDTH = 5.0f;
	constexpr GLfloat DEFAULT_POINT_SIZE = 5.0f;
	constexpr const char* DEFAULT_TEXTURE_NAME = "NoTexture";
	
	// Debugging constants
	constexpr float DEFAULT_SCALE_CHANGE = 100.0f;
	constexpr GLfloat DEFAULT_AABB_WIDTH = 2.0f;
	constexpr GLfloat DEFAULT_VELOCITY_LINE_LENGTH = 1.5f;

	// ------------------------------ Animation_System.cpp --------------------------------
	constexpr float DEFAULT_FRAME_TIME_ELAPSED = 0.0f;
	constexpr float DEFAULT_Y_OFFSET = 1.0f;

	// ------------------------------ Collision_System.cpp --------------------------------
	constexpr const float         BOUNDING_RECT_SIZE = 1.0f;
	constexpr const unsigned int	COLLISION_LEFT = 0x00000001;	//0001
	constexpr unsigned int	COLLISION_RIGHT = 0x00000002;	//0010
	constexpr const unsigned int	COLLISION_TOP = 0x00000004;	//0100
	constexpr const unsigned int	COLLISION_BOTTOM = 0x00000008;	//1000

	// -------------------------- Common variables used in Systems -----------------------------------
	constexpr char const* DEFAULT_PLAYER_NAME = "player1";

	// ------------------------------ GUI_System.cpp --------------------------------
	// GUI Layout Constants
	constexpr float DEFAULT_GUI_PROGRESS_BAR_WIDTH = 450.0f;
	constexpr float DEFAULT_GUI_PROGRESS_BAR_HEIGHT = 40.0f;
	constexpr float DEFAULT_GUI_IMAGE_OFFSET = 150.0f;
	constexpr float DEFAULT_GUI_IMAGE_SIZE = 100.0f;
	constexpr float DEFAULT_GUI_VERTICAL_SPACING = 50.0f;

} // namespace lof

#endif // CONSTANTS_H
