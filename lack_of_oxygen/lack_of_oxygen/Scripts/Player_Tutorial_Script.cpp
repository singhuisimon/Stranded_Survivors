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

    Player_Tutorial_Script::Player_Tutorial_Script() {
        moving_speed = 10; //10 px per sec
    }

    std::string Player_Tutorial_Script::get_type() const {
        return script_name;
    }

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

    static float timer = 0.0f;
    static bool up_direction = true;  //Initial direction

    void Player_Tutorial_Script::player_tutorial_update() const {
        EntityID player_in_chair_id = ECSM.find_entity_by_name("player_behind_chair");

        if (player_in_chair_id != INVALID_ENTITY_ID) {
            auto& player_in_chair_transform = ECSM.get_component<Transform2D>(player_in_chair_id);

            //Time per frame
            float delta_time = FPSM.get_delta_time();  
            //Pixels per second
            float moving_speed = 20.0f;
            float one_way_movement_time = 0.5f;

            timer += delta_time;

            if (up_direction) {
                player_in_chair_transform.position.y += moving_speed * delta_time;
            }
            else {
                player_in_chair_transform.position.y -= moving_speed * delta_time;
            }

            //Switching direction
            if (timer >= one_way_movement_time) {
                up_direction = !up_direction;

                //Reset timer
                timer = 0.0f;
            }
        }
    }

} // namespace lof