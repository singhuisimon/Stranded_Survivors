
#pragma once
#if 0
#ifndef  LOF_INTERRUPTION_SYSTEM_H
#define LOF_INTERRUPTION_SYSTEM_H

#include "../System/System.h"
#include <GLFW/glfw3.h>
#include "../Manager/Log_Manager.h"
#include "../Manager/ECS_Manager.h"


namespace lof {
	class Interruption_System : public System {

	public:
		enum class WindowState {
			NORMAL, 
			MINIMIZED
		};

		Interruption_System(GLFWwindow* window); 
		void update(float delta_time) override;
		std::string get_type() const override; 

		~Interruption_System() = default;

		void minimize_window(); 
		void restore_window();

		//call back
		static void iconify_callback(GLFWwindow* window, int iconified);
		

	private:
		/*
		void handle_lost_focus();
		void handle_gained_focus();
		*/
	
	private: //data members 
		bool is_minimized; 
		bool is_paused;
		GLFWwindow* window;
		WindowState prev_window_state;
	};
} // ! namespace lof
#endif // ! LOF_INTERRUPTION_SYSTEM_H
#endif

#if 1
#pragma once
#ifndef LOF_INTERRUPTION_SYSTEM_H
#define LOF_INTERRUPTION_SYSTEM_H

#include "../System/System.h"
#include <GLFW/glfw3.h>
#include "../Manager/Log_Manager.h"
#include "../Manager/ECS_Manager.h"

namespace lof {
	class Interruption_System : public System {
	public:
		enum class WindowState {
			NORMAL,
			MAXIMIZED,
			MINIMIZED,
			INTERRUPTED
		};

		//constructor and destructor
		Interruption_System(GLFWwindow* window);
		~Interruption_System() = default;

		void update(float delta_time) override;
		std::string get_type() const override;

		void minimize_window();
		void restore_window();
		bool is_game_paused() const;

		void check_key_combinations();
		void handle_audio();

		// Callbacks
		static void iconify_callback(GLFWwindow* window, int iconified);
		static void focus_callback(GLFWwindow* window, int focused);

		//window state
		void store_window_state();

	private:
		void handle_interruption();
		void handle_restoration();
		void check_window_state();

	private:
		bool is_minimized;
		bool is_paused;
		bool focus_lost;
		GLFWwindow* window;
		WindowState previous_state;
		WindowState current_state;

		//to track the window state 
		bool was_fullscreen;
		int prev_height;
		int prev_width;
		int prev_x; 
		int prev_y;

		bool alt_tab; 
		bool ctrl_alt_del;
		bool prev_alt_tab = false;
		bool prev_ctrl_alt_del = false;

	};
}
#endif
#endif