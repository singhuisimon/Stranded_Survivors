#include <iostream>
#include <string>
#include <random>
#include <iomanip>  // for std::setw and std::setfill
#include <sstream>  // for std::stringstream

#include "../Scripts/Tutorial_Lights_Script.h"
#include "../Manager/Log_Manager.h"
#include "../Manager/ECS_Manager.h"
#include "../Manager/FPS_Manager.h"

namespace lof {

    Tutorial_Light_Script::Tutorial_Light_Script() {}

    std::string Tutorial_Light_Script::get_type() const {
        return script_name;
    }

    void Tutorial_Light_Script::register_script() {
        auto tutorial_light_script = shared_from_this();

        tutorial_light_script->add_function("init", [weak_script = std::weak_ptr<Tutorial_Light_Script>(tutorial_light_script)](EntityID entity_id) {
            (void)entity_id;
            auto tutorial_light_script = weak_script.lock();
            tutorial_light_script->init_light();
            });

        tutorial_light_script->add_function("update", [weak_script = std::weak_ptr<Tutorial_Light_Script>(tutorial_light_script)](EntityID entity_id) {
            (void)entity_id;
            auto tutorial_light_script = weak_script.lock();
            tutorial_light_script->update_frame();
            tutorial_light_script->lights_animation();
            });

    }

    void Tutorial_Light_Script::init_light() {
        elapsed_frames = 0;
        current_frame_index = 1;
        elapsed_frames = 0;
    }

    void Tutorial_Light_Script::update_frame() {
        elapsed_frames++;

        if (current_frame_index == 1) {
            std::cout << "currently frame 1" << "at update frame" << std::endl;;
            if (elapsed_frames % 60 == 0) {
                current_frame_index = 2;
                elapsed_frames = 0;
                return;
            }
        }
        else if (current_frame_index == 2) {
            std::cout << "currently frame 2" << std::endl;
            if (elapsed_frames % 8 == 0) {
                current_frame_index = 3;
                elapsed_frames = 0;
                return;
            }
        }
        else if (current_frame_index == 3) {
            std::cout << "curently frame 3" << std::endl;
            if (elapsed_frames % 8 == 0) {
                current_frame_index = 4;
                elapsed_frames = 0;
                return;
            }
        }
        else if (current_frame_index == 4) {
            std::cout << "currently frame 4" << std::endl;
            if (elapsed_frames % 8 == 0) {
                current_frame_index = 1;
                elapsed_frames = 0;
                return;
            }
        }

    }

    void Tutorial_Light_Script::lights_animation() {

        EntityID light_left = ECSM.find_entity_by_name("siren_bulb_left");
        EntityID light_right = ECSM.find_entity_by_name("siren_bulb_right");

        if (light_left != INVALID_ENTITY_ID && light_right != INVALID_ENTITY_ID) {
            // Get both clouds transform
            auto& light_right_graphics = ECSM.get_component<Graphics_Component>(light_right);
            auto& light_left_graphics = ECSM.get_component<Graphics_Component>(light_left);

            light_right_graphics.texture_name = base_texture + std::to_string(current_frame_index);
            light_left_graphics.texture_name = base_texture + std::to_string(current_frame_index);
            std::cout << "currently both set to frame" << current_frame_index << std::endl;
            return;
        }
        std::cout << "didn't update animation" << std::endl;
    }

} // namespace lof