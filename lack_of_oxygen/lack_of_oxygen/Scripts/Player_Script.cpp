/**
 * @file Player_Script.cpp
 * @brief Declare the player_script class
 * @author Amanda Leow Boon Suan (40%), Wai Lwin Thit (20%), Chua Wen Bin Kenny (20%), Saw Hui Shan (20%)
 * @date February 2, 2025
 * Copyright (C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#include "../Scripts/Player_Script.h"
#include "../System/Particle_System.h"
#include "../Manager/Audio_Manager.h"
#include "../Manager/ECS_Manager.h"
#include "../Manager/Graphics_Manager.h"
#include "../System/GUI_System.h"

#include <GLFW/glfw3.h>

namespace lof {

    Player_Script::Player_Script() {
        //entity_data = std::unordered_map<EntityID, MovementData>();
        key_space_last_frame = false;
        key_space_pressed = false;
        key_a_pressed = false;
        key_d_pressed = false;
        key_a_last_frame = false;
        key_d_last_frame = false;
        forces_flag = -1;
        player_id = 0;

        f_mag_original = DEFAULT_LR_FORCE_MAG;
        panic_level = 0.0f;
        
        key_e_last_frame = false;
        key_e_pressed = false;

        key_t_last_frame = false;
        key_t_pressed = false;
        teleport_flag = false;

        is_inside = false;
        //found_wormhole = false;
        key_t_last_frame = false;
        key_t_pressed = false;

        key_y_last_frame = false;
        key_y_pressed = false;

        key_u_last_frame = false;
        key_u_pressed = false;

    }

    std::string Player_Script::get_type() const {
        return script_name;
    }

    void Player_Script::register_script() {

        auto player_script = shared_from_this();

        player_script->add_function("init", [weak_script = std::weak_ptr<Player_Script>(player_script)](EntityID entity_id) {
            (void)entity_id;
            auto player_script = weak_script.lock();
            player_script->set_player_id(ECSM.find_entity_by_name(DEFAULT_PLAYER_NAME));
            player_script->set_force_flag(-1);
            });

        player_script->add_function("update", [weak_script = std::weak_ptr<Player_Script>(player_script)](EntityID entity_id) {
            auto player_script = weak_script.lock();
            if (!entity_id || !ECSM.has_component<Physics_Component>(entity_id) || !ECSM.has_component<Audio_Component>(entity_id) ||
                !ECSM.has_component<Transform2D>(entity_id)) {
                LM.write_log("Player_Script::register_script(): Entity %d does not have required components.", entity_id);
                return;
            }

            if (entity_id != player_script->get_player_id()) {
                LM.write_log("Player_Script::register_script(): Entity %d is not the player.", entity_id);
                return;
            }

            auto& physics_comp = ECSM.get_component<Physics_Component>(entity_id);
            auto& audio_comp = ECSM.get_component<Audio_Component>(entity_id);

            //update the player's movement forces before updating the player's movement
            player_script->update_player_panic_speed(physics_comp);

            player_script->check_keys();

            //update player horizontal movement
            player_script->update_player_movement(physics_comp);
            player_script->update_player_audio(physics_comp, audio_comp);
            player_script->update_player_animation();
            //std::cout << "Wormhole pair erased for entity " << entity_id << "\n";


            player_script->handle_teleportation(entity_id);
            });

        //LGS.add_script("player_script", player_script);
    }

    void Player_Script::check_keys() {
        //check for keys here
        key_space_last_frame = key_space_pressed;
        key_a_last_frame = key_a_pressed;
        key_d_last_frame = key_d_pressed;

        key_space_pressed = IM.is_key_held(GLFW_KEY_SPACE);
        key_a_pressed = IM.is_key_held(GLFW_KEY_A);
        key_d_pressed = IM.is_key_held(GLFW_KEY_D);

        key_e_last_frame = key_e_pressed;
        key_e_pressed = IM.is_key_held(GLFW_KEY_E);

        key_t_last_frame = key_t_pressed;
        key_t_pressed = IM.is_key_held(GLFW_KEY_T); //cheap code for teleport

        key_y_last_frame = key_y_pressed;
        key_y_pressed = IM.is_key_held(GLFW_KEY_Y); //cheap code for teleport

        key_u_last_frame = key_u_pressed;
        key_u_pressed = IM.is_key_held(GLFW_KEY_U); //cheap code for teleport

    }

    bool Player_Script::is_key_just_pressed(int key) {
        if (key == GLFW_KEY_SPACE) {
            return key_space_pressed && !key_space_last_frame;
        }
        else if (key == GLFW_KEY_A) {
            return key_a_pressed && !key_a_last_frame;
        }
        else if (key == GLFW_KEY_D) {
            return key_d_pressed && !key_d_last_frame;
        }
        else if (key == GLFW_KEY_E) {
            return key_e_pressed && !key_e_last_frame;

        }
        else if (key == GLFW_KEY_T)
        {
            return key_t_pressed && !key_e_last_frame;
        }
        else if (key == GLFW_KEY_T)
        {
            return key_t_pressed && !key_e_last_frame;
        }
        else if (key == GLFW_KEY_Y)
        {
            return key_y_pressed && !key_y_last_frame;
        }
        else if (key == GLFW_KEY_U)
        {
            return key_u_pressed && !key_u_last_frame;
        }
        return false;
    }

    void Player_Script::set_force_flag(int flag) {
        forces_flag = flag;
    }

    int Player_Script::get_force_flag() const {
        return forces_flag;
    }

    void Player_Script::set_player_id(EntityID entityid) {
        player_id = entityid;
    }

    EntityID Player_Script::get_player_id() const {
        return player_id;
    }

    void Player_Script::update_player_movement(Physics_Component& physic_comp) {

        // Handle horizontal movement
        if (is_key_just_pressed(GLFW_KEY_SPACE)) {
            physic_comp.set_jump_requested(true); //this will set the flag to true inside the physics_component 
        }
        else {
            physic_comp.set_jump_requested(false);
        }

        //activate and deactivate the forces. 
        if (IM.is_key_held(GLFW_KEY_A) && !(IM.is_key_held(GLFW_KEY_D))) {
            // Updates forces
            physic_comp.force_helper.deactivate_force(MOVE_RIGHT);
            physic_comp.force_helper.activate_force(MOVE_LEFT);
            forces_flag = MOVE_LEFT;

            //std::cout << "moving left current scene number is " << current_scene << std::endl;
        }
        else if (IM.is_key_held(GLFW_KEY_D) && !(IM.is_key_held(GLFW_KEY_A))) {
            // Update forces
            physic_comp.force_helper.deactivate_force(MOVE_LEFT);
            physic_comp.force_helper.activate_force(MOVE_RIGHT);
            forces_flag = MOVE_RIGHT;

        }
        else if (IM.is_key_held(GLFW_KEY_D) && IM.is_key_held(GLFW_KEY_A)) {
            if (forces_flag == MOVE_LEFT) {
                // Update forces
                physic_comp.force_helper.activate_force(MOVE_LEFT);
                forces_flag = MOVE_LEFT;
            }
            else {
                // Update forces
                physic_comp.force_helper.deactivate_force(MOVE_LEFT);
                physic_comp.force_helper.activate_force(MOVE_RIGHT);
                forces_flag = MOVE_RIGHT;
            }
        }
        else {
            // Reset forces
            physic_comp.force_helper.deactivate_force(MOVE_LEFT);
            physic_comp.force_helper.deactivate_force(MOVE_RIGHT);
            forces_flag = -1;
        }
    }

    void Player_Script::update_player_animation() {

        //activate and deactivate the forces. 
        if (IM.is_key_held(GLFW_KEY_A) && !(IM.is_key_held(GLFW_KEY_D))) {

            // Update player animation flag
            int& direction = GFXM.get_player_direction();
            direction = FACE_LEFT;
            int& moving_status = GFXM.get_moving_status();
            moving_status = RUN_LEFT;

            //std::cout << "moving left current scene number is " << current_scene << std::endl;
        }
        else if (IM.is_key_held(GLFW_KEY_D) && !(IM.is_key_held(GLFW_KEY_A))) {

            // Update player animation flag
            int& direction = GFXM.get_player_direction();
            direction = FACE_RIGHT;
            int& moving_status = GFXM.get_moving_status();
            moving_status = RUN_RIGHT;

        }
        else if (IM.is_key_held(GLFW_KEY_D) && IM.is_key_held(GLFW_KEY_A)) {
            if (forces_flag == MOVE_LEFT) {
                // Update player animation flag
                int& direction = GFXM.get_player_direction();
                direction = FACE_LEFT;
                int& moving_status = GFXM.get_moving_status();
                moving_status = RUN_LEFT;
            }
            else {
                // Update player animation flag
                int& direction = GFXM.get_player_direction();
                direction = FACE_RIGHT;
                int& moving_status = GFXM.get_moving_status();
                moving_status = RUN_RIGHT;
            }
        }
        else {
            // Reset player animation
            int& moving_status = GFXM.get_moving_status();
            moving_status = NO_ACTION;

        }


        //player mining animation
        if (IM.is_key_held(GLFW_KEY_LEFT) && !(IM.is_key_held(GLFW_KEY_RIGHT))
            && !(IM.is_key_held(GLFW_KEY_UP)) && !(IM.is_key_held(GLFW_KEY_DOWN))) {
            auto& mining_status = GFXM.get_mining_status();
            mining_status = MINE_LEFT;

        }
        else if (IM.is_key_held(GLFW_KEY_RIGHT) && !(IM.is_key_held(GLFW_KEY_LEFT))
            && !(IM.is_key_held(GLFW_KEY_UP)) && !(IM.is_key_held(GLFW_KEY_DOWN))) {
            auto& mining_status = GFXM.get_mining_status();
            mining_status = MINE_RIGHT;

        }
        else if (IM.is_key_held(GLFW_KEY_UP) && !(IM.is_key_held(GLFW_KEY_LEFT))
            && !(IM.is_key_held(GLFW_KEY_RIGHT)) && !(IM.is_key_held(GLFW_KEY_DOWN))) {
            auto& mining_status = GFXM.get_mining_status();
            mining_status = MINE_UP;

        }
        else if (IM.is_key_held(GLFW_KEY_DOWN) && !(IM.is_key_held(GLFW_KEY_LEFT))
            && !(IM.is_key_held(GLFW_KEY_RIGHT)) && !(IM.is_key_held(GLFW_KEY_UP))) {
            auto& mining_status = GFXM.get_mining_status();
            mining_status = MINE_DOWN;

        }
        else {
            if (!IM.is_key_held(GLFW_KEY_RIGHT) && !IM.is_key_held(GLFW_KEY_LEFT) &&
                !IM.is_key_held(GLFW_KEY_UP) && !IM.is_key_held(GLFW_KEY_DOWN)) {
                auto& mining_status = GFXM.get_mining_status();
                mining_status = NO_ACTION;
            }
        }
    }

    float teleport_audio_end_time = 0.0f;
    void Player_Script::update_player_audio(Physics_Component& physic_comp, Audio_Component& audio_comp) {
        //audio logic is here.
        if (forces_flag != -1) {
            if (physic_comp.get_is_grounded()) {
                if (forces_flag == MOVE_RIGHT || forces_flag == MOVE_LEFT) {

                    ADM.play_now(player_id, "moving", audio_comp);
                    //audio_comp.set_isactive("moving", true);
                    //audio_comp.increase_playcount("moving");
                    update_player_walking_particle();
                }
            }
        }
        else {
            if (!is_key_just_pressed(GLFW_KEY_D) || !is_key_just_pressed(GLFW_KEY_A)) {
                ADM.stop_now(player_id, "moving", audio_comp.get_filepath("moving"));
                //audio_comp.set_isactive("moving", false);
            }
        }

        float current_time = static_cast<float>(glfwGetTime());
        if (teleport_flag) {
            ADM.play_now(player_id, "tunneling", audio_comp);
            teleport_audio_end_time = current_time + 1.5f;  // set a 1.5 seconds for the sound to finish
            teleport_flag = false;  // Reset teleport flag immediately to prevent retriggering
        }

        // **Stop the teleport sound after it has played fully
        if (current_time >= teleport_audio_end_time) {
            ADM.stop_now(player_id, "tunneling", audio_comp.get_filepath("tunneling"));
        }

    }

    void Player_Script::update_player_walking_particle() {
        //animation logic is here.

        for (auto& system : ECSM.get_systems()) {
            if (system->get_type() == "Particle_System") {
                auto* particle_system = dynamic_cast<Particle_System*>(system.get());
                if (!particle_system) {
                    LM.write_log("Player_Script::update_player_animation(): Failed to get Particle_System.");
                    return;
                }
                // Emit walking dirt particles
                auto& player_transform = ECSM.get_component<Transform2D>(player_id);
                float part_x = player_transform.position.x - (player_transform.scale.x / 2.0f) + (particle_system->get_rand_float() * player_transform.scale.x);
                float part_y = player_transform.position.y - (player_transform.scale.y * 0.45f);
                particle_system->particle_emit("walking", Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f));

            }

        }

    }


    //Panic Script for movement speed
    void Player_Script::update_player_panic_speed(Physics_Component& physics_comp) {
        //get the panic level fro Game Manager
        panic_level = GM.get_current_panic_level();
        update_movement_forces(physics_comp);
    }

    void Player_Script::update_movement_forces(Physics_Component& physics_comp) {
       
        float panic_multiplier = 1.0f + (panic_level / 100.0f);
        float new_force_magnitude = f_mag_original * panic_multiplier; 

        //update force magnitudes for movement
        for (auto& force : physics_comp.force_helper.get_forces()) {
            if (force.type == MOVE_LEFT || force.type == MOVE_RIGHT) {

                //update the magnitude for movement forces
                const_cast<Force&>(force).magnitude = new_force_magnitude;
            }
        }
    }

    //==============================================================//
    void Player_Script::handle_teleportation(EntityID Player_ID) {
        auto& player_transform = ECSM.get_component<Transform2D>(Player_ID);
        std::vector<EntityID>& wormhole_entity = SM.get_wormholes_id();

        // Clear the previous pair
        wormhole_pairs.clear();

        if (wormhole_entity.size() % 2 == 0) {
            for (size_t i = 0; i < wormhole_entity.size(); i += 2) {
                wormhole_pairs[wormhole_entity[i]] = wormhole_entity[i + 1];
                wormhole_pairs[wormhole_entity[i + 1]] = wormhole_entity[i];
            }
        }

        bool was_inside = is_inside; // Store previous state
        is_inside = false; // Reset is_inside flag
        active_wormhole = INVALID_ENTITY_ID;

        // Check if player is inside any wormhole
        for (const auto& pair : wormhole_pairs) {
            EntityID wormhole_id = pair.first;
            if (wormhole_id == INVALID_ENTITY_ID) continue;

            auto& wormhole_transform = ECSM.get_component<Transform2D>(wormhole_id);

            if (is_player_inside_wormhole(player_transform, wormhole_transform)) {
                is_inside = true;
                active_wormhole = wormhole_id;
                break;
            }
        }

        // Always update the E prompt position
        GUI_System* gui_system = nullptr;
        for (auto& system : ECSM.get_systems()) {
            if (auto* gs = dynamic_cast<GUI_System*>(system.get())) {
                gui_system = gs;
                break;
            }
        }

        if (gui_system) {
            // Update the wormhole e prompt position based on player position 
            gui_system->set_wormhole_e_prompt_x(player_transform.position.x);
            gui_system->set_wormhole_e_prompt_y(player_transform.position.y);

            if (is_inside) {
                gui_system->show_wormhole_gui();
            }
            else if (was_inside) {
                gui_system->hide_wormhole_gui();
            }
        }

        // Handle teleportation based on player input
        float current_time = static_cast<float>(glfwGetTime());
        for (const auto& pair : wormhole_pairs) {
            EntityID wormhole_id = pair.first;
            EntityID linked_wormhole = pair.second;
            if (wormhole_id == INVALID_ENTITY_ID || linked_wormhole == INVALID_ENTITY_ID) continue;

            auto& wormhole_transform = ECSM.get_component<Transform2D>(wormhole_id);

            if (is_player_inside_wormhole(player_transform, wormhole_transform)) {
                if ((current_time - last_teleport_time) >= teleport_cooldown &&
                    is_key_just_pressed(GLFW_KEY_E)) {
                    teleport_player(player_id, linked_wormhole); // Teleport the player
                    last_teleport_time = current_time;
                    teleport_flag = true;
                    is_inside = false; // Reset is_inside flag after teleportation
                    break;
                }
            }
        }

        if (is_key_just_pressed(GLFW_KEY_T)) {

            Cheap_Code_Teleport_Wormhole(657.0f, -3750.0f);
        }
        else if (is_key_just_pressed(GLFW_KEY_Y))
        {
            Cheap_Code_Teleport_Wormhole(-741.0f, -7302.0f);
        }
        else if (is_key_just_pressed(GLFW_KEY_U))
        {
            Cheap_Code_Teleport_Wormhole(0.0f, -102.0f);
        }
    }

    bool Player_Script::is_player_inside_wormhole(Transform2D& player, Transform2D& wormhole)
    {
        float wormhole_half_width = wormhole.scale.x / 2.0f;
        float wormhole_half_height = wormhole.scale.y / 2.0f;

        /*std::cout << "Player Position: (" << player.position.x << ", " << player.position.y << ")\n";
        std::cout << "Wormhole Position: (" << wormhole.position.x << ", " << wormhole.position.y << ")\n";*/
        //std::cout << "Wormhole Half Width: " << wormhole_half_width << ", Half Height: " << wormhole_half_height << "\n";

        return (player.position.x >= wormhole.position.x - wormhole_half_width &&
            player.position.x <= wormhole.position.x + wormhole_half_width &&
            player.position.y >= wormhole.position.y - wormhole_half_height &&
            player.position.y <= wormhole.position.y + wormhole_half_height);
    }

    void Player_Script::teleport_player( EntityID Player_ID, EntityID to_wormhole) {
        auto& player_transform = ECSM.get_component<Transform2D>(Player_ID);
        auto& to_wormhole_transform = ECSM.get_component<Transform2D>(to_wormhole);

        // Teleport the player to the new wormhole's position
        player_transform.position = to_wormhole_transform.position;


       // std::cout << "Player teleported to wormhole: " << to_wormhole << "\n";
    }

    void Player_Script::Cheap_Code_Teleport_Wormhole(float pos_x, float pos_y)
    {
        // cheap code for teleport 
        //if (is_key_just_pressed(GLFW_KEY_T))
        //{
            auto& player_transform = ECSM.get_component<Transform2D>(player_id);
            player_transform.position.x = pos_x;
            player_transform.position.y = pos_y;

        //}
       
    }



}