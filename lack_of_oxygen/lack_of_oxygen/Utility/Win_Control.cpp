#include "..\Utility\Win_Control.h"

namespace lof {

	Window_Control::Window_Control(int x, int y) : window_x(x), window_y(y), win_height(0), win_width(0) {}

	void Window_Control::toggle_fullscreen(GLFWwindow* window, GLFWmonitor* monitor, const GLFWvidmode* mode,
		bool& is_full_screen, unsigned int width, unsigned int height) {

		if (is_full_screen) {
			glfwSetWindowMonitor(window, nullptr, window_x, window_y, width, height, GLFW_DONT_CARE);
			set_win_size(width, height);
			LM.write_log("Window_Control::toggle_fullscreen: changing from fullscreen to windowed");
		}
		else {
			glfwGetWindowPos(window, &window_x, &window_y);
			glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
			set_win_size(width, height);
			LM.write_log("Window_Control::toggle_fullscreen: changing from windowed to fullscreen");
		}

		is_full_screen = !is_full_screen;
	}

	void Window_Control::set_win_size(unsigned int width, unsigned int height) {
		win_width = width;
		win_height = height;
		LM.write_log("Window_Control::set_win_size: width is set from %ui to %ui, height is set from %ui to %ui", width, win_width, height, win_height);
	}

	unsigned int Window_Control::get_win_width() const {
		LM.write_log("Window_Control::get_win_width: width is %ui", win_width);
		return win_width;
	}

	unsigned int Window_Control::get_win_height() const {
		LM.write_log("Window_Control::get_win_width: height is %ui", win_height);
		return win_height;
	}

}