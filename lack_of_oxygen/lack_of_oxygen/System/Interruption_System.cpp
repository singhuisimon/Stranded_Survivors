#if 0
#include "Interruption_System.h"

namespace lof {

	Interruption_System::Interruption_System(GLFWwindow* window) : is_minimized(false), is_paused(false), window(window),
	prev_window_state(WindowState::NORMAL) {
		glfwSetWindowIconifyCallback(window, iconify_callback);

		LM.write_log("Interruption_System: Constructed.");
	}

	void Interruption_System::update(float delta_time) {
		(void)delta_time; 

	}

	std::string Interruption_System::get_type() const {
		return "Interruption_System";
	}

/*
	//private functions
	void Interruption_System::handle_gained_focus() {

	}

	void Interruption_System::handle_lost_focus() {

	}
*/

	 void Interruption_System:: iconify_callback(GLFWwindow* window, int iconified) {

		//find the system instance 
		for (auto const& system : ECSM.get_systems()) {
			if (system->get_type() == "Interruption_System") {
				auto* interrupt_system = static_cast<Interruption_System*>(system.get());

				if (iconified) {
					interrupt_system->minimize_window();
				}

				else interrupt_system->restore_window();

				break;
			}
		}
	}

	void Interruption_System::minimize_window() {
		if (!is_minimized) {
			is_minimized = true; 
			glfwIconifyWindow(window); //minimize the window
			LM.write_log("Interruption_System: Window Minimized.");
		}
	}
	void Interruption_System::restore_window() {
		if (is_minimized) {
			is_minimized = false;
			glfwRestoreWindow(window); //restore the window
			LM.write_log("Interruption_System: Window Restored.");
		}
	}

}
#endif


#if 1 
#include "Interruption_System.h"
#include "../Manager/Input_Manager.h"

namespace lof {

    Interruption_System::Interruption_System(GLFWwindow* window)
        : is_minimized(false)
        , is_paused(false)
        , focus_lost(false)
        , window(window)
        , previous_state(WindowState::NORMAL)
        , current_state(WindowState::NORMAL)
        , was_fullscreen(false)
        , prev_width(DEFAULT_SCREEN_WIDTH)
        , prev_height(DEFAULT_SCREEN_HEIGHT)
        , prev_x(0)
        , prev_y(0)
        {

        store_window_state(); 

        // Set up window callbacks
        glfwSetWindowIconifyCallback(window, iconify_callback);
        glfwSetWindowFocusCallback(window, focus_callback);

        LM.write_log("Interruption_System: Initialized with callbacks.");
    }

    void Interruption_System::update(float delta_time) {
        static WindowState previous_logged_state = current_state;

        // Check current window state
        check_window_state();

        // Only handle state changes if the state has actually changed
        if (current_state != previous_state) {
            // Log state transition if it's different from last logged state
            if (current_state != previous_logged_state) {
                LM.write_log("Interruption_System: State changed from %d to %d",
                    static_cast<int>(previous_logged_state),
                    static_cast<int>(current_state));
                previous_logged_state = current_state;
            }

            if (current_state == WindowState::INTERRUPTED ||
                current_state == WindowState::MINIMIZED) {
                handle_interruption();
            }
            else {
                handle_restoration();
            }
            previous_state = current_state;
        }
    }
    void Interruption_System::check_window_state() {
        bool is_visible = glfwGetWindowAttrib(window, GLFW_VISIBLE);
        bool is_focused = glfwGetWindowAttrib(window, GLFW_FOCUSED);
        bool is_iconified = glfwGetWindowAttrib(window, GLFW_ICONIFIED);

        //If window is visible and not minimized, store its current state 
        if (is_visible && !is_iconified && is_focused) {
            store_window_state(); 
            LM.write_log("Interruption_System: Stored window state - Fullscreen: %d, Size: %dx%d, Pos: (%d,%d)",
                was_fullscreen, prev_width, prev_height, prev_x, prev_y);
        }


        // Add debug logging to understand window state
        LM.write_log("Interruption_System: Window State Debug - Visible: %d, Focused: %d, Iconified: %d",
            is_visible, is_focused, is_iconified);

        // Only consider it interrupted if 
        //1. Window is actually minimized (iconified)
        //2. Window becomes invisible

/*
        if (!is_focused && !is_iconified && is_visible) {
            glfwIconifyWindow(window);
            LM.write_log("Interruption_System: Auto-minimizing window due to focus check");
            return;
        }
*/
        
        if (!is_visible  || is_iconified) {
            if (!is_minimized) {  // Only trigger once when first minimized
                current_state = WindowState::MINIMIZED; 
                is_minimized = true;
                handle_interruption();
                LM.write_log("Interruption_System: True minimization detected");
            }
        }
        // Handle window restoration
        else if (is_visible && !is_iconified) {
            if (is_minimized) {  // Only log when recovering from lost focus
                current_state = WindowState::NORMAL;
                is_minimized = false;
                handle_restoration();
                LM.write_log("Interruption_System: Window restored from minimization");
            }

            //update focus state 
            if (is_focused) {
                current_state = WindowState::NORMAL;
                focus_lost = false;
            }
        }
    }
    void Interruption_System::handle_interruption() {
        
        //store previous state 
        previous_state = current_state;

        // Pause game logic
        is_paused = true;

        // Reset input states to prevent stuck keys
        IM.reset();

        // Pause audio (assuming you have an Audio_Manager)
        // AM.pause_all();

        LM.write_log("Interruption_System: Game interrupted - Input reset, Audio paused");
    }

    void Interruption_System::handle_restoration() {
        
        if (is_paused) {
            //first restore the window 
            glfwRestoreWindow(window);

            //check if it was a full screen before 
            if (was_fullscreen) {
                glfwMaximizeWindow(window);
            }
            else {
                //restore previous window position and size
                glfwSetWindowPos(window, prev_x, prev_y); 
                glfwSetWindowSize(window, prev_width, prev_height);
            }
        }

        // Reset input states again to ensure clean state
        IM.reset();

        //pause game logic
        is_paused = false;

        LM.write_log("Interruption_System: Game restored - window restored, input reset");
    }

    void Interruption_System::iconify_callback(GLFWwindow* window, int iconified) {
        for (auto const& system : ECSM.get_systems()) {
             if (auto* interrupt_system = dynamic_cast<Interruption_System*>(system.get())) {

                  if (iconified) {
                        //interrupt_system->current_state = WindowState::MINIMIZED;
                        interrupt_system->minimize_window();
                        LM.write_log("Interruption_System: Window minimized via iconify");
                  }
                  else {
                        //interrupt_system->current_state = WindowState::NORMAL;
                        interrupt_system->restore_window();
                        LM.write_log("Interruption_System: Window restored via iconify");
                  }
                  break;
            
                }
             }
        }


    void Interruption_System::focus_callback(GLFWwindow* window, int focused) {
        for (auto const& system : ECSM.get_systems()) {
            if (auto* interrupt_system = dynamic_cast<Interruption_System*>(system.get())) {
                LM.write_log("Interruption_System: Focus callback - focused: %d", focused);
#if 0
                if (!focused) {
                    // Store the current window state before minimizing
                    interrupt_system->store_window_state();

                    // Check for ALT+TAB using Input Manager
                    bool alt_pressed =
                        IM.is_key_held(GLFW_KEY_LEFT_ALT) ||
                        IM.is_key_held(GLFW_KEY_RIGHT_ALT);

                    // Check for CTRL+ALT+DEL using Input Manager
                    bool ctrl_alt_del =
                        (IM.is_key_held(GLFW_KEY_LEFT_CONTROL) ||
                            IM.is_key_held(GLFW_KEY_RIGHT_CONTROL)) &&
                        (IM.is_key_held(GLFW_KEY_LEFT_ALT) ||
                            IM.is_key_held(GLFW_KEY_RIGHT_ALT)) &&
                        IM.is_key_held(GLFW_KEY_DELETE);

                    // Only minimize for ALT+TAB or CTRL+ALT+DEL
                    if (alt_pressed || ctrl_alt_del) {
                        glfwIconifyWindow(window);
                        interrupt_system->focus_lost = true;
                        LM.write_log("Interruption_System: Auto-minimizing from ALT+TAB or CTRL+ALT+DEL");
                        // Log stored state
                        LM.write_log("Interruption_System: Stored state - Fullscreen: %d, Size: %dx%d, Pos: (%d,%d)",
                            interrupt_system->was_fullscreen,
                            interrupt_system->prev_width,
                            interrupt_system->prev_height,
                            interrupt_system->prev_x,
                            interrupt_system->prev_y);
                    }
                }
#endif
                if (!focused) {
                    // Store the current window state before minimizing
                    interrupt_system->store_window_state();

                    // Force minimize when focus is lost in fullscreen
                    if (glfwGetWindowAttrib(window, GLFW_MAXIMIZED)) {
                        glfwIconifyWindow(window);
                        interrupt_system->focus_lost = true;
                        LM.write_log("Interruption_System: Force minimizing fullscreen window on focus loss");
                    }
                    else {
                        // For windowed mode, check for ALT+TAB using Input Manager
                        bool alt_pressed =
                            IM.is_key_held(GLFW_KEY_LEFT_ALT) ||
                            IM.is_key_held(GLFW_KEY_RIGHT_ALT);

                        // Only minimize for ALT+TAB in windowed mode
                        if (alt_pressed) {
                            glfwIconifyWindow(window);
                            interrupt_system->focus_lost = true;
                            LM.write_log("Interruption_System: Auto-minimizing from ALT+TAB");
                        }
                    }

                    // Log stored state
                    LM.write_log("Interruption_System: Stored state - Fullscreen: %d, Size: %dx%d, Pos: (%d,%d)",
                        interrupt_system->was_fullscreen,
                        interrupt_system->prev_width,
                        interrupt_system->prev_height,
                        interrupt_system->prev_x,
                        interrupt_system->prev_y);
                }

                break;
            }
        }
    }

    void Interruption_System::minimize_window() {
        if (!is_minimized) {
            is_minimized = true;
            handle_interruption();
            //glfwIconifyWindow(window);
            LM.write_log("Interruption_System: Window Minimized");
        }
    }

    void Interruption_System::restore_window() {
        if (is_minimized) {
            is_minimized = false;
            handle_restoration();
            LM.write_log("Interruption_System: Window Restored");
        }
    }

    void Interruption_System::store_window_state() {
        //store if window was full screen
        was_fullscreen = glfwGetWindowAttrib(window, GLFW_MAXIMIZED);

        //store window size and position
        glfwGetWindowSize(window, &prev_width, &prev_height); 
        glfwGetWindowPos(window, &prev_x, &prev_y);
    }

    bool Interruption_System::is_game_paused() const {
        return is_paused;
    }

    std::string Interruption_System::get_type() const {
        return "Interruption_System";
    }
}
#endif