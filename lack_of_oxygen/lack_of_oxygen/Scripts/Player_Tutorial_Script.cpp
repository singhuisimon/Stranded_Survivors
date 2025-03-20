/**
 * @file Player_Tutorial_Script.cpp
 * @brief Define the functions in player tutorial script class.
 * @author Liliana Hanawardani (100%)
 * @date February 13, 2025
 * Copyright (C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#include <iostream>
#include <string>
#include <random>
#include <iomanip>  // for std::setw and std::setfill
#include <sstream>  // for std::stringstream

#include "../Scripts/Player_Tutorial_Script.h"
#include "../Manager/Log_Manager.h"
#include "../Manager/ECS_Manager.h"
#include "../Manager/FPS_Manager.h"

namespace lof {

    //Constructor for Player_Tutorial_Script
    Player_Tutorial_Script::Player_Tutorial_Script() {
        ;
    }

    //Returns name of script as string
    std::string Player_Tutorial_Script::get_type() const {
        return script_name;
    }

    //Register the script by adding functions into it
    void Player_Tutorial_Script::register_script() {
        auto player_tutorial_script = shared_from_this();

        player_tutorial_script->add_function("init", [weak_script = std::weak_ptr<Player_Tutorial_Script>(player_tutorial_script)](EntityID entity_id) {
            (void)entity_id;
            auto tutorial_player_script = weak_script.lock();
            tutorial_player_script->player_tutorial_update();
            });

        player_tutorial_script->add_function("update", [weak_script = std::weak_ptr<Player_Tutorial_Script>(player_tutorial_script)](EntityID entity_id) {
            (void)entity_id;
            auto tutorial_player_script = weak_script.lock();
            tutorial_player_script->player_tutorial_update();
            });

    }

    //Static variable to hold timer for 0.5 seconds
    static float timer = 0.0f;

    //Set Initial direction
    static bool up_direction = true;  

    //Controls the player tutorial movement in chair
    void Player_Tutorial_Script::player_tutorial_update() const {
        EntityID player_in_chair_id = ECSM.find_entity_by_name("player_behind_chair");

        //Get player ID behind the chair
        if (player_in_chair_id != INVALID_ENTITY_ID) {
            auto& player_in_chair_transform = ECSM.get_component<Transform2D>(player_in_chair_id);

            //Time per frame
            float delta_time = FPSM.get_delta_time();  

            //Set timer limit to 0.5 sceonds
            float one_way_movement_time = 0.5f;

            //Progress in timer
            timer += delta_time;

            //Switching direction
            if (timer >= one_way_movement_time) {

                //Switch directions
                up_direction = !up_direction;

                //Snap player to 10 pixels up/down
                if (up_direction) {
                    player_in_chair_transform.position.y += 10;
                }
                else {
                    player_in_chair_transform.position.y -= 10;
                }

                //Reset timer
                timer = 0.0f;
            }
        }
    }

} // namespace lof