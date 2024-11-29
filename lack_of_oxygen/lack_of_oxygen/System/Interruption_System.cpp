/**
 * @file Interruption_System.cpp
 * @author Wai Lwin Thit (100%)
 * @date November 27, 2024
 * @brief Implementation of Interruption_System methods
 * @details Implements window state management, key combination detection, and interrupt handling
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#include "Interruption_System.h"
#include "../Manager/Input_Manager.h"
#include "Audio_System.h"

namespace lof {

    Interruption_System::Interruption_System(GLFWwindow* window)
        : is_minimized(false)
        , is_paused(false)
        , focus_lost(false)
        , window(window)
        , previous_state(WindowState::NORMAL)
        , current_state(WindowState::NORMAL)
        , was_fullscreen(false)
        , prev_width(0)
        , prev_height(0)
        , prev_x(0)
        , prev_y(0)
        , alt_tab_active(false)
        , ctrl_alt_del_active(false)
        , force_minimize(false)
        {

        //Initialize key states 
        current_keys = { false, false, false, false, false, false };
        previous_keys = { false, false, false, false, false, false };

        store_window_state(); 

        // Set up window callbacks
        glfwSetWindowIconifyCallback(window, iconify_callback);
        glfwSetWindowFocusCallback(window, focus_callback);

        LM.write_log("Interruption_System: Initialized with callbacks.");
    }

    void Interruption_System::update(float delta_time) {
        (void)delta_time;

        check_key_combinations();

        // Check current window state
        check_window_state();

    }

    void Interruption_System::check_window_state() {
        bool is_visible = glfwGetWindowAttrib(window, GLFW_VISIBLE);
        bool is_focused = glfwGetWindowAttrib(window, GLFW_FOCUSED);
        bool is_iconified = glfwGetWindowAttrib(window, GLFW_ICONIFIED);

        //If window is visible and not minimized, store its current state 
        if (is_visible && !is_iconified && is_focused) {
            store_window_state(); 
            //LM.write_log("Interruption_System: Stored window state - Fullscreen: %d, Size: %dx%d, Pos: (%d,%d)",
            //    was_fullscreen, prev_width, prev_height, prev_x, prev_y);
        }


        // Add debug logging to understand window state
        //LM.write_log("Interruption_System: Window State Debug - Visible: %d, Focused: %d, Iconified: %d",
        //    is_visible, is_focused, is_iconified);
        
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

        if (!is_paused) { //only handle if not paused

            //store previous state 
            previous_state = current_state;

            // Pause game logic
            is_paused = true;

            // Reset input states to prevent stuck keys
            IM.reset();

            // Pause audio
            for (auto const& system : ECSM.get_systems()) {
                if (auto* audio_system = dynamic_cast<Audio_System*>(system.get())) {
                    audio_system->pause_resume_mastergroup();
                    LM.write_log("Interruption_System: Audio paused");
                    break;
                }
            }

            LM.write_log("Interruption_System: Game interrupted - Input reset, Audio paused");
        }
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

            // Resume audio
            for (auto const& system : ECSM.get_systems()) {
                if (auto* audio_system = dynamic_cast<Audio_System*>(system.get())) {
                    audio_system->pause_resume_mastergroup();
                    LM.write_log("Interruption_System: Audio resumed");
                    break;
                }
            }
        // Reset input states again to ensure clean state
        IM.reset();

        //pause game logic
        is_paused = false;

        LM.write_log("Interruption_System: Game restored - window restored, input reset");
        }
    }

    void Interruption_System::iconify_callback(GLFWwindow* window, int iconified) {
        for (auto const& system : ECSM.get_systems()) {
             if (auto* interrupt_system = dynamic_cast<Interruption_System*>(system.get())) {

                  if (iconified) {
                        //interrupt_system->current_state = WindowState::MINIMIZED;
                        glfwIconifyWindow(window);
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

#if 0
    void Interruption_System::focus_callback(GLFWwindow* window, int focused) {
        for (auto const& system : ECSM.get_systems()) {
            if (auto* interrupt_system = dynamic_cast<Interruption_System*>(system.get())) {
                LM.write_log("Interruption_System: Focus callback - focused: %d", focused);

                if (!focused) {
                    // Store the current window state before minimizing
                    interrupt_system->store_window_state();


                    //check for ALT+TAB
                    bool alt_pressed =
                        IM.is_key_held(GLFW_KEY_LEFT_ALT) ||
                        IM.is_key_held(GLFW_KEY_RIGHT_ALT);

                    //check for CTRL+ALT+DEL
                    bool ctrl_alt_del =
                        (IM.is_key_held(GLFW_KEY_LEFT_CONTROL) ||
                            IM.is_key_held(GLFW_KEY_RIGHT_CONTROL)) &&
                        (IM.is_key_held(GLFW_KEY_LEFT_ALT) ||
                            IM.is_key_held(GLFW_KEY_RIGHT_ALT)) &&
                        IM.is_key_held(GLFW_KEY_DELETE);

                    //check if window is fullscreen 
                    bool is_fullscreen = glfwGetWindowAttrib(window, GLFW_MAXIMIZED); 

                    if (alt_pressed || ctrl_alt_del || is_fullscreen) {
                        //minimize the window 
                        glfwIconifyWindow(window); 
                        interrupt_system->focus_lost = true;
                        interrupt_system->current_state = WindowState::INTERRUPTED;

                        //handle interruption 
                        //pause audio and reset input
                        //interrupt_system->handle_interruption(); 

                        if (ctrl_alt_del) {
                            LM.write_log("Interruption_System: Handling CTRL+ALT+DEL interruption");
                        }
                        else if (alt_pressed) {
                            LM.write_log("Interruption_System: Handling ALT+TAB interruption");
                        }
                        else {
                            LM.write_log("Interruption_System: Handling full screen focus loss");
                        }
                        
                        //log stored state 
                        LM.write_log("Interruption_System: Stored state - Fullscreen: %d, Size: %dx%d, Pos: (%d,%d)",
                            interrupt_system->was_fullscreen,
                            interrupt_system->prev_width,
                            interrupt_system->prev_height,
                            interrupt_system->prev_x,
                            interrupt_system->prev_y);
                    }
           
                }
                else {
                    //window gained focus
                    if (interrupt_system->focus_lost) {
                        interrupt_system->focus_lost = false;
                        interrupt_system->current_state = WindowState::NORMAL;
                        interrupt_system->handle_restoration();
                        LM.write_log("Interruption_System: Window regained focus - restoring state");
                    }
                }
                break;
            }

        }
    }
    
#endif


#if 0
    void Interruption_System::focus_callback(GLFWwindow* window, int focused) {
        for (auto const& system : ECSM.get_systems()) {
            if (auto* interrupt_system = dynamic_cast<Interruption_System*>(system.get())) {
                LM.write_log("Interruption_System: Focus callback - focused: %d", focused);
                if (!focused) {
                    // Store the current window state before minimizing
                    interrupt_system->store_window_state(); 

                    // Force minimize when focus is lost in full screen
                    if (glfwGetWindowAttrib(window, GLFW_MAXIMIZED)) {
                        glfwIconifyWindow(window);
                        interrupt_system->focus_lost = true;
                        LM.write_log("Interruption_System: Force minimizing full screen window on focus loss");
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
#endif

    void Interruption_System::focus_callback(GLFWwindow* window, int focused) {
        for (auto const& system : ECSM.get_systems()) {
            if (auto* interrupt_system = dynamic_cast<Interruption_System*>(system.get())) {
                LM.write_log("Interruption_System: Focus callback - focused: %d", focused);

                 if (!focused) {
                    //check for ALT+TAB
      

   
                    //check if window is fullscreen 
                    bool is_fullscreen = glfwGetWindowAttrib(window, GLFW_MAXIMIZED);
                    // Only handle through focus_callback if it's a key combination
                                   // Let iconify_callback handle regular minimize/restore


                    if (interrupt_system->alt_tab_active || interrupt_system->ctrl_alt_del_active) {
                        if (!interrupt_system->is_minimized && !focused) {
                            interrupt_system->current_state = WindowState::INTERRUPTED;
                            glfwIconifyWindow(window);
                            interrupt_system->handle_interruption();

                            if (interrupt_system->ctrl_alt_del_active) {
                                LM.write_log("Interruption_System: Handling CTRL+ALT+DEL interruption");
                            }
                            else {
                                LM.write_log("Interruption_System: Handling ALT+TAB interruption");
                            }
                        }
                    }
                    // Handle fullscreen focus loss
                    else if (is_fullscreen) {
                        if (!interrupt_system->is_minimized) {
                            interrupt_system->current_state = WindowState::INTERRUPTED;
                            glfwIconifyWindow(window);
                            interrupt_system->handle_interruption();
                            LM.write_log("Interruption_System: Handling fullscreen focus loss");
                        }
                    }

                    interrupt_system->focus_lost = true;
                }
                else {
                    // Window gained focus
                    if (interrupt_system->focus_lost && !interrupt_system->is_minimized) {
                        interrupt_system->focus_lost = false;
                        interrupt_system->current_state = WindowState::NORMAL;
                        LM.write_log("Interruption_System: Window regained focus");
                    }
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

    void Interruption_System::check_key_combinations() {
       
      //store previous frame's key states 
        previous_keys = current_keys;

        //get current key states 
        current_keys.ctrl = IM.is_key_held(GLFW_KEY_LEFT_CONTROL) ||
            IM.is_key_held(GLFW_KEY_RIGHT_CONTROL);

        current_keys.alt = IM.is_key_held(GLFW_KEY_LEFT_ALT) ||
            IM.is_key_held(GLFW_KEY_RIGHT_ALT);

        current_keys.del = IM.is_key_held(GLFW_KEY_DELETE); 

        //update "was pressed" states
        current_keys.ctrl_was_pressed = previous_keys.ctrl || current_keys.ctrl;
        current_keys.alt_was_pressed = previous_keys.alt || current_keys.alt;
        current_keys.del_was_pressed = previous_keys.del || current_keys.del;

        //check for ALT+TAB
        alt_tab_active = current_keys.alt;

        //check for CTRL+ALT+DEL
        bool potential_ctrl_alt_del = (current_keys.ctrl_was_pressed &&
            current_keys.alt_was_pressed &&
            current_keys.del_was_pressed);

        if (potential_ctrl_alt_del) {
            ctrl_alt_del_active = true;
            LM.write_log("CTRL+ALT+DEL detected (using frame history)");
        }


        //Log Key states for debugging
        if (current_keys.ctrl || current_keys.alt || current_keys.del) {
            LM.write_log("Raw Key States - CTRL: %d, ALT: %d, DEL: %d",
                current_keys.ctrl, current_keys.alt, current_keys.del);
            LM.write_log("Key History States - CTRL: %d, ALT: %d, DEL: %d",
                current_keys.ctrl_was_pressed, current_keys.alt_was_pressed, current_keys.del_was_pressed);
        }

        //reset the history if no keys are pressed 
        if (!current_keys.ctrl && !current_keys.alt && !current_keys.del) {
            current_keys.ctrl_was_pressed = false;
            current_keys.alt_was_pressed = false;
            current_keys.del_was_pressed = false;
        }
       
    }

    bool Interruption_System::is_game_paused() const {
        return is_paused;
    }

    std::string Interruption_System::get_type() const {
        return "Interruption_System";
    }
}
