/**
 * @file Win_Control.h
 * @brief Declaration of the Window_Control class for controlling game resolution.
 * @author Amanda Leow Boon Suan (100%)
 * @date November 20, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */
#pragma once

#ifndef _WIN_CONTROL_H_
#define _WIN_CONTROL_H_
#define WC lof::Window_Control::get_instance() ///< Shortcut for accessing the singleton instance of Window_Control.

//Include necessary header files
#include "../Manager/Log_Manager.h"

namespace lof {

	/**
	* @class Window_Control
	* @brief Singleton class for managing game window properties such as windowed and fullscreen mode.
	*/
	class Window_Control {
	public:

		/**
		* @brief Retrieves the singleton instance of Window_Control
		* @return Reference to the Window_Control instance.
		*/
		static Window_Control& get_instance();

		/**
		* @brief Deleted copy constructor to prevent copying the singleton
		*/
		Window_Control(const Window_Control&) = delete;

		/**
		* @brief Deleted assignment operator to prevent assigning the singleton instance
		*/
		Window_Control& operator=(const Window_Control&) = delete;

		/**
		* @brief Toggles between fullscreen and windowed mode.
		* @param window Pointer to the current GLFW window.
		* @param monitor Pointer to the current GLFW monitor for fullscreen mode.
		* @param mode Pointer to the GLFW video mode settings for the monitor.
		* @param is_full_screen Reference to the boolean tracking fullscreen state.
		* @param width The width of the window in windowed mode.
		* @param height The height of the window in windowed mode.
		*/
		void toggle_fullscreen(GLFWwindow* window, GLFWmonitor* monitor, const GLFWvidmode* mode,
			bool& is_full_screen, unsigned int width, unsigned int height);

		void update_win_size(GLFWwindow* window);

		unsigned int get_win_width() const;

		unsigned int get_win_height() const;

	private:

		Window_Control(int x = 200, int y = 200);

		int window_x;
		int window_y;
		unsigned int win_height;
		unsigned int win_width;
	};

}

#endif