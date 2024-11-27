
#pragma once

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
		bool is_game_paused() const;



		void minimize_window();
		void restore_window();


		void handle_audio();


		//window state

	private:
		void handle_interruption();
		void handle_restoration();
		void check_window_state();
		void check_key_combinations();
		void store_window_state();

		// Callbacks
		static void iconify_callback(GLFWwindow* window, int iconified);
		static void focus_callback(GLFWwindow* window, int focused);

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

		struct KeyStates {
			bool ctrl; 
			bool alt; 
			bool del; 
			bool ctrl_was_pressed;
			bool alt_was_pressed;
			bool del_was_pressed;
			
		};

		KeyStates current_keys; 
		KeyStates previous_keys;

		bool alt_tab_active; 
		bool ctrl_alt_del_active;
		bool force_minimize;
	};
}
#endif



#endif