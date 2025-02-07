/**
 * @file Player_Scrupt.h
 * @brief Define the player script class.
 * @author Amanda Leow Boon Suan (100%)
 * @date January 15, 2025
 * Copyright (C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#pragma once
#ifndef LOF_PLAYER_SCRIPT_H
#define LOF_PLAYER_SCRIPT_H

#include "../Scripts/Script.h"
#include "../Component/Component.h"

namespace lof {

	class Player_Script : public Script {

	public:
		Player_Script();
		void register_script() override;
		void check_keys();
		bool is_key_just_pressed(int key);

	private:

		const std::string script_name = "player_script";

		EntityID player_id;

		int forces_flag;

		bool key_space_pressed = false;
		bool key_space_last_frame = false;
		bool key_a_pressed = false;
		bool key_d_pressed = false;
		bool key_a_last_frame = false;
		bool key_d_last_frame = false;

		void set_force_flag(int flag);

		int get_force_flag() const;

		void set_player_id(EntityID entityid);

		EntityID get_player_id() const;

		/**
		* @brief Updates linear movement for a given entity.
		* @param logic The Logic_Component containing movement parameters.
		* @param transform The Transform2D to update.
		* @param delta_time Time elapsed since the last update.
		*/
		void update_player_movement(Physics_Component& physic_comp);

		void update_player_animation();

		/**
		* @brief Updates circular movement for a given entity.
		* @param logic The Logic_Component containing movement parameters.
		* @param transform The Transform2D to update.
		* @param delta_time Time elapsed since the last update.
		*/
		void update_player_audio(Physics_Component& physic_comp, Audio_Component& audio_comp);

		void update_player_walking_particle();
		
	};

}


#endif // !LOF_PLAYER_SCRIPT_H