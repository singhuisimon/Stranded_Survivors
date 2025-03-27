/**
 * @file Sound_Setting_Script.cpp
 * @brief Implements the script class for the Sound Setting logic.
 * @details Initialize and update the script 
 * @author Chua Wen Bin Kenny (100%)
 * @date March 11, 2025
 * Copyright (C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

 // Include header file
#include "../Scripts/Sound_Setting_Script.h"
#include "../Manager/Log_Manager.h"
#include "../Manager/ECS_Manager.h"
#include "../Manager/FPS_Manager.h"

float master_multiplier = 1.0f;
float bgm_multiplier = 1.0f;
float sfx_multiplier = 1.0f;

namespace lof {

    Sound_Setting_Script::Sound_Setting_Script() {
        master_size = master_multiplier;
        bgm_size = bgm_multiplier;
        sfx_size = sfx_multiplier;
        bar_x_axis = -53.0f;
        x_modifier = 3.0f;
        width_modifier = 2.0f;

        //set to invalid
        master_id = INVALID_ENTITY_ID;
        bgm_id = INVALID_ENTITY_ID;
        sfx_id = INVALID_ENTITY_ID;
        back_button = INVALID_ENTITY_ID;

    }

    // Returns the script name as a string
    std::string Sound_Setting_Script::get_type() const {
        return script_name;
    }

    // Register the script by adding functions into it
    void Sound_Setting_Script::register_script() {
        auto sound_setting_script = shared_from_this();

        sound_setting_script->add_function("init", [weak_script = std::weak_ptr<Sound_Setting_Script>(sound_setting_script)](EntityID entity_id) {
            //(void)entity_id;
            auto sound_setting_script = weak_script.lock();
            sound_setting_script->init_entity();
            sound_setting_script->update_setting(entity_id);
            });

        sound_setting_script->add_function("update", [weak_script = std::weak_ptr<Sound_Setting_Script>(sound_setting_script)](EntityID entity_id) {
            //(void)entity_id;
            auto sound_setting_script = weak_script.lock();
            sound_setting_script->update_setting(entity_id);
            sound_setting_script->check_back_press(entity_id);

            });

    }

    bool Sound_Setting_Script::check_entity(EntityID entity_id) {
        return entity_id == master_id || entity_id == bgm_id || entity_id == sfx_id;
    }

    void Sound_Setting_Script::init_entity() {
        master_id = ECSM.find_entity_by_name("master_bar");
        bgm_id = ECSM.find_entity_by_name("bgm_bar");
        sfx_id = ECSM.find_entity_by_name("sfx_bar");
        back_button = ECSM.find_entity_by_name("back_button");
    }

    void Sound_Setting_Script::update_setting(EntityID entity_id) {
        update_multiplier();
        update_audio_bar(entity_id);
        update_audio_group();
    }

    void Sound_Setting_Script::update_multiplier() {

        EntityID master_plus_id = ECSM.find_entity_by_name("master_plus");
        EntityID master_minus_id = ECSM.find_entity_by_name("master_minus");
        EntityID bgm_plus_id = ECSM.find_entity_by_name("bgm_plus");
        EntityID bgm_minus_id = ECSM.find_entity_by_name("bgm_minus");
        EntityID sfx_plus_id = ECSM.find_entity_by_name("sfx_plus");
        EntityID sfx_minus_id = ECSM.find_entity_by_name("sfx_minus");

        Vec2D world_mouse_pos = ESS.Get_World_MousePos();

        //Ensure world_mouse_pos is in terms of viewport in level editor
        if (level_editor_mode) {
            world_mouse_pos.x = IMGUIM.imgui_mouse_pos().x;
            world_mouse_pos.y = IMGUIM.imgui_mouse_pos().y;
        }

        std::vector<EntityID> buttons = { master_minus_id, master_plus_id, bgm_minus_id, bgm_plus_id, sfx_minus_id, sfx_plus_id };

        for (EntityID entity_id : buttons) {
            if (entity_id == INVALID_ENTITY_ID) {
                continue;
            }

            auto entity = ECSM.get_entity(entity_id);
            if (!entity) {
                continue;
            }

            std::string entity_name = entity->get_name();

            auto& transform = ECSM.get_component<Transform2D>(entity_id);
            auto& graphics = ECSM.get_component<Graphics_Component>(entity_id);
            auto& audio = ECSM.get_component<Audio_Component>(entity_id);

            if (!ECSM.has_component<Transform2D>(entity_id) ||
                !ECSM.has_component<Graphics_Component>(entity_id) ||
                !ECSM.has_component<Audio_Component>(entity_id)) {
                return;
            }

            // Check if mouse is hovering over the button
            bool is_hovered = ESS.Mouse_Over_AABB(
                transform.position.x,
                transform.position.y,
                transform.scale.x,
                transform.scale.y,
                world_mouse_pos.x,
                world_mouse_pos.y
            );

            std::string base_texture;
            std::string hover_sound = "button_hover";


            if (entity_name == "master_minus") {
                base_texture = "Gold_Minus_Batch_23";
            }
            else if (entity_name == "master_plus") {
                base_texture = "Gold_Plus_Batch_23";
            }
            else if (entity_name == "bgm_minus" || entity_name == "sfx_minus") {
                base_texture = "Blue_Minus_Batch_23";
            }
            else if (entity_name == "bgm_plus" || entity_name == "sfx_plus") {
                base_texture = "Blue_Plus_Batch_23";
            }

            if (is_hovered) {
                //std::cout << "hovered" << std::endl;

                if (GM.is_mouse_left_held() || GM.is_mouse_left_pressed()) {
                    graphics.texture_name = base_texture + "_PRESSED";
                }

                if (GM.is_mouse_left_pressed() && !button_hover_states[entity_name]) {
                    ADM.play_now(entity_id, hover_sound, audio);
                    button_hover_states[entity_name] = true;
                }

                if (GM.is_mouse_left_released()) {
                    if (entity_name == "master_plus" && master_multiplier < 1.0f) {
                        master_multiplier += 0.1f;
                    }
                    else if (entity_name == "master_minus" && master_multiplier > 0.0f) {
                        master_multiplier -= 0.1f;
                    }
                    else if (entity_name == "bgm_plus" && bgm_multiplier < 1.0f) {
                        bgm_multiplier += 0.1f;
                    }
                    else if (entity_name == "bgm_minus" && bgm_multiplier > 0.0f) {
                        bgm_multiplier -= 0.1f;
                    }
                    else if (entity_name == "sfx_plus" && sfx_multiplier < 1.0f) {
                        sfx_multiplier += 0.1f;
                    }
                    else if (entity_name == "sfx_minus" && sfx_multiplier > 0.0f) {
                        sfx_multiplier -= 0.1f;
                    }
                    button_hover_states[entity_name] = false;
                }
                else {
                    graphics.texture_name = base_texture + "_HIGHLIGHTED";
                }
            }
            else {
                graphics.texture_name = base_texture + "_NORMAL";
                button_hover_states[entity_name] = false;
            }
        }
    }

    void Sound_Setting_Script::update_audio_bar(EntityID entity_id) {

        //update x_setting and bar width
        if (master_id != INVALID_ENTITY_ID && ECSM.has_component<Transform2D>(master_id)) {
            auto& transform_comp = ECSM.get_component<Transform2D>(master_id);
            float new_x = bar_x_axis + (master_multiplier * 10) * PER_BAR_X_AXIS;
            float new_width = (master_multiplier * 10) * PER_BAR_WIDTH;
            if (master_multiplier == 1.0f) {
                new_x -= x_modifier;
                new_width -= width_modifier;
            }
            transform_comp.position = Vec2D(new_x, transform_comp.position.y);
            transform_comp.scale = Vec2D(new_width, transform_comp.scale.y);
            /*Vec2D prev = transform_comp.position;
            float new_x = bar_x_axis;
            float new_width = bar_width;
            if (master_multiplier <= 0.9 && master_multiplier >= 0.1f) {
                new_x += (master_multiplier * 10) * PER_BAR_X_AXIS;
                new_width = (master_multiplier * 10) * PER_BAR_WIDTH;
            }
            else if (master_multiplier == 1.0f) {
                new_x += (master_multiplier * 10) * PER_BAR_X_AXIS;
                new_x -= 3;
                new_width = (master_multiplier * 10) * PER_BAR_WIDTH;
                new_width -= 2;
            }
            transform_comp.position = Vec2D(new_x, prev.y);
            transform_comp.scale = Vec2D(new_width, transform_comp.scale.y);*/
        }
        
        if (bgm_id != INVALID_ENTITY_ID && ECSM.has_component<Transform2D>(bgm_id)) {
            auto& transform_comp = ECSM.get_component<Transform2D>(bgm_id);
            float new_x = bar_x_axis + (bgm_multiplier * 10) * PER_BAR_X_AXIS;
            float new_width = (bgm_multiplier * 10) * PER_BAR_WIDTH;
            if (bgm_multiplier == 1.0f) {
                new_x -= x_modifier;
                new_width -= width_modifier;
            }
            transform_comp.position = Vec2D(new_x, transform_comp.position.y);
            transform_comp.scale = Vec2D(new_width, transform_comp.scale.y);
        }

        if (sfx_id != INVALID_ENTITY_ID && ECSM.has_component<Transform2D>(sfx_id)) {
            auto& transform_comp = ECSM.get_component<Transform2D>(sfx_id);
            float new_x = bar_x_axis + (sfx_multiplier * 10) * PER_BAR_X_AXIS;
            float new_width = (sfx_multiplier * 10) * PER_BAR_WIDTH;
            if (sfx_multiplier == 1.0f) {
                new_x -= x_modifier;
                new_width -= width_modifier;
            }
            transform_comp.position = Vec2D(new_x, transform_comp.position.y);
            transform_comp.scale = Vec2D(new_width, transform_comp.scale.y);
        }
    }

    void Sound_Setting_Script::update_audio_group() {
        ADM.set_group_volume(GroupType::TYPE_MASTER, master_multiplier);
        ADM.set_group_volume(GroupType::TYPE_BGM, bgm_multiplier);
        ADM.set_group_volume(GroupType::TYPE_SFX, sfx_multiplier);
        ADM.set_group_volume(GroupType::TYPE_UI, sfx_multiplier);
    }

    void Sound_Setting_Script::check_back_press(EntityID entity_id) {
        
        Vec2D world_mouse_pos = ESS.Get_World_MousePos();

        //Ensure world_mouse_pos is in terms of viewport in level editor
        if (level_editor_mode) {
            world_mouse_pos.x = IMGUIM.imgui_mouse_pos().x;
            world_mouse_pos.y = IMGUIM.imgui_mouse_pos().y;
        }

        std::string entity_name = ECSM.get_entity(entity_id)->get_name();

        if (entity_name == "back_button") {
            if (!back_button) return;

            if (!ECSM.has_component<Transform2D>(back_button) ||
                !ECSM.has_component<Graphics_Component>(back_button) ||
                !ECSM.has_component<Audio_Component>(back_button)) return;

            auto& transform = ECSM.get_component<Transform2D>(back_button);
            auto& graphics = ECSM.get_component<Graphics_Component>(back_button);
            auto& audio = ECSM.get_component<Audio_Component>(back_button);

            bool is_hovered = ESS.Mouse_Over_AABB(
                transform.position.x,
                transform.position.y,
                transform.scale.x,
                transform.scale.y,
                world_mouse_pos.x,
                world_mouse_pos.y
            );

            std::string base_texture = "Back_Batch_14";
            std::string hover_sound = "button_hover";
            std::string click_sound = "main_menu";

            //Update button batch textures in the level editor
            auto& buttons_and_associated_batches = IMGUIM.return_buttons_and_batches();
            for (auto& base_textures : buttons_and_associated_batches) {
                if (entity_name == base_textures.first) {
                    base_texture = base_textures.second;
                }
            }

            if (is_hovered) {
                if (!button_hover_states[entity_name]) {
                    // Play hover sound
                    ADM.play_now(entity_id, hover_sound, audio);
                    button_hover_states[entity_name] = true;  // Prevent playing repeatedly
                }

                if (GM.is_mouse_left_held() || GM.is_mouse_left_pressed()) {
                    graphics.texture_name = base_texture + "_PRESSED";
                }

                if (GM.is_mouse_left_pressed()) {
                    ADM.play_now(entity_id, click_sound, audio);
                }

                if (GM.is_mouse_left_released()) {
                    LM.write_log("Back button held - returning to main menu");

                    // Clear dynamic entities first
                    bool found_movement_system = false;
                    for (auto& system : ECSM.get_systems()) {
                        if (auto* movement_system = dynamic_cast<Movement_System*>(system.get())) {
                            movement_system->clear_dynamic_entities();
                            found_movement_system = true;
                            break;
                        }
                    }
                    if (!found_movement_system) {
                        LM.write_log("Warning: Movement System not found");
                    }

                    const std::string SCENES = "Scenes";
                    std::string scene_file = "main_menu.scn";
                    std::string scene_path = ASM.get_full_path(SCENES, scene_file);

                    if (SM.load_scene(scene_path.c_str())) {
                        LM.write_log("Main menu scene loaded successfully");

                        // Reset camera position
                        auto& camera = GFXM.get_camera();
                        camera.pos_x = DEFAULT_CAMERA_POS_X;
                        camera.pos_y = DEFAULT_CAMERA_POS_Y;

                        // Update current scene and IMGUI
                        GM.set_current_scene(0);
                        IMGUIM.set_current_file_shown(scene_file);
                        return;
                    }
                    else {
                        LM.write_log("Failed to load main menu scene: %s", scene_path.c_str());
                    }
                }
                else {
                    graphics.texture_name = base_texture + "_HIGHLIGHTED";
                }
            }
            else {
                graphics.texture_name = base_texture + "_NORMAL";
                button_hover_states[entity_name] = false;
            }
        }
    }

} // namespace lof