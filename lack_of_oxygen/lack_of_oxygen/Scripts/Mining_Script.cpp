/**
 * @file Mining_Script.cpp
 * @brief Declare the mining_script class
 * @author Amanda Leow Boon Suan (40%), Chua Wen Bin Kenny (30%), Wai Lwin Thit (30%)
 * @date February 24, 2025
 * Copyright (C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */
#include <iostream>
#include <string>
#include <random>
#include <iomanip>  // for std::setw and std::setfill
#include <sstream>  // for std::stringstream

#include "../Scripts/Mining_Script.h"
#include "../Manager/Log_Manager.h"
#include "../System/Logic_System.h"
#include "../System/GUI_System.h"
#include "../System/Collision_System.h"
#include "../Manager/ECS_Manager.h"
#include "../Manager/Input_Manager.h"
#include "../Manager/Game_Manager.h"
#include "../Manager/Audio_Manager.h"
#include "../Manager/IMGUI_Manager.h"
#include "../Manager/Graphics_Manager.h"

namespace lof {

    Mining_Script::Mining_Script() {
        player_id = 0;
        mining_strength = DEFAULT_STRENGTH;
        mining_cooldown = MINING_COOLDOWN_TIMER;

    }

    std::string Mining_Script::get_type() const {
        return script_name;
    }

    void Mining_Script::register_script() {
		auto mining_script = shared_from_this();

        mining_script->add_function("init", [weak_script = std::weak_ptr<Mining_Script>(mining_script)](EntityID entity_id) {
            (void)entity_id;
            auto mining_script = weak_script.lock();
            mining_script->set_player_id(ECSM.find_entity_by_name(DEFAULT_PLAYER_NAME));
        });

        mining_script->add_function("update", [weak_script = std::weak_ptr<Mining_Script>(mining_script)](EntityID entity_id) {
            auto mining_script = weak_script.lock();
            if (!entity_id || !ECSM.has_component<Audio_Component>(entity_id) || !ECSM.has_component<Animation_Component>(entity_id)) {
                LM.write_log("Mining_Script::register_script(): Entity %d does not have required components.", entity_id);
                return;
            }

            if (entity_id != mining_script->get_player_id()) {
                LM.write_log("Mining_Script::register_script(): Entity %d is not the player.", entity_id);
                return;
            }

            auto& audio_comp = ECSM.get_component<Audio_Component>(entity_id);

            mining_script->check_keys();
            mining_script->increase_mining_strength_cheat();

            for (auto& system : ECSM.get_systems()) {
                if (system->get_type() == "Particle_System") {
                    auto* particle_system = static_cast<Particle_System*>(system.get());
                    if (!particle_system) {
                        LM.write_log("Mining_Script::update_mining(): Fail to get particle system");
                        std::cerr << "Failed to get particle system" << std::endl;
                        return;
                    }
                    
                    mining_script->update_surrounding_blocks(particle_system);
                    mining_script->update_mining(audio_comp, particle_system);
                }
			}

            mining_script->increase_mineral_count_cheat();
           
            //update mineral popups
           // try {


                   // COMMENT 
                    mining_script->update_mineral_popups(FPSM.get_delta_time());

           // }
           // catch (const std::exception& e) {

           //     LM.write_log("Error updating mineral popup: %s", e.what());
           //     mining_script->popup_active = false;
           // }

        });

        //LGS.add_script("mining_script", mining_script);
    }

    void Mining_Script::set_player_id(EntityID entityid) {
        player_id = entityid;
    }

    EntityID Mining_Script::get_player_id() const {
        return player_id;
    }

    void Mining_Script::check_keys() {
        // Update previous frame states
        left_key_last_frame = left_key_pressed;
        right_key_last_frame = right_key_pressed;
        up_key_last_frame = up_key_pressed;
        down_key_last_frame = down_key_pressed;
        key_g_last_frame = key_g_pressed;
        key_h_last_frame = key_h_pressed;

        // Get current frame states
        left_key_pressed = IM.is_key_held(GLFW_KEY_LEFT);
        right_key_pressed = IM.is_key_held(GLFW_KEY_RIGHT);
        up_key_pressed = IM.is_key_held(GLFW_KEY_UP);
        down_key_pressed = IM.is_key_held(GLFW_KEY_DOWN);
        key_g_pressed = IM.is_key_held(GLFW_KEY_G);
        key_h_pressed = IM.is_key_held(GLFW_KEY_H);

    }

    bool Mining_Script::is_key_just_pressed(const std::string& direction) const {
        if (direction == "left") {
            return left_key_pressed && !left_key_last_frame;
        }
        else if (direction == "right") {
            return right_key_pressed && !right_key_last_frame;
        }
        else if (direction == "up") {
            return up_key_pressed && !up_key_last_frame;
        }
        else if (direction == "down") {
            return down_key_pressed && !down_key_last_frame;
        }
        return false;
    }

    void Mining_Script::increase_mining_strength_cheat() {
        if (key_h_pressed && !key_h_last_frame) {
			if (mining_strength == DEFAULT_STRENGTH) {
				mining_strength = GOD_STRENGTH;
			}
			else {
				mining_strength = DEFAULT_STRENGTH;
			}
        }
    }

    void Mining_Script::update_surrounding_blocks(Particle_System* particle_system) {

        // Loop through the TNT to destroy and update it 
        for (auto start = tnt_to_destroy.begin(), end = tnt_to_destroy.end(); start != end;) {

            // Get current tnt ready and prep iterator for the next
            auto current = start++;

            // Retrieve TNT id
            EntityID tnt_id = ECSM.find_entity_by_name(current->first);
            if (tnt_id != INVALID_ENTITY_ID) {

                // Emit particles every 0.5s within 2s of fuse time
                auto& tnt_transform = ECSM.get_component<Transform2D>(tnt_id);
                int time_fract = static_cast<int>(10.0f * (current->second - std::floorf(current->second)));
                if (time_fract % 6 == 2) {

                    // play the sound
                    if (ECSM.has_component<Audio_Component>(tnt_id)) {
                        auto& audio = ECSM.get_component<Audio_Component>(tnt_id);
                        ADM.play_now(tnt_id, "TNT_Explosion_Sound", audio);
                    }

                    // Emit fuse sparks particles
                    float part_x = tnt_transform.position.x - (tnt_transform.scale.x / 2.0f) + (particle_system->get_rand_float() * tnt_transform.scale.x);
                    float part_y = tnt_transform.position.y - (tnt_transform.scale.y / 2.0f) + (particle_system->get_rand_float() * tnt_transform.scale.y);
                    particle_system->particle_emit("TNT", Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f));
                }
                current->second -= FPSM.get_delta_time(); // Decrement particle fuse time

                // Emit circular visual effect for tnt fuse
                float angle_fuse = (360.0f * current->second / 2.0f) * (PI_VALUE / 180.0f);
                for (int i = 1; i <= 5; ++i) {

                    // Randomizer value
                    int randomizer = 4 + static_cast<int>(particle_system->get_rand_float() * 5.0f);

                    // lifetime of particle
                    float lifetime = current->second - 0.01f;
                    if (lifetime <= 0.0f) {
                        lifetime = 0.001f;
                    }

                    float part_x = tnt_transform.position.x + i * (cos(angle_fuse) * tnt_transform.scale.x / randomizer);
                    float part_y = tnt_transform.position.y + i * (sin(angle_fuse) * tnt_transform.scale.y / randomizer);
                    particle_system->particle_emit("TNT_VFX", Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f), lifetime);
                }

                // Destroy itself and emit final particles when fuse time ends 
                if (current->second <= 0.0f) {

                    if (ECSM.has_component<Audio_Component>(tnt_id)) {
                        auto& audio = ECSM.get_component<Audio_Component>(tnt_id);
                        ADM.play_now(tnt_id, "TNT_Boom", audio);
                    }

                    // Emit explosion particles in a circular pattern
                    for (int i = 0; i < 10; ++i) {
                        float angle = i * 36.0f * (PI_VALUE / 180.0f);
                        float part_x = tnt_transform.position.x + (cos(angle) * tnt_transform.scale.x / 10.0f);
                        float part_y = tnt_transform.position.y + (sin(angle) * tnt_transform.scale.y / 10.0f);

                        for (int j = 1; j <= 5; ++j) {
                            part_x = tnt_transform.position.x + j * (cos(angle) * tnt_transform.scale.x / 10.0f);
                            part_y = tnt_transform.position.y + j * (sin(angle) * tnt_transform.scale.y / 10.0f);
                            particle_system->particle_emit("TNT_Explode", Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f));
                        }
                    }

                    // Decide how many entities to check 
                    int preceding_check_cnt{ 21 }, following_check_cnt{ 21 };
                    preceding_check_cnt = tnt_id > 21 ? 21 : (tnt_id - 1);
                    following_check_cnt = (static_cast<int>(ECSM.get_entities().size()) - tnt_id - 1) > 21 ?
                        21 : (static_cast<int>(ECSM.get_entities().size()) - tnt_id - 1);

                    if (tnt_transform.position.x == -912.0f && preceding_check_cnt > 0) {
                        preceding_check_cnt--;
                    }
                    else if (tnt_transform.position.x == 912.0f && following_check_cnt > 0) {
                        following_check_cnt--;
                    }

                    // Calculate boundary around tnt to destroy tiles within  
                    float boundary_offset = tnt_transform.scale.x * 2.0f - 1.0f;
                    float boundary_left = tnt_transform.position.x - boundary_offset;
                    float boundary_right = tnt_transform.position.x + boundary_offset;
                    float boundary_top = tnt_transform.position.y + boundary_offset;
                    float boundary_bottom = tnt_transform.position.y - boundary_offset;

                    // Destroy tiles within boundary
                    if (following_check_cnt > 0) {
                        for (int i = 0; i < following_check_cnt; ++i) {

                            // Set entity id
                            EntityID entity_id = tnt_id + following_check_cnt - i;

                            // Leave player to be checked last
                            if (entity_id == player_id) {
                                continue;
                            }

                            auto& entity_transform = ECSM.get_component<Transform2D>(entity_id);
                            if (!ECSM.has_component<Animation_Component>(entity_id)) {
                                continue;
                            }
                            auto& entity_animation = ECSM.get_component<Animation_Component>(entity_id);

                            // Skip for these entities
                            if (entity_animation.animations["0"] == "vent_strip_up" || entity_animation.animations["0"] == "vent_strip_right" ||
                                entity_animation.animations["0"] == "vent_strip_left" || entity_animation.animations["0"] == "vent" ||
                                entity_animation.animations["0"] == "wormhole" || entity_animation.animations["0"] == "lava" ||
                                entity_animation.animations["0"] == "lava_animate") {
                                continue;
                            }

                            // Check if entity is inside boundary
                            if ((boundary_left <= entity_transform.position.x && entity_transform.position.x <= boundary_right) &&
                                (boundary_bottom <= entity_transform.position.y && entity_transform.position.y <= boundary_top)) {

                                // Check if it's TNT or minerals
                                if (entity_animation.animations["0"] != "TNT") {
                                    // Emit final particles before destroying tile
                                    for (int j = 0; j < 6; ++j) {
                                        // Randomize particle emit location within the tile
                                        float part_x = entity_transform.position.x - (entity_transform.scale.x / 2.0f) + (particle_system->get_rand_float() * entity_transform.scale.x);
                                        float part_y = entity_transform.position.y - (entity_transform.scale.y / 2.0f) + (particle_system->get_rand_float() * entity_transform.scale.y);
                                        particle_system->particle_emit(entity_animation.animations["0"], Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f));
                                    }

                                    // Destroy the entity
                                    ECSM.destroy_entity(entity_id);
                                    LM.write_log("Game_Manager::update: Removed block (Entity %u)", entity_id);
                                }
                                else {
                                    // Check if tnt is activated to prevent activation again
                                    if (entity_animation.curr_tile_health != 0) {
                                        // Set tnt health to 0
                                        entity_animation.curr_tile_health = 0;

                                        // Store name of TNT to destroy
                                        std::string name = ECSM.get_entity(entity_id)->get_name();
                                        tnt_to_destroy[name] = 2.0f;
                                    }
                                }
                            }
                        }
                    }

                    if (preceding_check_cnt > 0) {

                        int offset = 0;
                        for (int i = 1; i <= preceding_check_cnt; ++i) {

                            // Set entity id
                            EntityID entity_id = tnt_id - i + offset;

                            // Leave player to be checked last
                            if (entity_id == player_id) {
                                continue;
                            }

                            auto& entity_transform = ECSM.get_component<Transform2D>(entity_id);
                            if (!ECSM.has_component<Animation_Component>(entity_id)) {
                                continue;
                            }
                            auto& entity_animation = ECSM.get_component<Animation_Component>(entity_id);

                            // Skip for these entities
                            if (entity_animation.animations["0"] == "vent_strip_up" || entity_animation.animations["0"] == "vent_strip_right" ||
                                entity_animation.animations["0"] == "vent_strip_left" || entity_animation.animations["0"] == "vent" ||
                                entity_animation.animations["0"] == "wormhole" || entity_animation.animations["0"] == "lava" ||
                                entity_animation.animations["0"] == "lava_animate") {
                                continue;
                            }

                            // Check if entity is inside boundary
                            if ((boundary_left <= entity_transform.position.x && entity_transform.position.x <= boundary_right) &&
                                (boundary_bottom <= entity_transform.position.y && entity_transform.position.y <= boundary_top)) {

                                // Check if it's TNT or minerals
                                if (entity_animation.animations["0"] != "TNT") {

                                    // Increment offset and decrement tnt_id
                                    offset++;
                                    tnt_id--;

                                    // Emit final particles before destroying tile
                                    for (int j = 0; j < 6; ++j) {
                                        // Randomize particle emit location within the tile
                                        float part_x = entity_transform.position.x - (entity_transform.scale.x / 2.0f) + (particle_system->get_rand_float() * entity_transform.scale.x);
                                        float part_y = entity_transform.position.y - (entity_transform.scale.y / 2.0f) + (particle_system->get_rand_float() * entity_transform.scale.y);
                                        particle_system->particle_emit(entity_animation.animations["0"], Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f));
                                    }

                                    // Destroy the entity
                                    ECSM.destroy_entity(entity_id);
                                    LM.write_log("Game_Manager::update: Removed block (Entity %u)", entity_id);
                                }
                                else {
                                    // Check if tnt is activated to prevent activation again
                                    if (entity_animation.curr_tile_health != 0) {
                                        // Set tnt health to 0
                                        entity_animation.curr_tile_health = 0;

                                        // Store name of TNT to destroy
                                        std::string name = ECSM.get_entity(entity_id)->get_name();
                                        tnt_to_destroy[name] = 2.0f;
                                    }
                                }
                            }
                        }
                    }

                    // Check if player is within boundary
                    bool is_player_dead = false;
                    auto& player_transform = ECSM.get_component<Transform2D>(player_id);
                    if ((boundary_left <= player_transform.position.x && player_transform.position.x <= boundary_right) &&
                        (boundary_bottom <= player_transform.position.y && player_transform.position.y <= boundary_top)) {
                        //std::cout << "Player triggle tnt!\n";
                        // Player is caught in the TNT blast - show game over screen
                        is_player_dead = true;
                        GM.set_player_dead_state(true);  // Use the setter for better encapsulation
                    }

                    // Check if player is dead to reset the scene
                    if (is_player_dead == true) {

                        ECSM.destroy_entity(tnt_id);
                        tnt_to_destroy.erase(current->first);
                        tnt_to_destroy.clear();

                        // Stop all audio first
                        //ADM.stop_mastergroup();

                        ADM.stop_groups(GroupType::TYPE_BGM);
                        ADM.stop_groups(GroupType::TYPE_SFX);

                        //reset panic
                        GM.reset_panic();

                        // Find GUI System and show game over screen
                        for (auto& systems_gui : ECSM.get_systems()) {
                            if (auto* gui_system = dynamic_cast<GUI_System*>(systems_gui.get())) {
                                // First reset all GUI states
                                gui_system->reset_all_game_state();

                                // Then show the game over screen
                                gui_system->show_game_over_menu();

                                LM.write_log("Game over screen displayed - player killed by TNT");
                                break;
                            }
                        }

                        // No need to immediately reload the scene or set current file
                        // The user will choose restart or main menu from the game over screen
                        break;
                    }
                    else {
                        // Destroy tnt and remove it from the list of tnt to destroy
                        ECSM.destroy_entity(tnt_id);
                        tnt_to_destroy.erase(current->first);
                        LM.write_log("Game_Manager::update: Removed block (Entity %u)", tnt_id);
                    }
                }
            }
        }
    }



    void Mining_Script::update_mining(Audio_Component& audio_comp, Particle_System* particle_system) {

        //std::cout << "hi" << std::endl;

        if(mining_cooldown > 0.0f){
            mining_cooldown -= FPSM.get_delta_time();
        }

        update_mining_direction();

        bool allow_mining = (current_mining_direction != "none") && (mining_cooldown <= 0.0f || is_key_just_pressed(current_mining_direction));

        if (allow_mining) {

            mining_cooldown = MINING_COOLDOWN_TIMER;

            if (current_mining_direction == "left") {

                // Emit mining sparks particles
                for (int i = 0; i < 10; ++i) {
                    // Randomize particle emit location in front of player
                    auto& player_transform = ECSM.get_component<Transform2D>(player_id);
                    float part_x = player_transform.position.x - (player_transform.scale.x / 2.0f);
                    float part_y = player_transform.position.y;
                    particle_system->particle_emit("mining", Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f));
                }

                if (CS.has_left_collide_detect()) {
                    EntityID block_to_remove = CS.get_left_collide_entity();
                    if (block_to_remove != INVALID_ENTITY_ID) {
                        update_tile(block_to_remove, particle_system);
                    }
                }
                else {
                    ADM.play_now(player_id, "mining air", audio_comp);
                }
            }
            else if (current_mining_direction == "right") {

                // Emit mining sparks particles
                for (int i = 0; i < 10; ++i) {
                    // Randomize particle emit location in front of player
                    auto& player_transform = ECSM.get_component<Transform2D>(player_id);
                    float part_x = player_transform.position.x + (player_transform.scale.x / 2.0f);
                    float part_y = player_transform.position.y;
                    particle_system->particle_emit("mining", Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f));
                }

                if (CS.has_right_collide_detect()) {
                    EntityID block_to_remove = CS.get_right_collide_entity();
                    if (block_to_remove != INVALID_ENTITY_ID) {
                        update_tile(block_to_remove, particle_system);
                    }
                }
                else {
                    ADM.play_now(player_id, "mining air", audio_comp);
                }
            }
            else if (current_mining_direction == "up") {

                // Emit mining sparks particles
                for (int i = 0; i < 10; ++i) {
                    // Randomize particle emit location in front of player
                    auto& player_transform = ECSM.get_component<Transform2D>(player_id);
                    float part_x = player_transform.position.x;
                    float part_y = player_transform.position.y + (player_transform.scale.y * 0.75f);
                    particle_system->particle_emit("mining", Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f));
                }

                if (CS.has_top_collide_detect()) {
                    EntityID block_to_remove = CS.get_top_collide_entity();
                    if (block_to_remove != INVALID_ENTITY_ID) {
                        update_tile(block_to_remove, particle_system);
                    }
                }
                else {
                    ADM.play_now(player_id, "mining air", audio_comp);
                }
            }
            else if (current_mining_direction == "down") {

                // Emit mining sparks particles
                for (int i = 0; i < 10; ++i) {
                    // Randomize particle emit location in front of player
                    auto& player_transform = ECSM.get_component<Transform2D>(player_id);
                    float part_x = player_transform.position.x;
                    float part_y = player_transform.position.y - (player_transform.scale.y * 0.75f);
                    particle_system->particle_emit("mining", Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f));
                }

                if (CS.has_bottom_collide_detect()) {
                    EntityID block_to_remove = CS.get_bottom_collide_entity();
                    if (block_to_remove != INVALID_ENTITY_ID) {
                        update_tile(block_to_remove, particle_system);
                    }
                }
                else {
                    ADM.play_now(player_id, "mining air", audio_comp);
                }
            }
        }
    }

    void Mining_Script::increase_mineral_count_cheat() {
        if (key_g_pressed) {
            int val_to_add = 500;
            update_mineral_count_text(val_to_add);
        }
    }

    void Mining_Script::update_mining_direction() {

        new_direction = "none";

        //check for new key press
        if (current_mining_direction == "none") {
            if (is_key_just_pressed("left")) {
                new_direction = "left";
            }
            else if (is_key_just_pressed("right")) {
                new_direction = "right";
            }
            else if (is_key_just_pressed("up")) {
                new_direction = "up";
            }
            else if (is_key_just_pressed("down")) {
                new_direction = "down";
            }
        }
        //if no new key press continue iwth current held direction
        else if (left_key_pressed && !right_key_pressed &&
                !up_key_pressed && !down_key_pressed) {
            new_direction = "left";
        }
        else if (right_key_pressed && !left_key_pressed &&
                !up_key_pressed && !down_key_pressed) {
            new_direction = "right";
        }
        else if (up_key_pressed && !left_key_pressed &&
                !right_key_pressed && !down_key_pressed) {
            new_direction = "up";
        }
        else if (down_key_pressed && !left_key_pressed &&
                !right_key_pressed && !up_key_pressed) {
            new_direction = "down";
        }
        else if(down_key_pressed || left_key_pressed ||
                right_key_pressed || up_key_pressed) {
            new_direction = current_mining_direction;
        }

        current_mining_direction = new_direction;
    }

    void Mining_Script::update_tile(EntityID block_to_remove, Particle_System* particle_system) {
        auto& animation = ECSM.get_component<Animation_Component>(block_to_remove);
        if (animation.curr_tile_health > 0) {
            if (animation.curr_tile_health <= mining_strength) {
                animation.curr_tile_health -= animation.curr_tile_health;
            }
            else {
                animation.curr_tile_health -= mining_strength;
            }
        }

        // Get block's position and size
        auto& block_transform = ECSM.get_component<Transform2D>(block_to_remove);

        // Get the audio comp from player for block audio
        auto& block_audio = ECSM.get_component<Audio_Component>(player_id);

        // Check if tile is not TNT
        if (animation.animations["0"] != "TNT") {
            // Emit particles, destroy the block and update mineral count when health reaches 0
            if (animation.curr_tile_health != 0) {

                // Randomize particle emit count
                update_mining_particle(particle_system, block_transform, animation);
				// Play the mining audio
                update_mining_audio(block_to_remove, block_audio, "mining mineral" , "mining normal");
            }
            else { 

                // Get mineral value before destroying the entity

                int mineral_value = get_mineral_value(block_to_remove);

                // Update the mineral count text and create the mineral pop_up
                if (mineral_value > 0) {


                    show_mineral_popup(mineral_value, block_transform.position);

                    update_mineral_count_text(mineral_value);
                }



                // Emit final particles after destroying tile
                update_final_mining_particle(particle_system, block_transform, animation);

                // Play the mining audio
                update_mining_audio(block_to_remove, block_audio, "mineral destroy" , "mining normal");

                // Destroy the entity
                ECSM.destroy_entity(block_to_remove);
                LM.write_log("Game_Manager::update: Removed block (Entity %u) with value %d",
                    block_to_remove, mineral_value);
            }
        }
        else {
            // Emit particles, destroy the block and update mineral count when health reaches 0
            if (animation.curr_tile_health == 0 && animation.curr_frame_index != 1) {

                // Store name of TNT to destroy
                std::string name = ECSM.get_entity(block_to_remove)->get_name();
                tnt_to_destroy[name] = 2.0f;
            }
        }
    }

    void Mining_Script::update_mining_audio(EntityID block_to_remove, Audio_Component& audio_comp, const std::string key_1, const std::string key_2) {
        // Determine sound based on mineral value
        std::string sound_key = (get_mineral_value(block_to_remove) > 0) ? key_1 : key_2;
        ADM.play_now(player_id, sound_key, audio_comp);
    }

    void Mining_Script::update_mining_particle(Particle_System* particle_system, Transform2D& transform_comp, Animation_Component& animation_comp) {
        // Randomize particle emit count
        int rand_part_cnt = 2 + static_cast<int>(std::floorf(particle_system->get_rand_float() * 3.0f));
        for (int i = 0; i < rand_part_cnt; ++i) {
            // Randomize particle emit location within the tile
            float part_x = transform_comp.position.x - (transform_comp.scale.x / 2.0f) + (particle_system->get_rand_float() * transform_comp.scale.x);
            float part_y = transform_comp.position.y - (transform_comp.scale.y / 2.0f) + (particle_system->get_rand_float() * transform_comp.scale.y);
            
            std::string particle_name = animation_comp.animations["0"];
            if (animation_comp.animations["0"].find("rock") != std::string::npos) {
                particle_name = "rock";
            }
            else if (animation_comp.animations["0"].find("dirt") != std::string::npos) {
                particle_name = "dirt";
            }
            particle_system->particle_emit(particle_name, Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f));
        }
    }

    void Mining_Script::update_final_mining_particle(Particle_System* particle_system, Transform2D& transform_comp, Animation_Component& animation_comp) {
        // Emit final particles after destroying tile
        for (int i = 0; i < 6; ++i) {
            // Randomize particle emit location within the tile
            float part_x = transform_comp.position.x - (transform_comp.scale.x / 2.0f) + (particle_system->get_rand_float() * transform_comp.scale.x);
            float part_y = transform_comp.position.y - (transform_comp.scale.y / 2.0f) + (particle_system->get_rand_float() * transform_comp.scale.y);
            
            std::string particle_name = animation_comp.animations["0"];
            if (animation_comp.animations["0"].find("rock") != std::string::npos) {
                particle_name = "rock";
            }
            else if (animation_comp.animations["0"].find("dirt") != std::string::npos) {
                particle_name = "dirt";
            }
            particle_system->particle_emit(particle_name, Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f));
        }
    }

    int Mining_Script::get_mineral_value(EntityID block_id) const {
        if (!ECSM.has_component<Animation_Component>(block_id)) {
            return 0;
        }

        auto* entity = ECSM.get_entity(block_id);
        if (!entity) {
            return 0;
        }

        const std::string& name = entity->get_name();
        LM.write_log("Checking mineral value for entity with name: %s", name.c_str());

        // Match the prefab names with their corresponding values
        if (name.find("quartz") != std::string::npos) {
            LM.write_log("Found quartz mineral, value: 100");
            return 100;
        }
        if (name.find("emerald") != std::string::npos) {
            LM.write_log("Found emerald mineral, value: 800");
            return 800;
        }
        if (name.find("sapphire") != std::string::npos) {
            LM.write_log("Found sapphire mineral, value: 1600");
            return 1600;
        }
        if (name.find("amethyst") != std::string::npos) {
            LM.write_log("Found amethyst mineral, value: 2400");
            return 2400;
        }
        if (name.find("citrine") != std::string::npos) {
            LM.write_log("Found citrine mineral, value: 3200");
            return 3200;
        }
        if (name.find("alexandrite") != std::string::npos) {
            LM.write_log("Found alexandrite mineral, value: 4000");
            return 4000;
        }

        LM.write_log("No mineral value found for this entity");
        return 0;
    }

    void Mining_Script::update_mineral_count_text(int value_to_add) {
        EntityID text_entity = ECSM.find_entity_by_name("top_ui_mineral_count_text");
        if (text_entity == INVALID_ENTITY_ID) {
            LM.write_log("Could not find mineral count text entity");
            return;
        }

        if (!ECSM.has_component<Text_Component>(text_entity)) {
            LM.write_log("Mineral count entity does not have Text_Component");
            return;
        }

        try {
            auto& text_comp = ECSM.get_component<Text_Component>(text_entity);
            // Convert current text to integer, add new value
            int current_value = std::stoi(text_comp.text);
            current_value += value_to_add;

            // Format the number with leading zeros (6 digits)
            std::stringstream ss;
            ss << std::setw(6) << std::setfill('0') << current_value;
            text_comp.text = ss.str();

            LM.write_log("Updated mineral count to: %06d", current_value);
        }
        catch (const std::exception& e) {
            LM.write_log("Error updating mineral count: %s", e.what());
        }
    }

    void Mining_Script::show_mineral_popup(int mineral_value, const Vec2D& position) {

        if (mineral_value <= 0) return; 

            //get the popup id again
            popup_entity_id = ECSM.find_entity_by_name("mineral_popup");

            if (popup_entity_id == INVALID_ENTITY_ID) {
                LM.write_log("Warning: Could not find 'mineral_pop' entity");
                return;
            
            }
            //std::cout << "Popup_EntityID: " << popup_entity_id << std::endl;

        //update position
        auto& transform = ECSM.get_component<Transform2D>(popup_entity_id);
        transform.position = position;

        
        //EntityID player = ECSM.find_entity_by_name(DEFAULT_PLAYER_NAME);
        //auto& player_transform = ECSM.get_component<Transform2D>(player);

        ////this is so temporarily the text can be seen (not accurate)
        //transform.position.y = player_transform.position.y - transform.position.y; 

        //update text 
        auto& text_comp = ECSM.get_component<Text_Component>(popup_entity_id);
        std::stringstream ss; 
        ss << "+" << mineral_value;
        text_comp.text = ss.str();

        //activate popup
        popup_active = true; 
        popup_timer = POPUP_LIFETIME;

       // LM.write_log("Showing mineral popup: +%d at position (%.2f, %.2f)", mineral_value, position.x, position.y); 

     
    }

    void Mining_Script::update_mineral_popups(float delta_time) {

        if (!popup_active || popup_entity_id == INVALID_ENTITY_ID) {
            return;
        }
        //get the entity id again 
        popup_entity_id = ECSM.find_entity_by_name("mineral_popup");

        // Ensure entity still exists
        if (!ECSM.get_entity(popup_entity_id)) {
            LM.write_log("Warning: Popup entity no longer exists");
            popup_active = false;
           return;
        }

        // Double-check components exist to prevent runtime errors
        if (!ECSM.has_component<Transform2D>(popup_entity_id) ||
            !ECSM.has_component<Text_Component>(popup_entity_id)) {
            LM.write_log("Warning: Popup entity missing required components");
            popup_active = false;
            return;
        }


        // Decrease timer
        popup_timer -= delta_time;

        // Hide popup when timer expires
        if (popup_timer <= 0.0f) {
            popup_active = false;

            try {

                auto& text_comp = ECSM.get_component<Text_Component>(popup_entity_id);
                text_comp.text = ""; // Clear text

                LM.write_log("Hidden mineral popup");
            }
            catch (const std::exception& e) {
                LM.write_log("Error hiding popup: %s", e.what());
            }
        }
    }

} // namespace lof