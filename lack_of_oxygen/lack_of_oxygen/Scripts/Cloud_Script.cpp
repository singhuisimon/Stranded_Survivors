#include <iostream>
#include <string>
#include <random>
#include <iomanip>  // for std::setw and std::setfill
#include <sstream>  // for std::stringstream

#include "../Scripts/Cloud_Script.h"
#include "../Manager/Log_Manager.h"
#include "../Manager/ECS_Manager.h"
#include "../Manager/FPS_Manager.h"

namespace lof {

    Cloud_Script::Cloud_Script() {
        moving_speed = CLOUD_MOVING_SPEED;
    }

    std::string Cloud_Script::get_type() const {
        return script_name;
    }

    void Cloud_Script::register_script() {
        auto cloud_script = shared_from_this();

        cloud_script->add_function("init", [weak_script = std::weak_ptr<Cloud_Script>(cloud_script)](EntityID entity_id) {
            (void)entity_id;
            auto cloud_script = weak_script.lock();
            cloud_script->cloud_movement_update();
            });

        cloud_script->add_function("update", [weak_script = std::weak_ptr<Cloud_Script>(cloud_script)](EntityID entity_id) {
            (void)entity_id;
            auto cloud_script = weak_script.lock();
            cloud_script->cloud_movement_update();
            });

    }

    void Cloud_Script::cloud_movement_update() const {
        EntityID cloud_1_id = ECSM.find_entity_by_name("cloud_1");
        EntityID cloud_2_id = ECSM.find_entity_by_name("cloud_2");

        if (cloud_1_id != INVALID_ENTITY_ID && cloud_2_id != INVALID_ENTITY_ID) {
            // Get both clouds transform
            auto& cloud_1_transform = ECSM.get_component<Transform2D>(cloud_1_id);
            auto& cloud_2_transform = ECSM.get_component<Transform2D>(cloud_2_id);
            
            // Determine cloud movement by current game fps
            float fps = FPSM.get_current_fps();  
            cloud_1_transform.position.x += (moving_speed / fps);
            cloud_2_transform.position.x += (moving_speed / fps);

            // Reset cloud if it goes out of the screen
            unsigned int screen_width = SM.get_scr_width();
            if (cloud_1_transform.position.x >= static_cast<float>(screen_width)) {
                cloud_1_transform.position.x = cloud_2_transform.position.x - static_cast<float>(screen_width);
            }

            if (cloud_2_transform.position.x >= static_cast<float>(screen_width)) {
                cloud_2_transform.position.x = cloud_1_transform.position.x - static_cast<float>(screen_width);
            }
        }
    }

} // namespace lof