/**
 * @file Constant.h
 * @brief Defines constant values used across the project.
 * @date October 01, 2024
 * Copyright (C) 20xx DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */
#pragma once
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

namespace lof {

	// ------------------------------ Serialization_Manager.cpp --------------------------------
	// Data Directory Constants
	// constexpr const char* BASE_DATA_DIR = "..\\..\\lack_of_oxygen\\Data\\";
	constexpr const char* CONFIG_PATH = "..\\..\\lack_of_oxygen\\Data\\..\\lack_of_oxygen\\Data\\config.json";
	constexpr const char* PREFABS_PATH = "..\\..\\lack_of_oxygen\\Data\\..\\lack_of_oxygen\\Data\\prefab.json";
	constexpr const char* SCENE_PATH = "..\\..\\lack_of_oxygen\\Data\\..\\lack_of_oxygen\\Data\\scene1.scn";

	// Screen Configuration Constants
	constexpr int DEFAULT_SCREEN_WIDTH = 800;
	constexpr int DEFAULT_SCREEN_HEIGHT = 600;

	// FPS Display Constants
	constexpr float DEFAULT_FPS_DISPLAY_INTERVAL = 1.0f;

	// ----------------------------- FPS_Manager.cpp -------------------------------------------
	// FPS_Manager Constants
	constexpr int DEFAULT_TARGET_FPS = 60;
	constexpr int64_t DEFAULT_MICROSECONDS_PER_SECOND = 1000000;
	constexpr int64_t DEFAULT_TARGET_TIME = DEFAULT_MICROSECONDS_PER_SECOND / DEFAULT_TARGET_FPS;

	constexpr int64_t DEFAULT_ADJUST_TIME = 0;
	constexpr float DEFAULT_DELTA_TIME = 0.0f;
	constexpr int64_t DEFAULT_LAST_FRAME_START_TIME = 0;

	// Time Conversion Constants
	constexpr int MILLISECONDS_PER_SECOND = 1000;
	constexpr int SECONDS_PER_MINUTE = 60;
	constexpr int MINUTES_PER_HOUR = 60;
	constexpr int HOURS_PER_DAY = 24;


	// ----------------------------- Component.h -------------------------------------------
	//Physics component constants 
	constexpr float DEFAULT_GRAVITY = -980.f;
	constexpr float DEFAULT_DAMPING_FACTOR = 0.9f;
	constexpr float DEFAULT_MAX_VELOCITY = 30000.0f;
	constexpr float DEFAULT_JUMP_FORCE = 2000.0f;


	// ----------------------------- Movement_System.cpp -------------------------------------------
    //Movement_System constants

	constexpr float DEFAULT_SPEED = 1000.0f;
	constexpr float GRAVITY_ACCELERATOR = 10.0f;

	// ------------------------------ Audio_Manager.cpp --------------------------------
	// Music Wav Directory
	constexpr const char* MUSICPATH1 = "..\\..\\lack_of_oxygen\\Data\\..\\lack_of_oxygen\\Data\\audio_test1.wav";
	constexpr const char* MUSICPATH2 = "..\\..\\lack_of_oxygen\\Data\\..\\lack_of_oxygen\\Data\\audio_test2.wav";

	constexpr int TRACK1 = 1;
	constexpr int TRACK2 = 2;

} // namespace lof

#endif // CONSTANTS_H
