
#pragma once

#ifndef _WIN_CONTROL_H_
#define _WIN_CONTROL_H_
#define WC lof::Window_Control::get_instance()

#include "../Manager/Log_Manager.h"

namespace lof {

	class Window_Control {
	public:

		static Window_Control& get_instance();

		Window_Control(const Window_Control&) = delete;
		Window_Control& operator=(const Window_Control&) = delete;

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