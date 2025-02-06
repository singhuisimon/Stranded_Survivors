

#include <memory>
#include "../Scripts/TNT_Script.h"
#include "../Manager/Log_Manager.h"
#include "../System/Particle_System.h"
#include "../System/Logic_System.h"
#include "../System/Collision_System.h"
#include "../Manager/ECS_Manager.h"
#include "../Manager/Input_Manager.h"
#include "../Manager/Audio_Manager.h"

namespace lof {

	TNT_Script::TNT_Script() {
		tnt_to_destroy = std::unordered_map<std::string, float>();
	}

	TNT_Script::~TNT_Script() {
		clean_up();
		LM.write_log("TNT_Script::clean up complete");
	}

	void TNT_Script::register_script() {
		std::shared_ptr<TNT_Script> tnt_script = std::make_shared<TNT_Script>();
		static auto maintained_script = tnt_script;
		std::weak_ptr<TNT_Script> weak_script = tnt_script;

		tnt_script->add_function("init", [weak_script](EntityID entity_id) {
			auto tnt_script = weak_script.lock();
			
		});

		tnt_script->add_function("kaboom", [weak_script](EntityID entity_id) {
			auto tnt_script = weak_script.lock();
			if (!entity_id || !ECSM.has_component<Audio_Component>(entity_id) || !ECSM.has_component<Animation_Component>(entity_id) ||
				!ECSM.has_component<Physics_Component>(entity_id)) {
				LM.write_log("TNT_Script::register_script(): Entity %d does not have required components.", entity_id);
				return;
			}
			auto& audio_comp = ECSM.get_component<Audio_Component>(entity_id);
			auto& physic_comp = ECSM.get_component<Physics_Component>(entity_id);

			for (auto& system : ECSM.get_systems()) {
				if (system->get_type() == "Particle_System") {
					auto* particle_system = static_cast<Particle_System*>(system.get());
					if (!particle_system) {
						LM.write_log("Game_Manager::update(): Fail to get particle system");
						std::cerr << "Failed to get particle system" << std::endl;
						return;
					}

					tnt_script->check_tnt_explosion(particle_system);
					/*tnt_script->update_tnt_animation();
					tnt_script->update_tnt_audio(physic_comp, audio_comp);
					tnt_script->update_tnt_particle();*/
				}
			}
			
		});

		LGS.add_script("tnt_script", tnt_script);
	}

	void TNT_Script::clean_up() {
		tnt_to_destroy.clear();
	}

	void TNT_Script::check_tnt_explosion(Particle_System* particle_system) {
        for (auto start = tnt_to_destroy.begin(), end = tnt_to_destroy.end(); start != end;) {

            // Get current tnt ready and prep iterator for the next
            auto current = start++;

            // Retrieve TNT id
            EntityID tnt_id = ECSM.find_entity_by_name(current->first);
            if (tnt_id != INVALID_ENTITY_ID) {

                // Emit particles every 0.5s within 2s of fuse time
                auto& tnt_transform = ECSM.get_component<Transform2D>(tnt_id);
                auto& tnt_graphics = ECSM.get_component<Graphics_Component>(tnt_id);
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

                    //// Change TNT alpha
                    //tnt_graphics.color.a = 0.5f;
                    //tnt_transform.scale.x = 96.0f * 1.1f;
                    //tnt_transform.scale.y = 96.0f * 1.1f;
                }
                else {
                    //// Change TNT alpha
                    //tnt_graphics.color.a = 1.0f;
                    //tnt_transform.scale.x = 96.0f;
                    //tnt_transform.scale.y = 96.0f;
                }

                float delta_time = FPSM.get_delta_time();
                current->second -= delta_time; // Decrement particle fuse time

                // Emit circular visual effect for tnt fuse
                float angle = (360.0f * current->second / 2.0f) * (PI_VALUE / 180.0f);
                for (int i = 1; i <= 5; ++i) {

                    // Randomizer value
                    int randomizer = 4 + static_cast<int>(particle_system->get_rand_float() * 5.0f);

                    // lifetime of particle
                    float lifetime = current->second - 0.01f;
                    if (lifetime <= 0.0f) {
                        lifetime = 0.001f;
                    }

                    float part_x = tnt_transform.position.x + i * (cos(angle) * tnt_transform.scale.x / randomizer);
                    float part_y = tnt_transform.position.y + i * (sin(angle) * tnt_transform.scale.y / randomizer);
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
                    following_check_cnt = (ECSM.get_entities().size() - tnt_id - 1) > 21 ? 21 : (ECSM.get_entities().size() - tnt_id - 1);

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
                            auto& entity_transform = ECSM.get_component<Transform2D>(entity_id);
                            if (!ECSM.has_component<Animation_Component>(entity_id)) {
                                continue;
                            }
                            auto& entity_animation = ECSM.get_component<Animation_Component>(entity_id);

                            // Skip for these entities
                            if (entity_animation.animations["0"] == "vent_strip" || entity_animation.animations["0"] == "vent" ||
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
                                    for (int i = 0; i < 6; ++i) {
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
                            auto& entity_transform = ECSM.get_component<Transform2D>(entity_id);
                            if (!ECSM.has_component<Animation_Component>(entity_id)) {
                                continue;
                            }
                            auto& entity_animation = ECSM.get_component<Animation_Component>(entity_id);

                            // Skip for these entities
                            if (entity_animation.animations["0"] == "vent_strip" || entity_animation.animations["0"] == "vent" ||
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
                                    for (int i = 0; i < 6; ++i) {
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
                    //bool is_player_dead = false;
                    //auto& player_transform = ECSM.get_component<Transform2D>(player_id);
                    //if ((boundary_left <= player_transform.position.x && player_transform.position.x <= boundary_right) &&
                    //    (boundary_bottom <= player_transform.position.y && player_transform.position.y <= boundary_top)) {

                    //    // Reset player to starting point if within TNT blast boundary
                    //    is_player_dead = true;
                    //    std::string scene_file{ "scene2.scn" };
                    //    current_scene = 2;

                    //    // Create full path to the scene file
                    //    std::string scene_path = ASM.get_full_path("Scenes", scene_file);

                    //    // Try to load the new scene
                    //    if (SM.load_scene(scene_path.c_str())) {
                    //        LM.write_log("Game_Manager::update(): Successfully loaded %s", scene_file.c_str());

                    //        // Reset camera position only if not in main menu
                    //        auto& camera = GFXM.get_camera();
                    //        if (current_scene != 0) {
                    //            camera.pos_x = DEFAULT_CAMERA_POS_X;
                    //            camera.pos_y = DEFAULT_CAMERA_POS_Y;
                    //        }

                    //        // Stop all audio currently playing
                    //        ADM.stop_mastergroup();

                    //        // Reset player position only if in scene1 or scene2
                    //        if (current_scene != 0) {
                    //            EntityID playerId = ECSM.find_entity_by_name(DEFAULT_PLAYER_NAME);
                    //            if (playerId != INVALID_ENTITY_ID) {
                    //                if (ECSM.has_component<Transform2D>(playerId)) {
                    //                    auto& transform = ECSM.get_component<Transform2D>(playerId);
                    //                    transform.position = Vec2D(0.0f, 0.0f);
                    //                    transform.prev_position = transform.position;
                    //                }
                    //                if (ECSM.has_component<Velocity_Component>(playerId)) {
                    //                    auto& velocity = ECSM.get_component<Velocity_Component>(playerId);
                    //                    velocity.velocity = Vec2D(0.0f, 0.0f);
                    //                }
                    //            }
                    //        }
                    //    }
                    //    else {
                    //        LM.write_log("Game_Manager::update(): Failed to load %s", scene_file.c_str());

                    //        // Revert to main menu since load failed
                    //        current_scene = 0;
                    //    }

                    //}

                    //// Check if player is dead to reset the scene
                    //if (is_player_dead == true) {
                    //    tnt_to_destroy.clear();
                    //    IMGUIM.set_current_file_shown("scene2.scn");
                    //    break;
                    //}
                    //else {
                    //    // Destroy tnt and remove it from the list of tnt to destroy
                    //    ECSM.destroy_entity(tnt_id);
                    //    tnt_to_destroy.erase(current->first);
                    //    LM.write_log("Game_Manager::update: Removed block (Entity %u)", tnt_id);
                    //}
                }
            }
        }
	}

}