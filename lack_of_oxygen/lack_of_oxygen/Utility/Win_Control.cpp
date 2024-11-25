#include "..\Utility\Win_Control.h"

namespace lof {

	void Window_Control::toggle_fullscreen(GLFWwindow* window, GLFWmonitor* monitor, const GLFWvidmode* mode,
		bool& is_full_screen, unsigned int width, unsigned int height) {

		if (is_full_screen) {
			glfwSetWindowMonitor(window, nullptr, window_x, window_y, width, height, GLFW_DONT_CARE);
		}
		else {
			glfwGetWindowPos(window, &window_x, &window_y);
			glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
		}

		is_full_screen = !is_full_screen;
	}
}