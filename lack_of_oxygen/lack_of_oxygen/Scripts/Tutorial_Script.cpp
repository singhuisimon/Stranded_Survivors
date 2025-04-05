/**
 * @file Tutorial_Script.cpp
 * @brief Declare the Tutorial_Script class
 * @author Amanda Leow Boon Suan (100%)
 * @date March 10, 2025
 * Copyright (C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#include <iostream>
#include <string>
#include <random>

#include "../Scripts/Tutorial_Script.h"
#include "../Manager/Log_Manager.h"
#include "../Manager/ECS_Manager.h"
#include "../Manager/FPS_Manager.h"
#include "../Manager/Game_Manager.h"
#include "../Manager/Assets_Manager.h"
#include "../Manager/Audio_Manager.h"
#include "../Utility/Entity_Selector_Helper.h"


namespace lof {

    std::string Tutorial_Script::get_type() const {
        return script_name;
    }

    void Tutorial_Script::register_script() {
        auto tutorial_script = shared_from_this();

        tutorial_script->add_function("init", [weak_script = std::weak_ptr<Tutorial_Script>(tutorial_script)](EntityID entity_id) {
            //(void)entity_id;
            auto tutorial_script = weak_script.lock();
            tutorial_script->set_tutorial_page(1);
            tutorial_script->set_transitioning(false);

            if (!ECSM.has_component<Graphics_Component>(entity_id)) {
                ECSM.get_component<Logic_Component>(entity_id).set_state("tutorial_script", ExecutionState::Terminated);
                return;
            }

            //std::cout << "tutorial script init" << std::endl;

            auto& graphic = ECSM.get_component<Graphics_Component>(entity_id);

            tutorial_script->update_button_visibility(entity_id, graphic);
            tutorial_script->update_text_visibility();
            });

        tutorial_script->add_function("update", [weak_script = std::weak_ptr<Tutorial_Script>(tutorial_script)](EntityID entity_id) {
            //(void)entity_id;
            auto tutorial_script = weak_script.lock();
            if (!entity_id) {
                LM.write_log("Tutorial_Script::register_script(): Entity %d is invalid.", entity_id);
                return;
            }
            tutorial_script->check_keys();
            if (!GM.is_transitioning()) {
                tutorial_script->check_pressing_button(entity_id);
            }
            tutorial_script->transit_next_scene();
            //std::cout << "tutorial script update^^^^^^^^^^^^^^^^^^^^" << std::endl;
            });

    }

    void Tutorial_Script::check_keys() {
        //check for keys here
        key_a_last_frame = key_a_pressed;
        key_d_last_frame = key_d_pressed;

        key_a_pressed = IM.is_key_held(GLFW_KEY_A);
        key_d_pressed = IM.is_key_held(GLFW_KEY_D);

        key_e_last_frame = key_e_pressed;
        key_e_pressed = IM.is_key_held(GLFW_KEY_E);

        key_backslash_last_frame = key_backslash_pressed;
        key_backslash_pressed = IM.is_key_held(GLFW_KEY_BACKSLASH); //code for skipping the entire tutorial.
    }

    bool Tutorial_Script::is_key_just_pressed(int key) {
        bool current_state = (glfwGetKey(window, key) == GLFW_PRESS);
        bool was_pressed = key_previous_state[key];  // Check previous state
        key_previous_state[key] = current_state;    // Update state
        return current_state && !was_pressed;
    }

    bool Tutorial_Script::is_key_just_released(int key) {
        bool is_released = (glfwGetKey(window, key) == GLFW_RELEASE) && key_previous_state[key];
        key_previous_state[key] = (glfwGetKey(window, key) == GLFW_PRESS); // Update state for next check
        return is_released;
    }

    bool Tutorial_Script::get_transitioning() {
        return transitioning;
    }

    void Tutorial_Script::set_transitioning(bool new_bool) {
        transitioning = new_bool;
    }

    void Tutorial_Script::set_tutorial_page(int new_page_num) {
        if (new_page_num < 1 || new_page_num > 10) {
            //std::cout << "return due to invalid page number0" << std::endl;
            LM.write_log("Invalid tutorial page %d", new_page_num);
            return;
        }

        tutorial_page = new_page_num;
        LM.write_log("Tutorial page set to %d", tutorial_page);

        EntityID tutorial_text_id = ECSM.find_entity_by_name("tutorial_background_text");
        auto& tutorial_text_graphic = ECSM.get_component<Graphics_Component>(tutorial_text_id);
        tutorial_text_graphic.texture_name = "Tutorial_Text_" + std::to_string(tutorial_page);
        //std::cout << tutorial_text_graphic.texture_name << std::endl;
    }

    void Tutorial_Script::update_button_visibility(EntityID entity_id, Graphics_Component& graphic_comp) {

        std::string entity_name = ECSM.get_entity(entity_id)->get_name();

        if (entity_name == "a_button") {
            if (tutorial_page == 1) {
                graphic_comp.color.a = 0;
            }
            else {
                graphic_comp.color.a = 1;
            }
            //return;
        }
        else if (entity_name == "d_button") {
            if (tutorial_page == 10) {
                graphic_comp.color.a = 0;
            }
            else {
                graphic_comp.color.a = 1;
            }
            //return;
        }
        else if (entity_name == "e_long_button") {
            if (tutorial_page == 10) {
                graphic_comp.color.a = 1;
            }
            else {
                graphic_comp.color.a = 0;
            }
            //return;
        }
    }

    void Tutorial_Script::update_text_visibility() {
        EntityID previous_text_id = ECSM.find_entity_by_name("previous_text");
        if (previous_text_id != INVALID_ENTITY_ID && ECSM.has_component<Text_Component>(previous_text_id)) {
            auto& text_comp = ECSM.get_component<Text_Component>(previous_text_id);
            std::string text;
            if (tutorial_page == 1) {
                text = "";
                text_comp.text = text.c_str();
            }
            else {
                text = "PREVIOUS";
                text_comp.text = text.c_str();
            }
        }

        EntityID next_text_id = ECSM.find_entity_by_name("next_text");
        if (previous_text_id != INVALID_ENTITY_ID && ECSM.has_component<Text_Component>(next_text_id)) {
            auto& text_comp = ECSM.get_component<Text_Component>(next_text_id);
            std::string text;
            if (tutorial_page == 10) {
                text = "";
                text_comp.text = text.c_str();
            }
            else {
                text = "NEXT";
                text_comp.text = text.c_str();
            }
        }
    }

    void Tutorial_Script::check_pressing_button(EntityID entity_id) {
        if (transitioning == true) {
            return;
        }

        if (is_key_just_pressed(GLFW_KEY_BACKSLASH)) {
            transitioning = true;
            return;
        }

        auto* entity = ECSM.get_entity(entity_id);
        if (!entity) {
            return;
        }

        std::string entity_name = entity->get_name();

        static const std::unordered_map<std::string, std::string> button_textures = {
            {"a_button", "A_Batch_16"},
            {"d_button", "D_Batch_16"},
            {"e_long_button", "E_To_Begin_Batch_16"}
        };

        static const std::unordered_map<std::string, std::string> button_sounds = {
            {"a_button", "tutorial_button"},
            {"d_button", "tutorial_button"},
            {"e_long_button", "main_menu"}
        };

        //return if the entity is not a button.
        if (button_textures.find(entity_name) == button_textures.end()) {
            return;
        }

        if (!ECSM.has_component<Transform2D>(entity_id) ||
            !ECSM.has_component<Graphics_Component>(entity_id) ||
            !ECSM.has_component<Audio_Component>(entity_id)) {
            return;
        }

        //auto& transform = ECSM.get_component<Transform2D>(entity_id);
        auto& graphics = ECSM.get_component<Graphics_Component>(entity_id);
        auto& audio = ECSM.get_component<Audio_Component>(entity_id);

        update_button_visibility(entity_id, graphics);
        update_text_visibility();

        std::string base_texture = button_textures.at(entity_name);
        std::string button_audio = button_sounds.at(entity_name);

        if (graphics.color.a < 1.0f) {
            graphics.texture_name = base_texture + "_NORMAL";

            //set it to true to avoid playing the audio
            tutorial_button_playing[entity_name] = true;
            return;
        }

        if (entity_name == "a_button") {

            if (is_key_just_released(GLFW_KEY_A)) {
                //std::cout << "a is released====================" << std::endl;
                set_tutorial_page(tutorial_page - 1);
                tutorial_button_playing[entity_name] = false;
                graphics.texture_name = base_texture + "_NORMAL";
                return;
            }
            else if (key_a_pressed) {
                //std::cout << "a is held=======================" << std::endl;
                if (!tutorial_button_playing[entity_name]) {
                    ADM.play_now(entity_id, button_audio, audio);
                    tutorial_button_playing[entity_name] = true;
                }
                graphics.texture_name = base_texture + "_PRESSED";
                return;
            }
            else {
                graphics.texture_name = base_texture + "_NORMAL";
                tutorial_button_playing[entity_name] = false;
                return;
            }
        }
        
        else if (entity_name == "d_button") {
            //std::cout << "correct entity " << entity_name << std::endl;
            if (is_key_just_released(GLFW_KEY_D)) {
                //std::cout << "d released+++++++++++++++" << std::endl;
                set_tutorial_page(tutorial_page + 1);
                tutorial_button_playing[entity_name] = false;
                graphics.texture_name = base_texture + "_NORMAL";
                return;
            }
            else if (key_d_pressed) {
                //std::cout << "d is held+++++++++++++++++" << std::endl;
                if (!tutorial_button_playing[entity_name]) {
                    ADM.play_now(entity_id, button_audio, audio);
                    tutorial_button_playing[entity_name] = true;
                }
                graphics.texture_name = base_texture + "_PRESSED";
                return;
            }
            else {
                tutorial_button_playing[entity_name] = false;
                graphics.texture_name = base_texture + "_NORMAL";
                return;
            }
        }
        
        if (entity_name == "e_long_button") {
            //std::cout << "entity is e" << std::endl;
            if (is_key_just_released(GLFW_KEY_E)) {
                //std::cout << "e released" << std::endl;
                transitioning = true;
                tutorial_button_playing[entity_name] = false;
                graphics.texture_name = base_texture + "_NORMAL";
                return;
            }
            else if (key_e_pressed) {
                //std::cout << "e is held" << std::endl;
                if (tutorial_button_playing[entity_name] == false) {
                    ADM.play_now(entity_id, button_audio, audio);
                    tutorial_button_playing[entity_name] = true;
                }
                graphics.texture_name = base_texture + "_PRESSED";
                return;
            }
            else {
                tutorial_button_playing[entity_name] = false;
                graphics.texture_name = base_texture + "_NORMAL";
                return;
            }

            
        }
    }

    void Tutorial_Script::transit_next_scene() {
        if (transitioning) {
            // Clear dynamic entities first
            bool found_movement_system = false;
            for (auto& system : ECSM.get_systems()) {
                if (auto* movement_system = dynamic_cast<Movement_System*>(system.get())) {
                    movement_system->clear_dynamic_entities();
                    found_movement_system = true;
                    LM.write_log("Found and cleared Movement System");
                    break;
                }
            }
            if (!found_movement_system) {
                LM.write_log("Warning: Movement System not found");
            }

            // Set up scene loading
            const std::string SCENES = "Scenes";
            std::string scene_file = "scene2.scn";
            std::string scene_path = ASM.get_full_path(SCENES, scene_file);
            LM.write_log("Attempting to load scene from path: %s", scene_path.c_str());

            // Try to load scene2
            if (SM.load_scene(scene_path.c_str())) {
                LM.write_log("Scene loaded successfully");

                // Reset camera position
                auto& camera = GFXM.get_camera();
                camera.pos_x = DEFAULT_CAMERA_POS_X;
                camera.pos_y = DEFAULT_CAMERA_POS_Y;

                // Reset player position if it exists
                EntityID playerId = ECSM.find_entity_by_name(DEFAULT_PLAYER_NAME);
                if (playerId != INVALID_ENTITY_ID) {
                    if (ECSM.has_component<Transform2D>(playerId)) {
                        auto& player_transform = ECSM.get_component<Transform2D>(playerId);
                        player_transform.position = Vec2D(0.0f, 0.0f);
                        player_transform.prev_position = player_transform.position;
                    }
                    if (ECSM.has_component<Velocity_Component>(playerId)) {
                        auto& velocity = ECSM.get_component<Velocity_Component>(playerId);
                        velocity.velocity = Vec2D(0.0f, 0.0f);
                    }
                }

                // Update current scene in Game Manager
                GM.set_current_scene(2);

                // Update IMGUI Manager's current file
                //IMGUIM.set_current_file_shown(scene_file);
                return;
            }
            else {
                LM.write_log("Failed to load scene file: %s", scene_path.c_str());
            }
        }
    }

} // namespace lof