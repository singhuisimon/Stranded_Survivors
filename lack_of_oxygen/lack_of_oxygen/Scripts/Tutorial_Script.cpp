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

    Tutorial_Script::Tutorial_Script() : siren_audio_cooldown(SIREN_AUDIO_COOLDOWN) {}

    std::string Tutorial_Script::get_type() const {
        return script_name;
    }

    void Tutorial_Script::register_script() {
        auto tutorial_script = shared_from_this();

        tutorial_script->add_function("init", [weak_script = std::weak_ptr<Tutorial_Script>(tutorial_script)](EntityID entity_id) {
            (void)entity_id;
            auto tutorial_script = weak_script.lock();
            tutorial_script->set_tutorial_page(1);

            if (!ECSM.has_component<Graphics_Component>(entity_id)) {
                ECSM.get_component<Logic_Component>(entity_id).set_state("tutorial_script", ExecutionState::Terminated);
                return;
            }

            auto& graphic = ECSM.get_component<Graphics_Component>(entity_id);

            tutorial_script->update_button_visibility(entity_id, graphic);
            });

        tutorial_script->add_function("update", [weak_script = std::weak_ptr<Tutorial_Script>(tutorial_script)](EntityID entity_id) {
            //(void)entity_id;
            auto tutorial_script = weak_script.lock();
            if (!entity_id) {
                LM.write_log("Tutorial_Script::register_script(): Entity %d is invalid.", entity_id);
                return;
            }
            tutorial_script->check_keys();
            tutorial_script->update_button(entity_id);
            tutorial_script->transit_next_scene();

            });

    }

    void Tutorial_Script::check_keys() {
        //check for keys here
        left_mouse_last_frame = left_mouse_pressed;
        key_a_last_frame = key_a_pressed;
        key_d_last_frame = key_d_pressed;

        left_mouse_pressed = IM.is_mouse_button_held(GLFW_MOUSE_BUTTON_LEFT);
        key_a_pressed = IM.is_key_held(GLFW_KEY_A);
        key_d_pressed = IM.is_key_held(GLFW_KEY_D);

        key_e_last_frame = key_e_pressed;
        key_e_pressed = IM.is_key_held(GLFW_KEY_E);

        key_esc_last_frame = key_esc_pressed;
        key_esc_pressed = IM.is_key_held(GLFW_KEY_ESCAPE); //code for skipping the entire tutorial.
    }

    bool Tutorial_Script::is_key_just_pressed(int key) {
        bool just_pressed = false;
        if (key == GLFW_MOUSE_BUTTON_LEFT) {
            just_pressed = left_mouse_pressed && !left_mouse_last_frame;
        }
        else if (key == GLFW_KEY_A) {
            just_pressed = key_a_pressed && !key_a_last_frame;
        }
        else if (key == GLFW_KEY_D) {
            just_pressed = key_d_pressed && !key_d_last_frame;
        }
        else if (key == GLFW_KEY_E) {
            just_pressed = key_e_pressed && !key_e_last_frame;
        }
        else if (key == GLFW_KEY_ESCAPE)
        {
            just_pressed = key_esc_pressed && !key_esc_last_frame;
        }

        if (just_pressed) {
            std::cout << "KEY " << key << " was just pressed" << std::endl;
        }

        return just_pressed;
    }

    bool Tutorial_Script::is_key_being_hold(int key) {
        if (key == GLFW_MOUSE_BUTTON_LEFT) {
            return left_mouse_pressed && left_mouse_last_frame;
        }
        else if (key == GLFW_KEY_A) {
            return key_a_pressed && key_a_last_frame;
        }
        else if (key == GLFW_KEY_D) {
            return key_d_pressed && key_d_last_frame;
        }
        else if (key == GLFW_KEY_E) {
            return key_e_pressed && key_e_last_frame;
        }
        else if (key == GLFW_KEY_ESCAPE) {
            return key_esc_pressed && key_esc_last_frame;
        }
        return false;
    }

    bool Tutorial_Script::is_key_just_released(int key) {
        if (key == GLFW_MOUSE_BUTTON_LEFT) {
            return !left_mouse_pressed && left_mouse_last_frame;
        }
        else if (key == GLFW_KEY_A) {
            return !key_a_pressed && key_a_last_frame;
        }
        else if (key == GLFW_KEY_D) {
            return !key_d_pressed && key_d_last_frame;
        }
        else if (key == GLFW_KEY_E) {
            return !key_e_pressed && key_e_last_frame;
        }
        else if (key == GLFW_KEY_ESCAPE) {
            return !key_esc_pressed && key_esc_last_frame;
        }
        return false;
    }

    void Tutorial_Script::set_tutorial_page(int new_page_num) {
        if (new_page_num < 1 || new_page_num > 10) {
            std::cout << "return due to invalid page number0" << std::endl;
            LM.write_log("Invalid tutorial page %d", new_page_num);
            return;
        }

        tutorial_page = new_page_num;
        LM.write_log("Tutorial page set to %d", tutorial_page);

        EntityID tutorial_text_id = ECSM.find_entity_by_name("tutorial_background_text");
        auto& tutorial_text_graphic = ECSM.get_component<Graphics_Component>(tutorial_text_id);
        tutorial_text_graphic.texture_name = "tutorial_text_" + std::to_string(tutorial_page);
        //page_transition_cooldown = TUTORIAL_COOLDOWN_TIME;
        std::cout << tutorial_text_graphic.texture_name << std::endl;
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

    void Tutorial_Script::update_button(EntityID entity_id) {

        if (page_transition_cooldown > 0.0f) {
            page_transition_cooldown -= FPSM.get_delta_time();
            //std::cout << "still in cooldown" << page_transition_cooldown << std::endl;
            return;
        }

        if (transitioning == true) {
            return;
        }

        //transitioning = false;

        if (is_key_just_pressed(GLFW_KEY_ESCAPE)) {
            transitioning = true;
            return;
        }

        Vec2D world_mouse_pos = ESS.Get_World_MousePos();

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

        auto& transform = ECSM.get_component<Transform2D>(entity_id);
        auto& graphics = ECSM.get_component<Graphics_Component>(entity_id);
        auto& audio = ECSM.get_component<Audio_Component>(entity_id);

        update_button_visibility(entity_id, graphics);

        // Check if mouse is hovering over the button
        bool is_hovered = ESS.Mouse_Over_AABB(
            transform.position.x,
            transform.position.y,
            transform.scale.x,
            transform.scale.y,
            world_mouse_pos.x,
            world_mouse_pos.y
        );

        std::string base_texture = button_textures.at(entity_name);
        std::string button_audio = button_sounds.at(entity_name);
        bool transit_to_play = false;

        update_button_visibility(entity_id, graphics);

        if (graphics.color.a < 1.0f) {
            graphics.texture_name = base_texture + "_NORMAL";

            //set it to true to avoid playing the audio
            tutorial_sound_playing[entity_name] = true;
            return;
        }

        if (is_hovered) {
            if (IM.is_mouse_button_held(GLFW_MOUSE_BUTTON_LEFT)) {
                graphics.texture_name = base_texture + "_PRESSED";
                if (tutorial_sound_playing[entity_name] == false) {
                    ADM.play_now(entity_id, button_audio, audio);
                    tutorial_sound_playing[entity_name] = true;
                }

                if (entity_name == "a_button") {
                    std::cout << "a_button press" << std::endl;
                    set_tutorial_page(tutorial_page - 1);
                    return;
                }
                else if (entity_name == "d_button") {
                    std::cout << "d_button pressed" << std::endl;
                    set_tutorial_page(tutorial_page + 1);
                    return;
                }
                else if (entity_name == "e_long_button") {
                    if (tutorial_page == 10) {
                        transitioning = true;
                        return;
                    }
                }
            }
            else {
                tutorial_sound_playing[entity_name] = false;
                graphics.texture_name = base_texture + "_HIGHLIGHTED";
            }
        }
        
        if (is_key_just_pressed(GLFW_KEY_A)) {
            std::cout << "KEY A IS PRESSED" << std::endl
                << entity_name << std::endl;

            EntityID entity = ECSM.find_entity_by_name("a_button");
            std::string name = ECSM.get_entity(entity)->get_name();
        
            if (name == "a_button") {
                std::cout << "a_button press" << std::endl;
                if (tutorial_sound_playing[entity_name] == false) {
                    ADM.play_now(entity_id, button_audio, audio);
                    tutorial_sound_playing[entity_name] = true;
                }
                set_tutorial_page(tutorial_page - 1);
                return;
            }
        }

        else if (is_key_just_pressed(GLFW_KEY_D)) {
            std::cout << "KEY D IS PRESSED" << std::endl
            << entity_name << std::endl;

            EntityID entity = ECSM.find_entity_by_name("d_button");
            std::string name = ECSM.get_entity(entity)->get_name();

            if (name == "d_button") {
                std::cout << "d_button_presS" << std::endl;
                if (tutorial_sound_playing[entity_name] == false) {
                    ADM.play_now(entity_id, button_audio, audio);
                    tutorial_sound_playing[entity_name] = true;
                }
                set_tutorial_page(tutorial_page + 1);
                return;
            }
        }

        else if (is_key_just_pressed(GLFW_KEY_E)) {
            std::cout << "KEY E IS PRESSED" << std::endl
            << entity_name << std::endl;

            EntityID entity = ECSM.find_entity_by_name("e_long_button");
            std::string name = ECSM.get_entity(entity)->get_name();

            if (name == "e_long_button") {
                std::cout << "e_button_press" << std::endl;
                if (tutorial_sound_playing[entity_name] == false) {
                    ADM.play_now(entity_id, button_audio, audio);
                    tutorial_sound_playing[entity_name] = true;
                    set_tutorial_page(tutorial_page + 1);
                    return;
                }
            }
        }
        else {
            //std::cout << "no changes is deteced" << std::endl;
            graphics.texture_name = base_texture + "_NORMAL";
            tutorial_sound_playing[entity_name] = false;
        }

        //if (entity_name == "a_button") {
        //    if (is_key_just_pressed(GLFW_KEY_A)) {
        //        graphics.texture_name = base_texture + "_PRESSED";
        //        if (tutorial_sound_playing[entity_name] == false) {
        //            ADM.play_now(entity_id, button_audio, audio);
        //            tutorial_sound_playing[entity_name] = true;
        //        }
        //        set_tutorial_page(tutorial_page - 1);
        //        return;
        //    }
        //    else if (is_key_just_released(GLFW_KEY_A)) {
        //        tutorial_sound_playing[entity_name] = false;
        //        graphics.texture_name = base_texture + "_NORMAL";
        //        return;
        //    }
        //}
        //else if (entity_name == "d_button") {
        //    if (is_key_just_pressed(GLFW_KEY_D)) {
        //        graphics.texture_name = base_texture + "_PRESSED";
        //        if (tutorial_sound_playing[entity_name] == false) {
        //            ADM.play_now(entity_id, button_audio, audio);
        //            tutorial_sound_playing[entity_name] = true;
        //        }
        //        set_tutorial_page(tutorial_page - 1);
        //    }
        //    else if (is_key_just_released(GLFW_KEY_D)) {
        //        tutorial_sound_playing[entity_name] = false;
        //        graphics.texture_name = base_texture + "_NORMAL";
        //    }
        //}
        

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
                IMGUIM.set_current_file_shown(scene_file);
                //current_cooldown = transition_cooldown;  // Set the cooldown timer
                //transitioning = true;
                return;
            }
            else {
                LM.write_log("Failed to load scene file: %s", scene_path.c_str());
            }
        }
    }

    //void Tutorial_Script::update_button(EntityID entity_id) {


    //    EntityID tutorial_bg = ECSM.find_entity_by_name("tutorial_background");

    //    if (!tutorial_bg) {
    //        auto& tutorial_bg_graphic = ECSM.get_component<Graphics_Component>(tutorial_bg);
    //    }
    //    else {
    //        std::cout << "fail to find" << std::endl;
    //    }

    //    if (current_cooldown > 0.0f) {
    //        //std::cout << "didn't update" << std::endl;
    //        return;  // Still in cooldown and transitioning
    //    }

    //    // Reset transition flag at start of frame
    //    transitioning = false;

    //    // Get mouse position in world coordinates
    //    Vec2D world_mouse_pos = ESS.Get_World_MousePos();

    //    auto* entity = ECSM.get_entity(entity_id);

    //    if (!entity) {
    //        return;
    //    }

    //    std::string entity_name = entity->get_name();

    //    static const std::unordered_map<std::string, std::string> button_textures = {
    //        {"a_button", "A_Batch_16"},
    //        {"d_button", "D_Batch_16"},
    //        {"e_long_button", "E_To_Begin_Batch_16"}
    //    };

    //    static const std::unordered_map<std::string, std::string> button_sounds = {
    //        {"a_button", "tutorial_button"},
    //        {"d_button", "tutorial_button"},
    //        {"e_long_button", "main_menu"}
    //    };

    //    //return if the entity is not a button.
    //    if (button_textures.find(entity_name) == button_textures.end()) {
    //        return;
    //    }

    //    if (!ECSM.has_component<Transform2D>(entity_id) ||
    //        !ECSM.has_component<Graphics_Component>(entity_id) ||
    //        !ECSM.has_component<Audio_Component>(entity_id)) {
    //        return;
    //    }

    //    auto& transform = ECSM.get_component<Transform2D>(entity_id);
    //    auto& graphics = ECSM.get_component<Graphics_Component>(entity_id);
    //    auto& audio = ECSM.get_component<Audio_Component>(entity_id);

    //    update_button_visibility(entity_id, graphics);

    //    // Check if mouse is hovering over the button
    //    bool is_hovered = ESS.Mouse_Over_AABB(
    //        transform.position.x,
    //        transform.position.y,
    //        transform.scale.x,
    //        transform.scale.y,
    //        world_mouse_pos.x,
    //        world_mouse_pos.y
    //    );

    //    std::string base_texture = button_textures.at(entity_name);
    //    std::string button_audio = button_sounds.at(entity_name);

    //    //dont know where this came or what it does
    //    //auto& buttons_and_associated_batches = IMGUIM.return_buttons_and_batches();
    //    //for (auto& base_textures : buttons_and_associated_batches) {
    //    //    if (entity_name == base_textures.first) {
    //    //        base_texture = base_textures.second;
    //    //    }
    //    //}

    //    bool transit_to_play = false;

    //    //reset the button texture
    //    if (graphics.color.a < 1.0f) {
    //        graphics.texture_name = base_texture + "_NORMAL";
    //        tutorial_sound_playing[entity_name] = false;
    //        return;
    //    }

    //    //reset the tutorial sound playing audio
    //    if (tutorial_sound_playing[entity_name] == true && page_transition_cooldown < 0) {
    //        tutorial_sound_playing[entity_name] = false;
    //        return;
    //    }

    //    //if the entity is a
    //    if (entity_name == "a_button") {
    //        //bool audio_play = false;
    //        if (is_hovered) {
    //            graphics.texture_name = base_texture + "_HIGHLIGHTED";

    //            if (IM.is_mouse_button_held(GLFW_MOUSE_BUTTON_LEFT)) {
    //                if (tutorial_sound_playing[entity_name] == false) {
    //                    ADM.play_now(entity_id, button_audio, audio);
    //                    tutorial_sound_playing[entity_name] = true;
    //                }
    //                graphics.texture_name = base_texture + "_PRESSED";
    //                set_tutorial_page(tutorial_page - 1);
    //            }

    //            if (left_mouse_pressed) {
    //                graphics.texture_name = base_texture + "_PRESSED";
    //            }
    //        }
    //        else {
    //            graphics.texture_name = base_texture + "_NORMAL";
    //        }

    //        if (is_key_just_pressed(GLFW_KEY_A)) {
    //            if (tutorial_sound_playing[entity_name] == false) {
    //                ADM.play_now(entity_id, button_audio, audio);
    //                tutorial_sound_playing[entity_name] = true;
    //            }
    //            graphics.texture_name = base_texture + "_PRESSED";
    //            set_tutorial_page(tutorial_page - 1);
    //        }

    //        //this is if it is pressed.
    //        if (key_a_pressed) {
    //            graphics.texture_name = base_texture + "_PRESSED";
    //        }

    //        //check if it is released
    //        /*if (!key_a_pressed && key_a_last_frame || !left_mouse_pressed && left_mouse_last_frame) {
    //            std::cout << "detecting a is released" << std::endl;
    //            tutorial_sound_playing[entity_name] = false;
    //        }*/
    //    }

    //    //if the entity is a
    //    if (entity_name == "d_button") {
    //        //bool audio_play = false;
    //        if (is_hovered) {
    //            graphics.texture_name = base_texture + "_HIGHLIGHTED";

    //            if (IM.is_mouse_button_held(GLFW_MOUSE_BUTTON_LEFT)) {
    //                if (tutorial_sound_playing[entity_name] == false) {
    //                    ADM.play_now(entity_id, button_audio, audio);
    //                    tutorial_sound_playing[entity_name] = true;
    //                }
    //                graphics.texture_name = base_texture + "_PRESSED";
    //                set_tutorial_page(tutorial_page - 1);
    //            }

    //            if (left_mouse_pressed) {
    //                graphics.texture_name = base_texture + "_PRESSED";
    //            }
    //        }
    //        else {
    //            graphics.texture_name = base_texture + "_NORMAL";
    //        }

    //        if (is_key_just_pressed(GLFW_KEY_D)) {
    //            if (tutorial_sound_playing[entity_name] == false) {
    //                ADM.play_now(entity_id, button_audio, audio);
    //                tutorial_sound_playing[entity_name] = true;
    //            }
    //            graphics.texture_name = base_texture + "_PRESSED";
    //            set_tutorial_page(tutorial_page - 1);
    //        }

    //        //this is if it is pressed.
    //        if (key_d_pressed) {
    //            graphics.texture_name = base_texture + "_PRESSED";
    //        }
    //    }

    //    if (entity_name == "e_long_button") {

    //        //check if key A is held or pressed
    //        if (key_e_pressed || is_key_just_pressed(GLFW_KEY_E)) {
    //            if (tutorial_sound_playing[entity_name] == false) {
    //                ADM.play_now(entity_id, button_audio, audio);
    //                tutorial_sound_playing[entity_name] = true;
    //            }
    //            graphics.texture_name = base_texture + "_PRESSED";
    //            transit_to_play = true;
    //        }
    //    }


        //if (graphics.color.a == 1.0f) {
            //if (is_hovered) {
            //    std::cout << "hover " << entity_name << std::endl;
            //    graphics.texture_name = base_texture + "_HIGHLIGHTED";

            //    if (is_key_just_pressed(GLFW_MOUSE_BUTTON_LEFT)) {

            //        std::cout << "TRUE ------------------------------------------------" << std::endl;
            //        if (tutorial_sound_playing[entity_name] == false) {
            //            ADM.play_now(entity_id, button_audio, audio);
            //            tutorial_sound_playing[entity_name] = true;
            //        }
            //        graphics.texture_name = base_texture + "_PRESSED";

            //        std::cout << entity_name << std::endl;

            //        if (entity_name == "a_button") {
            //            set_tutorial_page(tutorial_page - 1);
            //        } 
            //        
            //        if (entity_name == "d_button") {
            //            std::cout << "d is detected" << std::endl;
            //            set_tutorial_page(tutorial_page + 1);
            //        } 
            //        
            //        if (entity_name == "e_long_button") {
            //            if (tutorial_page == 10) {
            //                transit_to_play = true;
            //            }
            //        }
            //    }
            //    else {
            //        std::cout << "FALSE" << std::endl;
            //        //mouse is hovering over the button therefore change it to highlight
            //        //graphics.texture_name = base_texture + "_HIGHLIGHTED";
            //        //set the audio playing to false as no audio feedback is needed.
            //        tutorial_sound_playing[entity_name] = false;
            //        return;
            //    }
            //}
            //else {
            //    //reset to normal state texture
            //    tutorial_sound_playing[entity_name] = false;
            //    graphics.texture_name = base_texture + "_NORMAL";
            //}

            //if (entity_name == "a_button") {

            //    //check if key A is held or pressed
            //    /*if (key_a_pressed || is_key_just_pressed(GLFW_KEY_A)) {
            //        if (tutorial_sound_playing[entity_name] == false) {
            //            ADM.play_now(entity_id, button_audio, audio);
            //            tutorial_sound_playing[entity_name] = true;
            //        }
            //        graphics.texture_name = base_texture + "_PRESSED";
            //        set_tutorial_page(tutorial_page - 1);
            //    }*/

            //    if (tutorial_sound_playing[entity_name] == false) {
            //        //check if key A is held or pressed
            //        if (key_a_pressed || is_key_just_pressed(GLFW_KEY_A)) {
            //            ADM.play_now(entity_id, button_audio, audio);
            //            tutorial_sound_playing[entity_name] = true;
            //            graphics.texture_name = base_texture + "_PRESSED";
            //            set_tutorial_page(tutorial_page - 1);
            //        }
            //    }
            //}
            //
            //if (entity_name == "d_button") {

            //    //check if key A is held or pressed
            //    /*if (key_d_pressed || is_key_just_pressed(GLFW_KEY_D)) {
            //        if (tutorial_sound_playing[entity_name] == false) {
            //            ADM.play_now(entity_id, button_audio, audio);
            //            tutorial_sound_playing[entity_name] = true;
            //        }
            //        graphics.texture_name = base_texture + "_PRESSED";
            //        set_tutorial_page(tutorial_page + 1);
            //    }*/

            //    if (tutorial_sound_playing[entity_name] == false) {
            //        //check if key A is held or pressed
            //        if (key_d_pressed || is_key_just_pressed(GLFW_KEY_D)) {
            //            ADM.play_now(entity_id, button_audio, audio);
            //            tutorial_sound_playing[entity_name] = true;
            //            graphics.texture_name = base_texture + "_PRESSED";
            //            set_tutorial_page(tutorial_page + 1);
            //        }
            //    }
            //}
            //
            //if (entity_name == "e_long_button") {

            //    //check if key A is held or pressed
            //    if (key_e_pressed || is_key_just_pressed(GLFW_KEY_E)) {
            //        if (tutorial_sound_playing[entity_name] == false) {
            //            ADM.play_now(entity_id, button_audio, audio);
            //            tutorial_sound_playing[entity_name] = true;
            //        }
            //        graphics.texture_name = base_texture + "_PRESSED";
            //        transit_to_play = true;
            //    }
            //}
        //}
        //else {
        //    graphics.texture_name = base_texture + "_NORMAL";
        //    tutorial_sound_playing[entity_name] = false;
        //}

        //



    //    if (transit_to_play) {
    //        //LM.write_log("Play button held - attempting scene transition");

    //        // Clear dynamic entities first
    //        bool found_movement_system = false;
    //        for (auto& system : ECSM.get_systems()) {
    //            if (auto* movement_system = dynamic_cast<Movement_System*>(system.get())) {
    //                movement_system->clear_dynamic_entities();
    //                found_movement_system = true;
    //                LM.write_log("Found and cleared Movement System");
    //                break;
    //            }
    //        }
    //        if (!found_movement_system) {
    //            LM.write_log("Warning: Movement System not found");
    //        }

    //        // Set up scene loading
    //        const std::string SCENES = "Scenes";
    //        std::string scene_file = "scene2.scn";
    //        std::string scene_path = ASM.get_full_path(SCENES, scene_file);
    //        LM.write_log("Attempting to load scene from path: %s", scene_path.c_str());

    //        // Try to load scene2
    //        if (SM.load_scene(scene_path.c_str())) {
    //            LM.write_log("Scene loaded successfully");

    //            // Reset camera position
    //            auto& camera = GFXM.get_camera();
    //            camera.pos_x = DEFAULT_CAMERA_POS_X;
    //            camera.pos_y = DEFAULT_CAMERA_POS_Y;

    //            // Reset player position if it exists
    //            EntityID playerId = ECSM.find_entity_by_name(DEFAULT_PLAYER_NAME);
    //            if (playerId != INVALID_ENTITY_ID) {
    //                if (ECSM.has_component<Transform2D>(playerId)) {
    //                    auto& player_transform = ECSM.get_component<Transform2D>(playerId);
    //                    player_transform.position = Vec2D(0.0f, 0.0f);
    //                    player_transform.prev_position = player_transform.position;
    //                }
    //                if (ECSM.has_component<Velocity_Component>(playerId)) {
    //                    auto& velocity = ECSM.get_component<Velocity_Component>(playerId);
    //                    velocity.velocity = Vec2D(0.0f, 0.0f);
    //                }
    //            }

    //            // Update current scene in Game Manager
    //            GM.set_current_scene(2);

    //            // Update IMGUI Manager's current file
    //            IMGUIM.set_current_file_shown(scene_file);
    //            current_cooldown = transition_cooldown;  // Set the cooldown timer
    //            transitioning = true;
    //            return;
    //        }
    //        else {
    //            LM.write_log("Failed to load scene file: %s", scene_path.c_str());
    //        }
    //    }
    //}


} // namespace lof