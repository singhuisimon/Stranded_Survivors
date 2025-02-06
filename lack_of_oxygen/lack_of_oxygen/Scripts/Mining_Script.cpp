
#include <iostream>
#include <string>
#include <random>
#include <iomanip>  // for std::setw and std::setfill
#include <sstream>  // for std::stringstream

#include "../Scripts/Mining_Script.h"
#include "../Manager/Log_Manager.h"
#include "../System/Logic_System.h"
#include "../System/Collision_System.h"
#include "../Manager/ECS_Manager.h"
#include "../Manager/Input_Manager.h"
#include "../Manager/Audio_Manager.h"

namespace lof {

	Mining_Script::Mining_Script() {
		player_id = 0;
        mining_strength = DEFAULT_STRENGTH;
	}

	void Mining_Script::register_script() {
		std::shared_ptr<Mining_Script> mining_script = std::make_shared<Mining_Script>();
		static auto maintained_script = mining_script;
		std::weak_ptr<Mining_Script> weak_script = maintained_script;

		mining_script->add_function("init", [weak_script](EntityID entity_id) {
			auto mining_script = weak_script.lock();
			mining_script->set_player_id(ECSM.find_entity_by_name(DEFAULT_PLAYER_NAME));

		});

        mining_script->add_function("mining", [weak_script](EntityID entity_id) {
            auto mining_script = weak_script.lock();
            if (!entity_id || !ECSM.has_component<Audio_Component>(entity_id) || !ECSM.has_component<Animation_Component>(entity_id) ||
                !ECSM.has_component<Physics_Component>(entity_id)) {
                LM.write_log("Mining_Script::register_script(): Entity %d does not have required components.", entity_id);
                return;
            }

            if (entity_id != mining_script->get_player_id()) {
                LM.write_log("Mining_Script::register_script(): Entity %d is not the player.", entity_id);
                return;
            }

            auto& audio_comp = ECSM.get_component<Audio_Component>(entity_id);

            mining_script->check_keys();
            mining_script->update_mining(audio_comp);

        });

        LGS.add_script("mining_script", mining_script);
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

        // Get current frame states
        left_key_pressed = IM.is_key_held(GLFW_KEY_LEFT);
        right_key_pressed = IM.is_key_held(GLFW_KEY_RIGHT);
        up_key_pressed = IM.is_key_held(GLFW_KEY_UP);
        down_key_pressed = IM.is_key_held(GLFW_KEY_DOWN);
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

	void Mining_Script::update_mining(Audio_Component& audio_comp) {

        for (auto& system : ECSM.get_systems()) {
            if (system->get_type() == "Particle_System") {
                auto* particle_system = static_cast<Particle_System*>(system.get());
                if (!particle_system) {
                    LM.write_log("Mining_Script::update_mining(): Fail to get particle system");
                    std::cerr << "Failed to get particle system" << std::endl;
                    return;
                }
                
                if (is_key_just_pressed("left")) {
                    std::cout << "LEFT IS PRESSED" << std::endl;
                    // Emit mining sparks particles
                    update_mining_sparks(particle_system);

                    if (CS.has_left_collide_detect()) {
                        EntityID block_to_remove = CS.get_left_collide_entity();
                        if (block_to_remove != INVALID_ENTITY_ID) {
                            // Update tile 
                            update_tile(block_to_remove, particle_system);
                            //else { // TNT's logic
                            //    // Emit particles, destroy the block and update mineral count when health reaches 0
                            //    if (animation.curr_tile_health == 0 && animation.curr_frame_index != 1) {

                            //        // Store name of TNT to destroy
                            //        std::string name = ECSM.get_entity(block_to_remove)->get_name();
                            //        tnt_to_destroy[name] = 2.0f;
                            //    }
                            //}

                            // Determine sound based on mineral value
                            update_mining_audio(block_to_remove, audio_comp);
                        }
                    }

                    //left_was_press_last_frame = false;
                }
                else if (is_key_just_pressed("right")) {

                    // Emit mining sparks particles
                    update_mining_sparks(particle_system);

                    if (CS.has_right_collide_detect()) {
                        EntityID block_to_remove = CS.get_right_collide_entity();
                        if (block_to_remove != INVALID_ENTITY_ID) {
                            // Update tile 
                            update_tile(block_to_remove, particle_system);
                            //else {
                            //    // Emit particles, destroy the block and update mineral count when health reaches 0
                            //    if (animation.curr_tile_health == 0 && animation.curr_frame_index != 1) {

                            //        // Store name of TNT to destroy
                            //        std::string name = ECSM.get_entity(block_to_remove)->get_name();
                            //        tnt_to_destroy[name] = 2.0f;
                            //    }
                            //}

                            // play mineral value audio
                            update_mining_audio(block_to_remove, audio_comp);
                        }
                    }
                }
                else if (is_key_just_pressed("up")) {

                    // Emit mining sparks particles
                    update_mining_sparks(particle_system);

                    if (CS.has_top_collide_detect()) {
                        EntityID block_to_remove = CS.get_top_collide_entity();
                        if (block_to_remove != INVALID_ENTITY_ID) {
                            // Update tile 
                            update_tile(block_to_remove, particle_system);
                            //else {
                            //    // Emit particles, destroy the block and update mineral count when health reaches 0
                            //    if (animation.curr_tile_health == 0 && animation.curr_frame_index != 1) {

                            //        // Store name of TNT to destroy
                            //        std::string name = ECSM.get_entity(block_to_remove)->get_name();
                            //        tnt_to_destroy[name] = 2.0f;
                            //    }
                            //}

                            // play mineral value audio
                            update_mining_audio(block_to_remove, audio_comp);
                        }
                    }
                }
                else if (is_key_just_pressed("down")) {

                    // Emit mining sparks particles
                    update_mining_sparks(particle_system);

                    if (CS.has_bottom_collide_detect()) {
                        EntityID block_to_remove = CS.get_bottom_collide_entity();
                        if (block_to_remove != INVALID_ENTITY_ID) {
                            // Update tile 
                            update_tile(block_to_remove, particle_system);
                            //else {
                            //    // Emit particles, destroy the block and update mineral count when health reaches 0
                            //    if (animation.curr_tile_health == 0 && animation.curr_frame_index != 1) {

                            //        // Store name of TNT to destroy
                            //        std::string name = ECSM.get_entity(block_to_remove)->get_name();
                            //        tnt_to_destroy[name] = 2.0f;
                            //    }
                            //}

                            // play mineral value audio
                            update_mining_audio(block_to_remove, audio_comp);
                        }
                    }
                }
            }
        }



        

	}

	void Mining_Script::update_tile(int block_to_remove, Particle_System* particle_system) {
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

        // Check if tile is not TNT
        if (animation.animations["0"] != "TNT") {
            // Emit particles, destroy the block and update mineral count when health reaches 0
            if (animation.curr_tile_health != 0) {
                // Randomize particle emit count
                update_mining_particle(particle_system, block_transform, animation);
            }
            else {
                // Get mineral value before destroying the entity
                int mineral_value = get_mineral_value(block_to_remove);

                // Update the mineral count text
                if (mineral_value > 0) {
                    update_mineral_count_text(mineral_value);
                }

                // Emit final particles after destroying tile
                update_final_mining_particle(particle_system, block_transform, animation);

                // Destroy the entity
                ECSM.destroy_entity(block_to_remove);
                LM.write_log("Game_Manager::update: Removed block (Entity %u) with value %d",
                    block_to_remove, mineral_value);
            }
        }
	}

    void Mining_Script::update_mining_audio(int block_to_remove, Audio_Component& audio_comp) {
        // Determine sound based on mineral value
        std::string sound_key = (get_mineral_value(block_to_remove) > 0) ? "mining mineral" : "mining normal";
        ADM.play_now(player_id, sound_key, audio_comp);
    }

	void Mining_Script::update_mining_sparks(Particle_System* particle_system) {
        // Emit mining sparks particles
        for (int i = 0; i < 10; ++i) {
            // Randomize particle emit location in front of player
            auto& player_transform = ECSM.get_component<Transform2D>(player_id);
            float part_x = player_transform.position.x;
            float part_y = player_transform.position.y - (player_transform.scale.y * 0.75f);
            particle_system->particle_emit("mining", Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f));
        }
	}

    void Mining_Script::update_mining_particle(Particle_System* particle_system, Transform2D& transform_comp, Animation_Component& animation_comp) {
        int rand_part_cnt = 2 + static_cast<int>(std::floorf(particle_system->get_rand_float() * 3.0f));
        for (int i = 0; i < rand_part_cnt; ++i) {
            // Randomize particle emit location within the tile
            float part_x = transform_comp.position.x - (transform_comp.scale.x / 2.0f) + (particle_system->get_rand_float() * transform_comp.scale.x);
            float part_y = transform_comp.position.y - (transform_comp.scale.y / 2.0f) + (particle_system->get_rand_float() * transform_comp.scale.y);
            particle_system->particle_emit(animation_comp.animations["0"], Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f));
        }
    }

    void Mining_Script::update_final_mining_particle(Particle_System* particle_system, Transform2D& transform_comp, Animation_Component& animation_comp) {
        // Emit final particles after destroying tile
        for (int i = 0; i < 6; ++i) {
            // Randomize particle emit location within the tile
            float part_x = transform_comp.position.x - (transform_comp.scale.x / 2.0f) + (particle_system->get_rand_float() * transform_comp.scale.x);
            float part_y = transform_comp.position.y - (transform_comp.scale.y / 2.0f) + (particle_system->get_rand_float() * transform_comp.scale.y);
            particle_system->particle_emit(animation_comp.animations["0"], Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f));
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
}