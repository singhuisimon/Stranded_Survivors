/**
 * @file Interruption_System.h
 * @author Wai Lwin Thit (100%)
 * @date November 27, 2024
 * @brief System for handling window interruptions and game state management during focus loss events
 * @details Defines the Interruption_System class that manages window state transitions, focus events, and key combinations like
 * ALT+TAB and CTRL+ALT+DEL. 
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

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

		/**
		 * @brief Initializes the Interruption System with a window handle
		 * @param pointer to game window to monitor for state changes
		 * @details Sets up initial states, registers window callbacks, and initializes key tracking structures.
		 *          Stores initial window state for restoration.
		 */
		Interruption_System(GLFWwindow* window);
		~Interruption_System() = default;
		/**
		 * @brief Updates the system state each frame
		 * @param delta_time Time elapsed since last frame
		 * @details Checks key combinations and window state changes. Handles any pending
		 *          interruptions or restorations.
		 */		
		void update(float delta_time) override;
		/**
		 * @brief Returns system type identifier
		 * @return String identifying this as an Interruption_System
		 */
		std::string get_type() const override;
		/**
		 * @brief Returns current game pause state
		 * @return True if game is paused due to interruption
		 */
		bool is_game_paused() const;


		/**
		 * @brief Minimizes the window
		 * @details Triggers minimization handling if window isn't already minimized.
		 *          Updates state and triggers interruption handler.
		 */
		void minimize_window();
		/**
		 * @brief Restores window from minimized state
		 * @details Returns window to previous size/position.
		 *          Triggers restoration handler.
		 */
		void restore_window();



		//window state

	private:
		/**
		 * @brief Handles game interruption events
		 * @details Pauses game logic, resets input states, pauses audio,
		 *          and stores window state for later restoration.
		 */
		void handle_interruption();
		/**
		 * @brief Restores game state after interruption
		 * @details Restores window position/size, resumes audio playback,
		 *          resets input states, and resumes game logic.
		 */
		void handle_restoration();
		/**
		 * @brief Checks and updates current window state
		 * @details Monitors window visibility, focus state, and minimization.
		 *          Triggers appropriate handlers for state changes.
		 */
		void check_window_state();
		/**
		 * @brief Detects and tracks key combinations
		 * @details Monitors Control, Alt, and Delete keys for combinations.
		 *          Maintains key press history for reliable detection.
		 */
		void check_key_combinations();
		/**
		 * @brief Stores current window properties
		 * @details Saves window size, position, and fullscreen state
		 *          for restoration after minimization.
		 */
		void store_window_state();

		/**
		 * @brief Handles window iconification events
		 * @param window GLFW window that received the event
		 * @param iconified True if window was minimized, false if restored
		 */
		static void iconify_callback(GLFWwindow* window, int iconified);

		/**
		 * @brief Handles window focus events
		 * @param window GLFW window that received the event
		 * @param focused True if window gained focus, false if lost focus
		 * @details Manages interruptions from Alt+Tab, Ctrl+Alt+Del and fullscreen transitions.
		 */
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
