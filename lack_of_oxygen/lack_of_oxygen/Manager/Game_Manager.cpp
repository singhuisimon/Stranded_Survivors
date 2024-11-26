/**
 * @file Game_Manager.cpp
 * @brief Implements the Game_Manager class helper functions.
 * @author Simon Chan (82.901%), Amanda Leow (11.91%), Liliana Hanawardani (5.181%)
 * @date September 21, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

 // Include header file
#include "Game_Manager.h"

// Include other managers
#include "Log_Manager.h"
#include "ECS_Manager.h"
#include "FPS_Manager.h"
#include "Serialization_Manager.h"
#include "Input_Manager.h"
#include "Graphics_Manager.h"

// Include utility
#include "../Utility/Constant.h"
//#include "../Utility/Path_Helper.h"
#include "../Utility/Globals.h"

// Include systems
#include "../System/Audio_System.h" // Add this for Audio System access
#include "../System/GUI_System.h"  // Add this for GUI system access
#include "../System/Animation_System.h"  // For player_direction
#include "../System/Collision_System.h" // for click entity object

#include "../Utility/Entity_Selector_Helper.h"

// Include iostream for console output
#include <iostream>
#include <random>
#include <chrono>

namespace lof {

    float imgui_camara_pos_x = 0.0f;
    float imgui_camera_pos_y = 0.0f;
    Game_Manager::Game_Manager()
        : m_game_over(false), m_step_count(0) {
        set_type("Game_Manager");
    }

    Game_Manager& Game_Manager::get_instance() {
        static Game_Manager instance;
        return instance;
    }

    int Game_Manager::start_up() {
        if (is_started()) {
            return 0; // Already started
        }

        // -------------------------- Log Manager Start Up --------------------------
        if (LM.start_up() != 0) {
            // If Log_Manager fails to start, there's no way to log the error using LM
            return -1;
        }
        else {
            LM.write_log("Game_Manager::start_up(): Log_Manager start_up() successful");
        }

        // -------------------------- ECS Manager Start Up --------------------------
        if (ECSM.start_up() != 0) {
            LM.write_log("Game_Manager::start_up(): ECS_Manager start_up() failed");
            LM.shut_down();
            return -2;
        }
        else {
            LM.write_log("Game_Manager::start_up(): ECS_Manager start_up() successful");
        }

        // -------------------------- Serialization Manager Start Up -----------------
        if (SM.start_up() != 0) {
            LM.write_log("Game_Manager::start_up(): Serialization_Manager start_up() failed");
            ECSM.shut_down();
            LM.shut_down();
            return -3;
        }
        else {
            LM.write_log("Game_Manager::start_up(): Serialization_Manager start_up() successful");
        }

        // -------------------------- FPS Manager Start Up --------------------------
        if (FPSM.start_up() != 0) {
            LM.write_log("Game_Manager::start_up(): FPS_Manager start_up() failed");
            SM.shut_down();
            ECSM.shut_down();
            LM.shut_down();
            return -4;
        }
        else {
            LM.write_log("Game_Manager::start_up(): FPS_Manager start_up() successful");
        }

        // -------------------------- Input Manager Start Up --------------------------
        if (IM.start_up() != 0) {
            LM.write_log("Game_Manager::start_up(): Input_Manager start_up() failed");
            FPSM.shut_down();
            SM.shut_down();
            ECSM.shut_down();
            LM.shut_down();
            return -5;
        }
        else {
            LM.write_log("Game_Manager::start_up(): Input_Manager start_up() successful");
        }

        // -------------------------- Graphics Manager Start Up --------------------------
        if (GFXM.start_up() != 0) {
            LM.write_log("Game_Manager::start_up(): Graphics_Manager start_up() failed");
            IM.shut_down();
            FPSM.shut_down();
            SM.shut_down();
            ECSM.shut_down();
            LM.shut_down();
            return -7;
        }
        else {
            LM.write_log("Game_Manager::start_up(): Graphics_Manager start_up() successful");
        }

        m_is_started = true;
        LM.write_log("Game_Manager::start_up(): Game_Manager started");
        std::cout << "Game_Manager started successfully." << std::endl;

        return 0;
    }

    void Game_Manager::shut_down() {
        if (!is_started()) {
            return;
        }

        // Shut down managers in reverse order of startup
        GFXM.shut_down(); // Graphics_Manager
        IM.shut_down();   // Input_Manager
        FPSM.shut_down(); // FPS_Manager
        SM.shut_down();   // Serialization_Manager
        ECSM.shut_down(); // ECS_Manager
        LM.shut_down();   // Log_Manager

        m_is_started = false;
        std::cout << "Game_Manager shut down successfully." << std::endl;
    }

    EntityInfo& selectedEntityInfo = ESS.get_selected_entity_info();

    EntityID selectedID = -1;


    void Game_Manager::update(float delta_time) {
        if (!is_started()) {
            LM.write_log("Game_Manager::update(): Game_Manager not started");
            return;
        }

        //printf("-----------------in game manager--------------------------------\n");
        bool has_collision_bottom = CS.has_bottom_collide_detect();
        EntityID collision_entity_bottom = CS.get_bottom_collide_entity();

        bool has_collision_left = CS.has_left_collide_detect();
        EntityID collision_entity_left = CS.get_left_collide_entity();

        bool has_collision_right = CS.has_right_collide_detect();
        EntityID collision_entity_right = CS.get_right_collide_entity();

        bool has_collision_top = CS.has_top_collide_detect();
        EntityID collision_entity_top = CS.get_top_collide_entity();

        //printf("Has left collision outside: %s\n", has_collision_left ? "true" : "false");
        //printf("left collision entity outside: %d\n\n", collision_entity_left);

        if (IM.is_key_pressed(GLFW_KEY_DOWN) && !level_editor_mode && has_collision_bottom) {

            printf("Has bottom collision: %s\n", has_collision_bottom ? "true" : "false");
            printf("Bottom collision entity: %d\n", collision_entity_bottom);

        }
        if (IM.is_key_held(GLFW_KEY_LEFT) && !level_editor_mode && has_collision_left) {

            printf("Has left collision: %s\n", has_collision_left ? "true" : "false");
            printf("left collision entity: %d\n", collision_entity_left);

        }
        if (IM.is_key_held(GLFW_KEY_RIGHT) && !level_editor_mode && has_collision_right) {

            printf("Has right collision: %s\n", has_collision_right ? "true" : "false");
            printf("right entity: %d\n", collision_entity_right);

        }
        if (IM.is_key_held(GLFW_KEY_UP) && !level_editor_mode && has_collision_top) {

            printf("Has top collision: %s\n", has_collision_top ? "true" : "false");
            printf("top entity: %d\n", collision_entity_top);

        }

        //std::cout << "collision non collidable: " << CS.get_detect_entities() << "\n";
        //std::cout << "collision non collidable: " << CS.get_detect_entities() << "\n";
        //std::cout << "collision: " << CS.mineral_tank_detected() << "\n";
        //std::cout << "collision: " << CS.oxygen_tank_detected() << "\n";

        //printf("-----------------in game manager--------------------------------\n\n");

#if 0
        ESS.Check_Selected_Entity();

        // Check if the left mouse button was pressed
        EntityInfo& selectedEntityInfo = ESS.get_selected_entity_info();
        if (IM.is_key_held(GLFW_KEY_W) && !level_editor_mode) {
            // Handle left mouse button press
            std::cout << "Left mouse button pressed." << std::endl;


            if (selectedEntityInfo.isSelected) {
                select_entity = true;
                selectedID = selectedEntityInfo.selectedEntity;

                std::cout << "Selected Entity ID : " << selectedEntityInfo.selectedEntity << "\n";
                std::cout << "mouse position x: " << selectedEntityInfo.mousePos.x << " ,mouse position y: " << selectedEntityInfo.mousePos.y << "\n";
                std::cout << "bool if is selected (1 is selected, 0 is not): " << selectedEntityInfo.isSelected << "\n";
                LM.write_log("Selected Entity ID system: %d", selectedEntityInfo.selectedEntity);

            }
            else {
                select_entity = false;
                selectedID = selectedEntityInfo.selectedEntity;

                std::cout << "No entity is selected.\n";
                std::cout << "mouse position x: " << selectedEntityInfo.mousePos.x << " ,mouse position y: " << selectedEntityInfo.mousePos.y << "\n";
                std::cout << "bool if is selected (1 is selected, 0 is not): " << selectedEntityInfo.isSelected << "\n";
            }
        }
#endif
        //std::cout << "This is seleteed entity id no: " << selectedEntityID << "\n";
        try {
            // Simulate a crash when the 'P' key is pressed
            if (IM.is_key_pressed(GLFW_KEY_P)) {
                LM.write_log("Game_Manager::update(): Simulated crash. 'P' key was pressed.");
                throw std::runtime_error("Simulated crash: 'P' key was pressed.");
            }
        }
        catch (const std::exception& e) {
            LM.write_log("Game_Manager::update(): Exception caught: %s", e.what());
        }

        // Check for game over condition based on input, before IM update
        if (IM.is_key_pressed(GLFW_KEY_ESCAPE)) {
            set_game_over(true);
            LM.write_log("Game_Manager::update(): Escape key pressed. Setting game_over to true.");
            std::cout << "Escape key pressed. Closing the game." << std::endl;
        }

        // GUI System control
        if (IM.is_key_pressed(GLFW_KEY_G) && !level_editor_mode) {
            // Find GUI System
            for (auto& system : ECSM.get_systems()) {
                if (system->get_type() == "GUI_System") {
                    auto* gui_system = static_cast<GUI_System*>(system.get());
                    if (gui_system) {
                        // Toggle loading screen
                        static bool loading_screen_visible = false;
                        if (!loading_screen_visible) {
                            loading_screen_visible = true;
                            gui_system->show_loading_screen();
                            LM.write_log("Game_Manager::update(): Showing loading screen");
                        }
                        else {
                            loading_screen_visible = false;
                            gui_system->hide_loading_screen();
                            LM.write_log("Game_Manager::update(): Hiding loading screen");
                        }
                    }
                    break;
                }
            }
        }

        // Test progress bar updates with H key
        if (IM.is_key_pressed(GLFW_KEY_H) && !level_editor_mode) {
            static float test_progress = 0.0f;
            test_progress += 0.1f;
            if (test_progress > 1.0f) test_progress = 0.0f;

            // Find GUI System and update progress
            for (auto& system : ECSM.get_systems()) {
                if (system->get_type() == "GUI_System") {
                    auto* gui_system = static_cast<GUI_System*>(system.get());
                    if (gui_system) {
                        gui_system->set_progress(test_progress);
                        LM.write_log("Game_Manager::update(): Updated progress bar to %.2f", test_progress);
                    }
                    break;
                }
            }
        }

        //to pause all the sound that is playing
        if (IM.is_key_pressed(GLFW_KEY_5)) {
            for (auto& system : ECSM.get_systems()) {
                if (system->get_type() == "Audio_System") {
                    auto* audio_system = static_cast<Audio_System*>(system.get());
                    if (audio_system) {
                        audio_system->pause_resume_mastergroup();
                    }
                }
            }
        }

        // Handle player movement and physics input
        EntityID player_id = ECSM.find_entity_by_name(DEFAULT_PLAYER_NAME);

        if (player_id != 0) {  // If player entity exists
            if (ECSM.has_component<Physics_Component>(player_id)) {

                auto& physics = ECSM.get_component<Physics_Component>(player_id);

                if (IM.is_key_pressed(GLFW_KEY_LEFT)) {
                    if (CS.has_left_collide_detect()) {
                        EntityID block_to_remove = CS.get_left_collide_entity();
                        if (block_to_remove != INVALID_ENTITY_ID) {
                            // Destroy the colliding block on the left
                            ECSM.destroy_entity(block_to_remove);
                            LM.write_log("Game_Manager::update: Removed left block (Entity %u)", block_to_remove);
                        }
                    }
                }
                else if (IM.is_key_pressed(GLFW_KEY_RIGHT)) {
                    if (CS.has_right_collide_detect()) {
                        EntityID block_to_remove = CS.get_right_collide_entity();
                        if (block_to_remove != INVALID_ENTITY_ID) {
                            // Destroy the colliding block on the right
                            ECSM.destroy_entity(block_to_remove);
                            LM.write_log("Game_Manager::update: Removed right block (Entity %u)", block_to_remove);
                        }
                    }
                }
                else if (IM.is_key_pressed(GLFW_KEY_UP)) {
                    // Note: You might need to add top collision detection in Collision System
                    // Similar to left/right collisions
                    if (CS.has_top_collide_detect()) {
                        EntityID block_to_remove = CS.get_top_collide_entity();
                        if (block_to_remove != INVALID_ENTITY_ID) {
                            // Destroy the colliding block on the right
                            ECSM.destroy_entity(block_to_remove);
                            LM.write_log("Game_Manager::update: Removed right block (Entity %u)", block_to_remove);
                        }
                    }
                }
                else if (IM.is_key_pressed(GLFW_KEY_DOWN)) {
                    if (CS.has_bottom_collide_detect()) {
                        EntityID block_to_remove = CS.get_bottom_collide_entity();
                        if (block_to_remove != INVALID_ENTITY_ID) {
                            // Destroy the colliding block below
                            ECSM.destroy_entity(block_to_remove);
                            LM.write_log("Game_Manager::update: Removed bottom block (Entity %u)", block_to_remove);
                        }
                    }
                }


                // Handle horizontal movement
                if (IM.is_key_held(GLFW_KEY_SPACE)) {
                    physics.set_jump_requested(true); //this will set the flag to true inside the physics_component 
                }
                else {
                    physics.set_jump_requested(false);
                }

                //activate and deactivate the forces. 
                if (IM.is_key_held(GLFW_KEY_A) && !(IM.is_key_held(GLFW_KEY_D))) {
                    // Updates forces
                    physics.force_helper.deactivate_force(MOVE_RIGHT);
                    physics.force_helper.activate_force(MOVE_LEFT);
                    forces_flag = MOVE_LEFT;

                    // Update player animation flag
                    int& direction = GFXM.get_player_direction();
                    direction = MOVE_LEFT;

                    // Update sound effect for player moving left
                    if (physics.get_is_grounded()) {
                        ECSM.get_component<Audio_Component>(player_id).set_audio_state("moving left", PLAYING);
                    }
                }
                else if (IM.is_key_held(GLFW_KEY_D) && !(IM.is_key_held(GLFW_KEY_A))) {
                    // Update forces
                    physics.force_helper.deactivate_force(MOVE_LEFT);
                    physics.force_helper.activate_force(MOVE_RIGHT);
                    forces_flag = MOVE_RIGHT;

                    // Update player animation flag
                    int& direction = GFXM.get_player_direction();
                    direction = MOVE_RIGHT;

                    // Update sound effect for player moving right
                    if (physics.get_is_grounded()) {
                        ECSM.get_component<Audio_Component>(player_id).set_audio_state("moving right", PLAYING);
                    }
                }
                else if (IM.is_key_held(GLFW_KEY_D) && IM.is_key_held(GLFW_KEY_A)) {
                    if (forces_flag == MOVE_LEFT) {
                        // Update forces
                        physics.force_helper.activate_force(MOVE_LEFT);
                        forces_flag = MOVE_LEFT;

                        // Update player animation flag
                        int& direction = GFXM.get_player_direction();
                        direction = MOVE_LEFT;

                        // Update sound effect for player moving left
                        if (physics.get_is_grounded()) {
                            ECSM.get_component<Audio_Component>(player_id).set_audio_state("moving left", PLAYING);
                        }
                    }
                    else {
                        // Update forces
                        physics.force_helper.deactivate_force(MOVE_LEFT);
                        physics.force_helper.activate_force(MOVE_RIGHT);
                        forces_flag = MOVE_RIGHT;

                        // Update player animation flag
                        int& direction = GFXM.get_player_direction();
                        direction = MOVE_RIGHT;

                        // Update sound effect for player moving right
                        if (physics.get_is_grounded()) {
                            ECSM.get_component<Audio_Component>(player_id).set_audio_state("moving right", PLAYING);
                        }
                    }
                }
                else {
                    // Reset forces and player animation
                    physics.force_helper.deactivate_force(MOVE_LEFT);
                    physics.force_helper.deactivate_force(MOVE_RIGHT);
                    forces_flag = -1;

                    int& direction = GFXM.get_player_direction();
                    direction = -1;
                }

            }
        }


        // Change render mode with 1 (FILL), 2 (LINE), 3 (POINT) 
        if (IM.is_key_pressed(GLFW_KEY_1)) {
            LM.write_log("Graphics_Manager::update(): '1' key pressed, render mode is now FILL.");
            GLenum& mode = GFXM.get_render_mode();
            mode = GL_FILL;
        }
        else if (IM.is_key_pressed(GLFW_KEY_2)) {
            LM.write_log("Graphics_Manager::update(): '2' key pressed, render mode is now LINE.");
            GLenum& mode = GFXM.get_render_mode();
            mode = GL_LINE;
        }
        else if (IM.is_key_pressed(GLFW_KEY_3)) {
            LM.write_log("Graphics_Manager::update(): '3' key pressed, render mode is now POINT.");
            GLenum& mode = GFXM.get_render_mode();
            mode = GL_POINT;
        }

        // Toggle debug mode using 'B" or 'N'
        if (IM.is_key_pressed(GLFW_KEY_B)) {
            LM.write_log("Graphics_Manager::update(): 'B' key pressed, Debug Mode is now ON.");
            GLboolean& mode = GFXM.get_debug_mode();
            mode = GL_TRUE;
        }
        else if (IM.is_key_pressed(GLFW_KEY_N)) {
            LM.write_log("Graphics_Manager::update(): 'N' key pressed, Debug Mode is now OFF.");
            GLboolean& mode = GFXM.get_debug_mode();
            mode = GL_FALSE;
        }

#if 0
        // Object  when up and down arrow keys pressed
        if (IM.is_key_held(GLFW_KEY_UP) && !(IM.is_key_held(GLFW_KEY_DOWN))) {
            int& flag = GFXM.get_scale_flag();
            flag = GLFW_KEY_UP;
        }
        else if (IM.is_key_held(GLFW_KEY_DOWN) && !(IM.is_key_held(GLFW_KEY_UP))) {
            int& flag = GFXM.get_scale_flag();
            flag = GLFW_KEY_DOWN;
        }
        else if (IM.is_key_held(GLFW_KEY_UP) && IM.is_key_held(GLFW_KEY_DOWN)) {
            int& flag = GFXM.get_scale_flag();
            if (flag == GLFW_KEY_UP) {
                flag = GLFW_KEY_UP;
            }
            else {
                flag = GLFW_KEY_DOWN;
            }
        }
        else {
            int& flag = GFXM.get_scale_flag();
            flag = 0;
        }
#endif
        if (IM.is_mouse_button_pressed(GLFW_MOUSE_BUTTON_LEFT)) {
            if (select_entity)
            {
                selectedID = selectedEntityInfo.selectedEntity;
            }


        }

        //std::cout << selectedID << "in game manager\n";


#if 1
        //std::cout << "bool if is selected (1 is selected, 0 is not): " << select_entity << "\n";
       // std::cout << select_entity << " this is select entity\n";
        if (select_entity && selectedID != -1 && level_editor_mode) {
            auto& transform = ECSM.get_component<Transform2D>(selectedID);
            auto& collision = ECSM.get_component<Collision_Component>(selectedID);

            GLfloat rot_change = transform.orientation.y * static_cast<GLfloat>(delta_time);
            GLfloat scale_change = DEFAULT_SCALE_CHANGE * static_cast<GLfloat>(delta_time);
            // Example: Scaling the selected entity
            if (IM.is_key_held(GLFW_KEY_UP) && !(IM.is_key_held(GLFW_KEY_DOWN))) {
                // Increase the size of the selected entity
                transform.scale.x += scale_change;
                transform.scale.y += scale_change;
                collision.width += scale_change;
                collision.height += scale_change;
            }
            else if (IM.is_key_held(GLFW_KEY_DOWN) && !(IM.is_key_held(GLFW_KEY_UP))) {
                // Increase the size of the selected entity
                if (transform.scale.x > 0.0f) {
                    transform.scale.x -= scale_change;
                    collision.width -= scale_change;
                }
                else {
                    transform.scale.x = 0.0f;
                    collision.width = 0.0f;
                }

                if (transform.scale.y > 0.0f) {
                    transform.scale.y -= scale_change;
                    collision.height -= scale_change;
                }
                else {
                    transform.scale.y = 0.0f;
                    collision.height = 0.0f;
                }
            }
            else if (IM.is_key_held(GLFW_KEY_LEFT) && !(IM.is_key_held(GLFW_KEY_RIGHT)))
            {
                transform.orientation.x += rot_change;
            }
            else if (IM.is_key_held(GLFW_KEY_RIGHT) && !(IM.is_key_held(GLFW_KEY_LEFT)))
            {
                transform.orientation.x -= rot_change;
            }
        }
#endif 

#if 0
        // Object rotation when left and right arrow keys pressed
        if (IM.is_key_held(GLFW_KEY_LEFT) && !(IM.is_key_held(GLFW_KEY_RIGHT))) {
            int& flag = GFXM.get_rotation_flag();
            flag = GLFW_KEY_LEFT;
        }
        else if (IM.is_key_held(GLFW_KEY_RIGHT) && !(IM.is_key_held(GLFW_KEY_LEFT))) {
            int& flag = GFXM.get_rotation_flag();
            flag = GLFW_KEY_RIGHT;
        }
        else if (IM.is_key_held(GLFW_KEY_LEFT) && IM.is_key_held(GLFW_KEY_RIGHT)) {
            int& flag = GFXM.get_rotation_flag();
            if (flag == GLFW_KEY_LEFT) {
                flag = GLFW_KEY_LEFT;
            }
            else {
                flag = GLFW_KEY_RIGHT;
            }
        }
        else {
            int& flag = GFXM.get_rotation_flag();
            flag = 0;
        }
#endif


        if (IM.is_key_pressed(GLFW_KEY_TAB)) {
            auto& camera = GFXM.get_camera();
            if (camera.is_free_cam == GL_FALSE) {
                camera.is_free_cam = GL_TRUE;
            }
            else {
                camera.is_free_cam = GL_FALSE;
            }

            int& editor_mode = GFXM.get_editor_mode();
            if (editor_mode == 1) {
                editor_mode = 0;
            }
            else {
                editor_mode = 1;
            }
        }

        // Camera up-down scrolling when Number Pad 8 or 2 pressed
        if (IM.is_key_held(GLFW_KEY_KP_8) && !(IM.is_key_held(GLFW_KEY_KP_2))) {
            camera_up_down_scroll_flag = GLFW_KEY_KP_8;
            auto& camera = GFXM.get_camera();
            if (camera.is_free_cam == true) {
                camera.pos_y += (DEFAULT_CAMERA_SPEED * static_cast<GLfloat>(delta_time));
                imgui_camera_pos_y = camera.pos_y;
                LM.write_log("Render_System::update(): 'Keypad 8' key held, camera position is now %f.", camera.pos_y);
            }
        }
        else if (IM.is_key_held(GLFW_KEY_KP_2) && !(IM.is_key_held(GLFW_KEY_KP_8))) {
            camera_up_down_scroll_flag = GLFW_KEY_KP_2;
            auto& camera = GFXM.get_camera();
            if (camera.is_free_cam == true) {
                camera.pos_y -= (DEFAULT_CAMERA_SPEED * static_cast<GLfloat>(delta_time));
                imgui_camera_pos_y = camera.pos_y;
                LM.write_log("Render_System::update(): 'Keypad 2' key held, camera position is now %f.", camera.pos_y);
            }
        }
        else if (IM.is_key_held(GLFW_KEY_KP_8) && IM.is_key_held(GLFW_KEY_KP_2)) {
            auto& camera = GFXM.get_camera();
            if (camera_up_down_scroll_flag == GLFW_KEY_KP_8) {
                camera.pos_y += (DEFAULT_CAMERA_SPEED * static_cast<GLfloat>(delta_time));
                imgui_camera_pos_y = camera.pos_y;
                LM.write_log("Render_System::update(): 'Keypad 8' key held, camera position is now %f.", camera.pos_y);
            }
            else {
                camera.pos_y -= (DEFAULT_CAMERA_SPEED * static_cast<GLfloat>(delta_time));
                imgui_camera_pos_y = camera.pos_y;
                LM.write_log("Render_System::update(): 'Keypad 2' key held, camera position is now %f.", camera.pos_y);
            }
        }
        else {
            camera_up_down_scroll_flag = 0;
        }

        // Camera left-right scrolling when Number Pad 4 or 6 pressed
        if (IM.is_key_held(GLFW_KEY_KP_4) && !(IM.is_key_held(GLFW_KEY_KP_6))) {
            camera_left_right_scroll_flag = GLFW_KEY_KP_4;
            auto& camera = GFXM.get_camera();
            if (camera.is_free_cam == true) {
                camera.pos_x -= (DEFAULT_CAMERA_SPEED * static_cast<GLfloat>(delta_time));
                imgui_camara_pos_x = camera.pos_x;
                LM.write_log("Render_System::update(): 'Keypad 8' key held, camera position is now %f.", camera.pos_y);
            }
        }
        else if (IM.is_key_held(GLFW_KEY_KP_6) && !(IM.is_key_held(GLFW_KEY_KP_4))) {
            camera_left_right_scroll_flag = GLFW_KEY_KP_6;
            auto& camera = GFXM.get_camera();
            if (camera.is_free_cam == true) {
                camera.pos_x += (DEFAULT_CAMERA_SPEED * static_cast<GLfloat>(delta_time));
                imgui_camara_pos_x = camera.pos_x;
                LM.write_log("Render_System::update(): 'Keypad 2' key held, camera position is now %f.", camera.pos_y);
            }
        }
        else if (IM.is_key_held(GLFW_KEY_KP_4) && IM.is_key_held(GLFW_KEY_KP_6)) {
            auto& camera = GFXM.get_camera();
            if (camera_left_right_scroll_flag == GLFW_KEY_KP_4) {
                camera.pos_x -= (DEFAULT_CAMERA_SPEED * static_cast<GLfloat>(delta_time));
                imgui_camara_pos_x = camera.pos_x;
                LM.write_log("Render_System::update(): 'Keypad 8' key held, camera position is now %f.", camera.pos_y);
            }
            else {
                camera.pos_x += (DEFAULT_CAMERA_SPEED * static_cast<GLfloat>(delta_time));
                imgui_camara_pos_x = camera.pos_x;
                LM.write_log("Render_System::update(): 'Keypad 2' key held, camera position is now %f.", camera.pos_y);
            }
        }
        else {
            camera_left_right_scroll_flag = 0;
        }

        if (IM.is_key_pressed(GLFW_KEY_0)) {
            LM.write_log("Game_Manager::update(): Toggling between scenes");

            // Toggle between scenes
            current_scene = (current_scene == 1) ? 2 : 1;

            // Create full path to the scene file
            const std::string SCENES = "Scenes";
            std::string scene_path = ASM.get_full_path(SCENES, "scene" + std::to_string(current_scene) + ".scn");

            // Try to load the new scene
            if (SM.load_scene(scene_path.c_str())) {
                LM.write_log("Game_Manager::update(): Successfully loaded scene%d: %s", current_scene, scene_path.c_str());

                // Reset camera position
                auto& camera = GFXM.get_camera();
                camera.pos_x = DEFAULT_CAMERA_POS_X;
                camera.pos_y = DEFAULT_CAMERA_POS_Y;

                // Reset player position if exists
                EntityID player_id = ECSM.find_entity_by_name(DEFAULT_PLAYER_NAME);
                if (player_id != INVALID_ENTITY_ID) {
                    if (ECSM.has_component<Transform2D>(player_id)) {
                        auto& transform = ECSM.get_component<Transform2D>(player_id);
                        transform.position = Vec2D(0.0f, 0.0f);
                        transform.prev_position = transform.position;
                    }
                    if (ECSM.has_component<Velocity_Component>(player_id)) {
                        auto& velocity = ECSM.get_component<Velocity_Component>(player_id);
                        velocity.velocity = Vec2D(0.0f, 0.0f);
                    }
                }
            }
            else {
                LM.write_log("Game_Manager::update(): Failed to load scene%d: %s", current_scene, scene_path.c_str());
                // Revert the scene number since load failed
                current_scene = (current_scene == 1) ? 2 : 1;
            }
        }



        // Getting delta time for Input Manager
        IM.set_time(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
        // Update Input_Manager
        IM.update();
        IM.set_time(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() - IM.get_time());

        // Getting delta time for Graphics Manager
        GFXM.set_time(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
        GFXM.set_time(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() - GFXM.get_time());

        // Getting delta time for ECS Manager
        ECSM.set_time(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
        // Update game world state
        ECSM.update(delta_time);
        ECSM.set_time(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() - ECSM.get_time());

        m_step_count++;
    }

    void Game_Manager::set_game_over(bool new_game_over) {
        m_game_over = new_game_over;
        LM.write_log("Game_Manager::set_game_over(): game_over set to %s", new_game_over ? "true" : "false");
        std::cout << "Game_Manager::set_game_over(): game_over set to " << (new_game_over ? "true" : "false") << std::endl;
    }

    bool Game_Manager::get_game_over() const {
        return m_game_over;
    }

    int Game_Manager::get_step_count() const {
        return m_step_count;
    }

} // namespace lof
