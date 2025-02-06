

#include "../Scripts/Player_Script.h"
#include "../System/Particle_System.h"
#include "../Manager/Audio_Manager.h"
#include "../Manager/ECS_Manager.h"
#include "../Manager/Graphics_Manager.h"

namespace lof {

    Player_Script::Player_Script() {
        //entity_data = std::unordered_map<EntityID, MovementData>();
        key_space_last_frame = false;
		key_space_pressed = false;
		key_a_pressed = false;
		key_d_pressed = false;
		key_a_last_frame = false;
		key_d_last_frame = false;
		forces_flag = -1;
		player_id = 0;
    }

    void Player_Script::register_script() {

        std::shared_ptr<Player_Script> player_script = std::make_shared<Player_Script>();
        static auto maintained_script = player_script;
        std::weak_ptr<Player_Script> weak_script = player_script;

        player_script->add_function("init", [weak_script](EntityID entity_id) {
			auto player_script = weak_script.lock();
            player_script->set_player_id(ECSM.find_entity_by_name(DEFAULT_PLAYER_NAME));
            player_script->set_force_flag(-1);
        });

        player_script->add_function("movement", [weak_script](EntityID entity_id) {
            auto player_script = weak_script.lock();
            if (!entity_id || !ECSM.has_component<Physics_Component>(entity_id) || !ECSM.has_component<Audio_Component>(entity_id) ||
                !ECSM.has_component<Transform2D>(entity_id)) {
                LM.write_log("Player_Script::register_script(): Entity %d does not have required components.", entity_id);
                return;
            }

            if (entity_id != player_script->get_player_id()) {
                LM.write_log("Player_Script::register_script(): Entity %d is not the player.", entity_id);
                return;
            }

            auto& physics_comp = ECSM.get_component<Physics_Component>(entity_id);
            auto& audio_comp = ECSM.get_component<Audio_Component>(entity_id);

            player_script->check_keys();

            //update player horizontal movement
            player_script->update_player_movement(physics_comp);
            player_script->update_player_audio(physics_comp, audio_comp);
            player_script->update_player_animation();

        });

        LGS.add_script("player_script", player_script);
    }

    void Player_Script::check_keys() {
		//check for keys here
		key_space_last_frame = key_space_pressed;
		key_a_last_frame = key_a_pressed;
		key_d_last_frame = key_d_pressed;
        
        key_space_pressed = IM.is_key_held(GLFW_KEY_SPACE);
		key_a_pressed = IM.is_key_held(GLFW_KEY_A);
		key_d_pressed = IM.is_key_held(GLFW_KEY_D);
    }

    bool Player_Script::is_key_just_pressed(int key) {
        if (key == GLFW_KEY_SPACE) {
            return key_space_pressed && !key_space_last_frame;
        }
        else if (key == GLFW_KEY_A) {
			return key_a_pressed && !key_a_last_frame;
        }
        else if (key == GLFW_KEY_D) {
            return key_d_pressed && !key_d_last_frame;
        }
        return false;
    }

	void Player_Script::set_force_flag(int flag) {
		forces_flag = flag;
	}

    int Player_Script::get_force_flag() const{
        return forces_flag;
    }

    void Player_Script::set_player_id(EntityID entityid) {
		player_id = entityid;
    }

    EntityID Player_Script::get_player_id() const{
		return player_id;
    }

    void Player_Script::update_player_movement(Physics_Component& physic_comp) {

        // Handle horizontal movement
        if (is_key_just_pressed(GLFW_KEY_SPACE)) {
            physic_comp.set_jump_requested(true); //this will set the flag to true inside the physics_component 
        }
        else {
            physic_comp.set_jump_requested(false);
        }

        //activate and deactivate the forces. 
        if (IM.is_key_held(GLFW_KEY_A) && !(IM.is_key_held(GLFW_KEY_D))) {
            // Updates forces
            physic_comp.force_helper.deactivate_force(MOVE_RIGHT);
            physic_comp.force_helper.activate_force(MOVE_LEFT);
            forces_flag = MOVE_LEFT;

            //std::cout << "moving left current scene number is " << current_scene << std::endl;
        }
        else if (IM.is_key_held(GLFW_KEY_D) && !(IM.is_key_held(GLFW_KEY_A))) {
            // Update forces
            physic_comp.force_helper.deactivate_force(MOVE_LEFT);
            physic_comp.force_helper.activate_force(MOVE_RIGHT);
            forces_flag = MOVE_RIGHT;

        }
        else if (IM.is_key_held(GLFW_KEY_D) && IM.is_key_held(GLFW_KEY_A)) {
            if (forces_flag == MOVE_LEFT) {
                // Update forces
                physic_comp.force_helper.activate_force(MOVE_LEFT);
                forces_flag = MOVE_LEFT;
            }
            else {
                // Update forces
                physic_comp.force_helper.deactivate_force(MOVE_LEFT);
                physic_comp.force_helper.activate_force(MOVE_RIGHT);
                forces_flag = MOVE_RIGHT;
            }
        }
        else {
            // Reset forces
            physic_comp.force_helper.deactivate_force(MOVE_LEFT);
            physic_comp.force_helper.deactivate_force(MOVE_RIGHT);
            forces_flag = -1;
        }
    }

    void Player_Script::update_player_animation() {

        //activate and deactivate the forces. 
        if (IM.is_key_held(GLFW_KEY_A) && !(IM.is_key_held(GLFW_KEY_D))) {

            // Update player animation flag
            int& direction = GFXM.get_player_direction();
            direction = FACE_LEFT;
            int& moving_status = GFXM.get_moving_status();
            moving_status = RUN_LEFT;

            //std::cout << "moving left current scene number is " << current_scene << std::endl;
        }
        else if (IM.is_key_held(GLFW_KEY_D) && !(IM.is_key_held(GLFW_KEY_A))) {

            // Update player animation flag
            int& direction = GFXM.get_player_direction();
            direction = FACE_RIGHT;
            int& moving_status = GFXM.get_moving_status();
            moving_status = RUN_RIGHT;

        }
        else if (IM.is_key_held(GLFW_KEY_D) && IM.is_key_held(GLFW_KEY_A)) {
            if (forces_flag == MOVE_LEFT) {
                // Update player animation flag
                int& direction = GFXM.get_player_direction();
                direction = FACE_LEFT;
                int& moving_status = GFXM.get_moving_status();
                moving_status = RUN_LEFT;
            }
            else {
                // Update player animation flag
                int& direction = GFXM.get_player_direction();
                direction = FACE_RIGHT;
                int& moving_status = GFXM.get_moving_status();
                moving_status = RUN_RIGHT;
            }
        }
        else {
            // Reset player animation
            int& moving_status = GFXM.get_moving_status();
            moving_status = NO_ACTION;

        }


        //player mining animation
        if (IM.is_key_held(GLFW_KEY_LEFT)) {
            auto& mining_status = GFXM.get_mining_status();
            mining_status = MINE_LEFT;
            int& direction = GFXM.get_player_direction();
            direction = FACE_LEFT;

        }
        else if (IM.is_key_held(GLFW_KEY_UP)) {
            auto& mining_status = GFXM.get_mining_status();
            mining_status = MINE_UP;

        }
        else if (IM.is_key_held(GLFW_KEY_DOWN)) {
            auto& mining_status = GFXM.get_mining_status();
            mining_status = MINE_DOWN;

        }
        else if (IM.is_key_held(GLFW_KEY_RIGHT)) {
            auto& mining_status = GFXM.get_mining_status();
            mining_status = MINE_RIGHT;
            int& direction = GFXM.get_player_direction();
            direction = FACE_RIGHT;

        }
        else {
            auto& mining_status = GFXM.get_mining_status();
            mining_status = NO_ACTION;
        }
    }

    void Player_Script::update_player_audio(Physics_Component& physic_comp, Audio_Component& audio_comp) {
        //audio logic is here.
        if (forces_flag != -1) {
            if (physic_comp.get_is_grounded()) {
                if (forces_flag == MOVE_RIGHT || forces_flag == MOVE_LEFT) {

                    ADM.play_now(player_id, "moving", audio_comp);
                    update_player_walking_particle();
                }
            }
        }
        else {
            if (!is_key_just_pressed(GLFW_KEY_D) || !is_key_just_pressed(GLFW_KEY_A)) {
                ADM.stop_now(player_id, "moving", audio_comp.get_filepath("moving"));
            }
        }
    }

    void Player_Script::update_player_walking_particle() {
        //animation logic is here.

        for (auto& system : ECSM.get_systems()) {
            if (system->get_type() == "Particle_System") {
                auto* particle_system = dynamic_cast<Particle_System*>(system.get());
                if (!particle_system) {
                    LM.write_log("Player_Script::update_player_animation(): Failed to get Particle_System.");
                    return;
                }
                // Emit walking dirt particles
                auto& player_transform = ECSM.get_component<Transform2D>(player_id);
                float part_x = player_transform.position.x - (player_transform.scale.x / 2.0f) + (particle_system->get_rand_float() * player_transform.scale.x);
                float part_y = player_transform.position.y - (player_transform.scale.y * 0.45f);
                particle_system->particle_emit("walking", Vec2D(part_x, part_y), Vec3D(1.0f, 1.0f, 1.0f));
            }

        }

    }
}