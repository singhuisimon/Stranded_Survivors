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
#include "../Utility/Constant.h"

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

		/**
		*@brief increase player movement speed based on the panic level
		* @param current panic (0-100)
		*/
		void add_panic_movespeed(float panic_amount); 

		void drop_panic_movespeed(float panic_amount); 

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


		//for panic 
		float panic_level; 
		float f_mag_original; 


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

		void update_panic_level(Physics_Component& physics_comp); 

		void update_movement_forces(Physics_Component& physics_comp); 
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
		
	};

}


#endif // !LOF_PLAYER_SCRIPT_H