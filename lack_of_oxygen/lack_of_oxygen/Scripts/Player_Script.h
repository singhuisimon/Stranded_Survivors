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

	/**
	* @class Player_Script
	* @brief Defines the player script class.
	*/
	class Player_Script : public Script {

	public:

		/**
		 * @brief Constructor for Player_Script.
		 */
		Player_Script();

		/**
		 * @brief Register the script and function as well as implement the function
		 */
		void register_script() override;

		/**
		 * @brief Set the boolean for key press
		 */
		void check_keys();

		/**
		 * @brief Check if key has been pressed last frame.
		 */
		bool is_key_just_pressed(int key);

	private:

		//script name
		const std::string script_name = "player_script";

		//player id
		EntityID player_id;

		//for forces (movement)
		int forces_flag;

		//for keys
		bool key_space_pressed = false;
		bool key_space_last_frame = false;
		bool key_a_pressed = false;
		bool key_d_pressed = false;
		bool key_a_last_frame = false;
		bool key_d_last_frame = false;

		/**
		 * @brief Set the force flag for the player.
		 */
		void set_force_flag(int flag);

		/**
		 * @brief Get the force flag
		 */
		int get_force_flag() const;

		/**
		 * @brief Set the player id
		 */
		void set_player_id(EntityID entityid);

		/**
		 * @brief Get the player id
		 */
		EntityID get_player_id() const;

		/**
		 * @brief Updates player movement based on input.
		 * @param physic_comp The Physics_Component to update.
		 */
		void update_player_movement(Physics_Component& physic_comp);

		/**
		 * @brief Updates player animation based on input.
		 */
		void update_player_animation();
		
		/**
		 * @brief Updates player audio based on input.
		 * @param physic_comp The Physics_Component to update.
		 * @param audio_comp The Audio_Component to update.
		 */
		void update_player_audio(Physics_Component& physic_comp, Audio_Component& audio_comp);

		/**
		 * @brief Updates player walking particle based on input.
		 */
		void update_player_walking_particle();


		//---------------------------------------------------------------------------//
		bool key_e_last_frame = false; // for tunnel
		bool key_e_pressed = false;
		
		float teleport_cooldown = 0.5f;
		float last_teleport_time = -teleport_cooldown;
		bool teleport_flag;

		std::unordered_map<EntityID, EntityID> wormhole_pairs;
		Transform2D last_wormhole_position; // to store the last wormhole

		void handle_teleportation(EntityID player_id);
		bool is_player_inside_wormhole(Transform2D& player, Transform2D& wormhole);

		void teleport_player(EntityID wormhole_id, EntityID player_id, EntityID linked_wormhole);
		
	};

}


#endif // !LOF_PLAYER_SCRIPT_H