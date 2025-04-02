/**
 * @file Win_Screen_Script.cpp
 * @brief Implements the script class for the Win Screen animation logic.
 * @details Initialize and update the animation and set up for win screen.
 * @author Chua Wen Bin Kenny (100%)
 * @date March 29, 2025
 * Copyright (C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

 // Include header file
#include "../Scripts/Win_Screen_Script.h"
#include "../Manager/Log_Manager.h"
#include "../Manager/ECS_Manager.h"
#include "../Manager/FPS_Manager.h"
#include "../Manager/Graphics_Manager.h"
#include "../Manager/Audio_Manager.h"
#include "../System/Particle_System.h"
#include "../Utility/Globals.h"

bool ship_launching = false;

// TESTING
#include "../Manager/Game_Manager.h"

namespace lof {

    // Constructor for Win_Screen_Script
    Win_Screen_Script::Win_Screen_Script() {
        ship_launching_speed = SHIP_STARTING_SPEED;
        background_speed = WIN_BACKGROUND_SPEED;
        launch_duration = DEFAULT_LAUNCH_DURATION;
        background_landed = false;
        win_audio_played = false;
    }

    // Returns the script name as a string
    std::string Win_Screen_Script::get_type() const {
        return script_name;
    }

    // Register the script by adding functions into it
    void Win_Screen_Script::register_script() {
        auto win_screen_script = shared_from_this();

        win_screen_script->add_function("init", [weak_script = std::weak_ptr<Win_Screen_Script>(win_screen_script)](EntityID entity_id) {
            (void)entity_id;
            auto win_screen_script = weak_script.lock();
            win_screen_script->win_screen_update();
            });

        win_screen_script->add_function("update", [weak_script = std::weak_ptr<Win_Screen_Script>(win_screen_script)](EntityID entity_id) {
            (void)entity_id;
            auto win_screen_script = weak_script.lock();
            win_screen_script->win_screen_update();
            });

    }

    // Updates the animation scene for win screen
    void Win_Screen_Script::win_screen_update() {

        // Get current scene
        int current_scene = GM.get_current_scene();

        // Check if current scene is win screen
        if (4 == current_scene) {

            // Get ship and win background ids
            EntityID win_background_id = ECSM.find_entity_by_name(win_background_name);
            EntityID ship_id = ECSM.find_entity_by_name(ship_name);
            EntityID restart_id = ECSM.find_entity_by_name(restart_button_name);
            EntityID main_menu_id = ECSM.find_entity_by_name(main_menu_button_name);

            // Change asset for ship
            if (ship_id != INVALID_ENTITY_ID) {
                auto& ship_graphics = ECSM.get_component<Graphics_Component>(ship_id);
                if (ship_graphics.texture_name != "ship_closed_batch_23") {
                    ship_graphics.texture_name = "ship_closed_batch_23";
                }
            }

            // Get ship and win background transform
            auto& ship_transform = ECSM.get_component<Transform2D>(ship_id);
            auto& win_background_transform = ECSM.get_component<Transform2D>(win_background_id);

            // Get camera
            auto& camera = GFXM.get_camera();

            // Get delta time
            float delta_time = FPSM.get_delta_time();

            // Win background movement update
            if (camera.pos_y == 1920.0f && launch_duration <= 2.5f && win_background_id != INVALID_ENTITY_ID) {

                if (win_background_transform.position.y > 1920.0f &&
                    2.5f >= launch_duration && launch_duration > 1.75f &&
                    background_landed == false) {
                    win_background_transform.position.y -= background_speed * delta_time;

                    // Check if win background has landed
                    if (win_background_transform.position.y <= 1920.0f) {
                        background_landed = true;
                        time_landed = launch_duration;
                        background_speed /= 4.0f;
                    }
                }
                else if (time_landed >= launch_duration && launch_duration > (time_landed - 0.2f)) { // 0.30s
                    win_background_transform.position.y += background_speed * delta_time;

                    // Accelerate at the start and decelerate at the end (Smooth out the movement)
                    if ((time_landed - launch_duration) < 0.1f) { // Accelerate
                        background_speed += background_speed * delta_time;
                    }
                    else { // Decelerate
                        background_speed -= background_speed * delta_time;
                    }
                }
                else if ((time_landed - 0.2f) >= launch_duration && launch_duration > (time_landed - 0.4f)) { // 0.20s
                    win_background_transform.position.y -= background_speed * delta_time;

                    // Accelerate at the start and decelerate at the end (Smooth out the movement)
                    if (((time_landed - 0.2f) - launch_duration) < 0.1f) { // Accelerate
                        background_speed += background_speed * delta_time;
                    }
                    else { // Decelerate
                        background_speed -= background_speed * delta_time;
                    }
                }
                else if ((time_landed - 0.4f) >= launch_duration && launch_duration > (time_landed - 0.6f)) { // 0.20s
                    win_background_transform.position.y += background_speed * delta_time;

                    // Accelerate at the start and decelerate at the end (Smooth out the movement)
                    if (((time_landed - 0.4f) - launch_duration) < 0.1f) { // Accelerate
                        background_speed += background_speed * delta_time;
                    }
                    else { // Decelerate
                        background_speed -= background_speed * delta_time;
                    }
                }
                else if ((time_landed - 0.6f) >= launch_duration &&
                    launch_duration > 0.0f &&
                    win_background_transform.position.y > 1920.0f) { // Until end of launch

                    win_background_transform.position.y -= background_speed / 2.0f * delta_time;

                    // Accelerate at the start and decelerate at the end (Smooth out the movement)
                    //if (((time_landed - 0.4f) - launch_duration) < 0.05f) { // Accelerate
                        //background_speed += background_speed * delta_time;
                    //}
                    //else { // Decelerate
                    background_speed -= background_speed * delta_time;
                    //}
                }
                else if (launch_duration <= 0.0f) {
                    win_background_transform.position.y = 1920.0f;
                }
            }

            // Display restart and main menu buttons
            if (launch_duration <= 0.0f) {
                if (restart_id != INVALID_ENTITY_ID && main_menu_id != INVALID_ENTITY_ID) {
                    // Get graphics component of restart and main menu button
                    auto& restart_graphics = ECSM.get_component<Graphics_Component>(restart_id);
                    auto& main_menu_graphics = ECSM.get_component<Graphics_Component>(main_menu_id);

                    // Set alpha to 1 to display buttons
                    restart_graphics.color.a = 1.0f;
                    main_menu_graphics.color.a = 1.0f;
                }

                //set global boolean to false
                ship_launching = false;
            }

            // Ship to launch up to the sky
            if (launch_duration > 0.0f) {
                // Decrement duration
                launch_duration -= delta_time;

                // Update boolean
                ship_launching = true;

                // Update the win audio
                if (!win_audio_played && ship_id != INVALID_ENTITY_ID) {
                    auto& ship_audio = ECSM.get_component<Audio_Component>(ship_id);
                    ADM.play_now(ship_id, "game win", ship_audio);
                    win_audio_played = true;
                }

                // Update ship launch speed and position
                if (7.0f >= launch_duration && launch_duration > 6.5f) { // 0.5s
                    ship_transform.position.y += delta_time * ship_launching_speed;
                    ship_launching_speed += ship_launching_speed * delta_time;
                }
                else if (6.5f >= launch_duration && launch_duration > 6.2f) { // 0.3s
                    ship_transform.position.y -= delta_time * ship_launching_speed;
                    ship_launching_speed += ship_launching_speed * delta_time;
                }
                else if (6.2f >= launch_duration && launch_duration > 5.2f) { // 1.0s
                    ship_transform.position.y += delta_time * ship_launching_speed;

                    // Accelerate at the start and decelerate at the end 
                    if ((6.2f - launch_duration) < 0.5f) { // Accelerate
                        ship_launching_speed += ship_launching_speed * delta_time;
                    }
                    else { // Decelerate
                        ship_launching_speed -= ship_launching_speed * delta_time;
                    }
                }
                else if (5.2f >= launch_duration && launch_duration > 4.7f) { // 0.5s
                    ship_transform.position.y -= delta_time * ship_launching_speed;

                    // Accelerate at the start and decelerate at the end 
                    if ((5.2f - launch_duration) < 0.25f) { // Accelerate
                        ship_launching_speed += ship_launching_speed * delta_time * 4;
                    }
                    else { // Decelerate
                        ship_launching_speed -= ship_launching_speed * delta_time;
                    }
                }
                else if (4.7f >= launch_duration && launch_duration > 3.0f) {
                    ship_transform.position.y += delta_time * ship_launching_speed;
                    ship_launching_speed += ship_launching_speed * delta_time * 2;
                }
            }

            // Emit particles
            if (7.0f >= launch_duration && launch_duration > 0.0f) {
                for (auto& system : ECSM.get_systems()) {
                    if (system->get_type() == "Particle_System") {
                        auto* particle_system = static_cast<Particle_System*>(system.get());
                        if (!particle_system) {
                            LM.write_log("Mining_Script::update_mining(): Fail to get particle system");
                            std::cerr << "Failed to get particle system" << std::endl;
                            return;
                        }

                        // Emit flame particles
                        if (ship_transform.position.y > ship_transform.prev_position.y) {
                            for (int i = 0; i < 4; ++i) {
                                // Randomize particle emit location under the ship
                                float rand_float_val = particle_system->get_rand_float();
                                float part_x = ship_transform.position.x - (ship_transform.scale.x / 20.0f) + (rand_float_val * ship_transform.scale.x / 10.0f);

                                // Set direction for flame particles. Those to the left of ship will float left, vice versa for right
                                float direction = 225.0f + (rand_float_val * 90.0f);

                                // Lifetime for particle
                                float lifetime = 0.2f;
                                if (launch_duration <= 4.8f) {
                                    lifetime = 1.5f;
                                }

                                float part_y = ship_transform.position.y - (ship_transform.scale.y / 2.4f);
                                particle_system->particle_emit("ship_takeoff_flame", Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f), lifetime, direction);
                            }
                        }

                        // Emit smoke particles
                        int smoke_particle_cnt = 1;
                        if (4.8f >= launch_duration && launch_duration > 3.0f) {
                            smoke_particle_cnt = 2;
                        }
                        if (ship_transform.position.y > ship_transform.prev_position.y) {
                            for (int i = 0; i < smoke_particle_cnt; ++i) {
                                // Randomize particle emit location under the ship
                                float rand_float_val = particle_system->get_rand_float();
                                float part_x = ship_transform.position.x - (ship_transform.scale.x / 20.0f) + (rand_float_val * ship_transform.scale.x / 10.0f);

                                // Set direction for smoke particles. Those to the left of ship will float left, vice versa for right
                                float direction = 180.0f;
                                if (rand_float_val > 0.5f) {
                                    direction += 180.0f;
                                }

                                float part_y = ship_transform.position.y - (ship_transform.scale.y / 2.4f);
                                particle_system->particle_emit("ship_takeoff_smoke", Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f), 3.0f, direction);
                            }
                        }

                        // Emit ground particles
                        for (int i = 0; i < 2; ++i) {
                            // Randomize particle emit location under the ship
                            float rand_float_val = particle_system->get_rand_float();
                            float part_x = ship_transform.prev_position.x - (ship_transform.scale.x / 4.0f) + (rand_float_val * ship_transform.scale.x / 2.0f);

                            // Set direction for dirt particles. Those to the left of ship will float left, vice versa for right
                            float direction = 135.0f - (rand_float_val * 90.0f);

                            // Randomize lifetime for particles
                            float lifetime = 0.5f + 1.0f * particle_system->get_rand_float();

                            float part_y = ship_transform.prev_position.y - (ship_transform.scale.y / 2.4f);
                            particle_system->particle_emit("ship_takeoff_dirt", Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f), lifetime, direction);
                        }
                    }
                }
            }
        }
        else {
            // Set everything to default
            if (launch_duration <= 0.0f) {
                ship_launching_speed = SHIP_STARTING_SPEED;
                background_speed = WIN_BACKGROUND_SPEED;
                launch_duration = DEFAULT_LAUNCH_DURATION;
                background_landed = false;
                win_audio_played = false;
            }
        }


    }

} // namespace lof