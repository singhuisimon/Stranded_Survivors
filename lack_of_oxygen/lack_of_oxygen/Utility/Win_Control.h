
#pragma once

#ifndef _WIN_CONTROL_H_
#define _WIN_CONTROL_H_

#include "../Manager/Log_Manager.h"

namespace lof {

	class Window_Control {
	public:

		Window_Control(int x = 200, int y = 200);

		void toggle_fullscreen(GLFWwindow* window, GLFWmonitor* monitor, const GLFWvidmode* mode,
			bool& is_full_screen, unsigned int width, unsigned int height);

		void set_win_size(unsigned int width, unsigned int height);

		unsigned int get_win_width() const;

		unsigned int get_win_height() const;

	private:
		int window_x;
		int window_y;
		unsigned int win_height;
		unsigned int win_width;
	};

}

#endif