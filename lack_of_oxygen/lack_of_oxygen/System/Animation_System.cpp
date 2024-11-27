/**
 * @file Animation_System.cpp
 * @brief Implements the Animation_System class for the ECS that
 *        updates animation of the entity based on the animation and frame data.
 * @author Chua Wen Bin Kenny (100%)
 * @date October 25, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

 // Include header file
#include "Animation_System.h"
#include "../Manager/ECS_Manager.h"
#include "../Component/Component.h"

namespace lof {

    Animation_System::Animation_System(){ 
        // Set the required components for this system
        signature.set(ECSM.get_component_id<Animation_Component>());
    }

    std::string Animation_System::get_type() const { 
        return "Animation_System";
    }

    // Updates the animation of the entity according to the current frame and time delay.
    void Animation_System::update(float delta_time) { 

        // Get a reference to the animation container
        auto& animations_storage = GFXM.get_animation_storage();   
        EntityID player_id = ECSM.find_entity_by_name(DEFAULT_PLAYER_NAME);

        // Loop over the entities that match the system's signature
        for (EntityID entity_id : get_entities()) {  

            // Get reference to the animation component
            auto& animation_comp = ECSM.get_component<Animation_Component>(entity_id);  

            // Determine which frame of which animation to play
            // Set logic for player animation here first (This should be done before coming into animation system)
            if (entity_id == player_id) { 

                int& player_direction = GFXM.get_player_direction(); 

                // Determine player animation index
                // (When changing animation index, reset previous animation curr_frame_idx and frame_time_elapsed to 0)
                if (player_direction == MOVE_LEFT) { // Set running_left animation if not playing currently
                    if (animation_comp.curr_animation_idx != 2) {
                        animation_comp.curr_frame_index = 0; // Reset previous animation curr_frame_idx
                        animations_storage[animation_comp.animations[std::to_string(animation_comp.curr_animation_idx)]].frame_elapsed_time = 0.0f; // Reset previous animation frame_time_elapsed
                        animation_comp.curr_animation_idx = 2;
                    }
                } else if (player_direction == MOVE_RIGHT) { // set running_right animation if not playing currently
                    if (animation_comp.curr_animation_idx != 3) {
                        animation_comp.curr_frame_index = 0; // Reset previous animation curr_frame_idx
                        animations_storage[animation_comp.animations[std::to_string(animation_comp.curr_animation_idx)]].frame_elapsed_time = 0.0f; // Reset previous animation frame_time_elapsed 
                        animation_comp.curr_animation_idx = 3;
                    }
                } else { // Idling (No animation)
                    // Check last direction based on last animation
                    if (animation_comp.curr_animation_idx == 2) { // Was running left
                        animation_comp.curr_frame_index = 0; // Reset running_left animation curr_frame_idx  
                        animations_storage[animation_comp.animations["2"]].frame_elapsed_time = 0.0f; // Reset running_left animation frame_time_elapsed
                        animation_comp.curr_animation_idx = 0;    // Set idle left
                    } else if (animation_comp.curr_animation_idx == 3) { // Was running right
                        animation_comp.curr_frame_index = 0; // Reset running_right animation curr_frame_idx 
                        animations_storage[animation_comp.animations["3"]].frame_elapsed_time = 0.0f; // Reset running_right animation frame_time_elapsed
                        animation_comp.curr_animation_idx = 1;    // Set idle right
                    }
                }

                // Retrieve the time delay of the current frame in the current animation
                std::string const& curr_animation_name = animation_comp.animations[std::to_string(animation_comp.curr_animation_idx)]; 
                //unsigned int& curr_frame_idx = animations_storage[curr_animation_name].curr_frame_index; 
                float delay = animations_storage[curr_animation_name].frames[animation_comp.curr_frame_index].time_delay;

                // Calculate the time delay to determine current frame index of current animation
                if (delay == 0) { // delay = 0 means animation only has one frame, no changes 
                    continue;
                } else if ((animations_storage[curr_animation_name].frame_elapsed_time < delay)) { // Frame elapsed time is still within delay time, increment by frame delta time
                    animations_storage[curr_animation_name].frame_elapsed_time += delta_time * MILLISECONDS_PER_SECOND; 
                } else { // Frame elapsed time has surpassed frame's delay time, time to change frame
                    animations_storage[curr_animation_name].frame_elapsed_time = DEFAULT_FRAME_TIME_ELAPSED;
                    if (animation_comp.curr_frame_index < (animations_storage[curr_animation_name].frames.size() - 1)) {
                        animation_comp.curr_frame_index++;
                    } else { // Reset animation to the start
                        animation_comp.curr_frame_index = 0;
                    }
                }
            } else { // Other objects that animates based on time only (i.e vent, etc)
                // Retrieve the time delay of the current frame in the current animation
                std::string const& curr_animation_name = animation_comp.animations[std::to_string(animation_comp.curr_animation_idx)]; 
                unsigned int& curr_frame_idx = animations_storage[curr_animation_name].curr_frame_index; // For shared animations
                
                // Shared animations v.s. individual animations
                float delay = 0;
                if (curr_animation_name == "vent_strip" || curr_animation_name == "lava") {
                    delay = animations_storage[curr_animation_name].frames[curr_frame_idx].time_delay;
                } else {
                    delay = animations_storage[curr_animation_name].frames[animation_comp.curr_frame_index].time_delay;
                }

                // Calculate the time delay to determine current frame index of current animation
                if (delay == 0) { // delay = 0 means animation only has one frame, no changes
                    if (animation_comp.curr_tile_health <= (animation_comp.start_tile_health / 2)) {
                        animation_comp.curr_frame_index = 1;
                    }
                    continue;
                } else if ((animations_storage[curr_animation_name].frame_elapsed_time < delay)) { // Frame elapsed time is still within delay time, increment by frame delta time
                    if (animations_storage[curr_animation_name].is_updated == true) {
                        continue;
                    }
                    animations_storage[curr_animation_name].frame_elapsed_time += delta_time * MILLISECONDS_PER_SECOND;
                    animations_storage[curr_animation_name].is_updated = true;
                } else { // Frame elapsed time has surpassed frame's delay time, time to change frame
                    animations_storage[curr_animation_name].frame_elapsed_time = DEFAULT_FRAME_TIME_ELAPSED; 
                    if (curr_frame_idx < (animations_storage[curr_animation_name].frames.size() - 1)) {
                        curr_frame_idx++;
                    } else { // Reset animation to the start
                        curr_frame_idx = 0;
                    }
                }
            }    
        }
        // Reset animations update status
        for (auto& animation : animations_storage) {
            animation.second.is_updated = false;
        }
    }

} // namespace lof
