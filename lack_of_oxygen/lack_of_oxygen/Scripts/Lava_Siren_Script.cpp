/**
 * @file Lava_Siren_Script.cpp
 * @brief Implements the script class for the Lava Siren playback logic.
 * @details Initialize and update the script for lava siren sound in the game.
 * @author Amanda Leow Boon Suan (100%)
 * @date March 30, 2025
 * Copyright (C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

 // Include header file
#include "../Scripts/Lava_Siren_Script.h"
#include "../Manager/Log_Manager.h"
#include "../Manager/ECS_Manager.h"
#include "../Manager/FPS_Manager.h"
#include "../Manager/Audio_Manager.h"

namespace lof {

    // Returns the script name as a string
    std::string Lava_Siren_Script::get_type() const {
        return script_name;
    }

    // Register the script by adding functions into it
    void Lava_Siren_Script::register_script() {
        auto lava_siren_script = shared_from_this();

        lava_siren_script->add_function("init", [weak_script = std::weak_ptr<Lava_Siren_Script>(lava_siren_script)](EntityID entity_id) {
            //(void)entity_id;
            auto lava_siren_script = weak_script.lock();
            lava_siren_script->set_lava_timer(DEFAULT_LAVA_SIREN_COUNTDOWN, entity_id);
            });

        lava_siren_script->add_function("update", [weak_script = std::weak_ptr<Lava_Siren_Script>(lava_siren_script)](EntityID entity_id) {
            auto lava_siren_script = weak_script.lock();
            lava_siren_script->update_lava_timer(entity_id);
            });
    }
    
    void Lava_Siren_Script::set_lava_timer(float new_timer, EntityID entity_id) {
        timer = new_timer;
        if (ECSM.has_component<Audio_Component>(entity_id)) {
            auto& bg_audio = ECSM.get_component<Audio_Component>(entity_id);
            bg_audio.set_isactive(lava_key, false);
        }
    }

    void Lava_Siren_Script::update_lava_timer(EntityID entity_id) {
        if (timer > 0.0f) {
            timer -= FPSM.get_delta_time();
        }
        else {
            if (ECSM.has_component<Audio_Component>(entity_id)) {
                auto& bg_audio = ECSM.get_component<Audio_Component>(entity_id);
                bg_audio.set_isactive(lava_key, true);
            }
        }
    }

} // namespace lof