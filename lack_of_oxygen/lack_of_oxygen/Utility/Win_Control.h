#ifndef _WIN_CONTROL_H_
#define _WIN_CONTROL_H_

#include <GLFW/glfw3.h>

namespace lof {

	class Window_Control {
	public:
		void toggle_fullscreen(GLFWwindow* window, GLFWmonitor* monitor, const GLFWvidmode* mode,
			bool& is_full_screen, unsigned int width, unsigned int height);
	private:
		int window_x = 200;
		int window_y = 200;
	};

}

#endif