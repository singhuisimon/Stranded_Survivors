/**
 * @file Audio_System.cpp
 * @brief Defining of the Window_Control class function for controlling game resolution.
 * @author Amanda Leow Boon Suan (100%)
 * @date November 20, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#include "../Utility/Win_Control.h"
#include "../Manager/Graphics_Manager.h"

namespace lof {

	Window_Control& Window_Control::get_instance() {
		static Window_Control instance;
		return instance;
	}

	Window_Control::Window_Control(int x, int y) : window_x(x), window_y(y), win_height(0), win_width(0) {}

	void Window_Control::toggle_fullscreen(GLFWwindow* window, GLFWmonitor* monitor, const GLFWvidmode* mode,
		bool& is_full_screen, unsigned int width, unsigned int height) {

		if (is_full_screen) {
			//change to windowed
			glfwSetWindowMonitor(window, nullptr, window_x, window_y, width, height, GLFW_DONT_CARE);
			update_win_size(window);
			LM.write_log("Window_Control::toggle_fullscreen: changing from fullscreen to windowed");
		}
		else {
			//change to fullscreen
			glfwGetWindowPos(window, &window_x, &window_y);			
			glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
			update_win_size(window);
			LM.write_log("Window_Control::toggle_fullscreen: changing from windowed to fullscreen");
		}

		is_full_screen = !is_full_screen;
	}

	void Window_Control::update_win_size(GLFWwindow* window) {
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		win_width = width;
		win_height = height;
		glViewport(0, 0, win_width, win_height);	//update the viewport
		LM.write_log("Window_Control::update_win_size: width set to %d, height set to %d", win_width, win_height);

#ifndef NDEBUG
		//for imgui_tex update <- this affect the viewport of game in imgui
		glBindTexture(GL_TEXTURE_2D, GFXM.get_framebuffer_texture());
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, win_width, win_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
#endif
	}

	unsigned int Window_Control::get_win_width() const {
		//LM.write_log("Window_Control::get_win_width: width is %ui", win_width);
		return win_width;
	}

	unsigned int Window_Control::get_win_height() const {
		//LM.write_log("Window_Control::get_win_height: height is %ui", win_height);
		return win_height;
	}

}