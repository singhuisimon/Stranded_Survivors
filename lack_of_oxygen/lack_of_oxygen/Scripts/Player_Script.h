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
	class Player_Script : public Script, public std::enable_shared_from_this<Player_Script> {

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
		//void add_panic_movespeed(float panic_amount); 

		//void drop_panic_movespeed(float panic_amount); 
		std::string get_type() const override;

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

		bool checked_once = false;


		//for panic 
		float panic_level; 
		float f_mag_original; 

		void set_once_flag(bool flag) { checked_once = flag; }

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

		/*
		* @brief update the player's movement speed according to the panic level
		* @param physics_comp player's physics component
		*/

		void update_player_panic_speed(Physics_Component& physics_comp); 

		/*
		* @brief update the player's moving forces according to the panic level
		* max speed is capped at 2 times of the original
		*/
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

		/**
		* @brief Collision for the player and lava and return to game over screen if player interact with lava pool
		*/
		void check_player_fall_into_lava();
		//---------------------------------------------------------------------------//
		bool key_e_last_frame = false; // for tunnel
		bool key_e_pressed = false;

	
		bool key_t_last_frame = false; // for tunnel
		bool key_t_pressed = false;

		bool key_y_last_frame = false; // for tunnel
		bool key_y_pressed = false;


		bool key_u_last_frame = false;
		bool key_u_pressed = false;
		
		float teleport_cooldown = 0.5f;
		float last_teleport_time = -teleport_cooldown;
		bool teleport_flag;
		bool is_inside = false; // to check if player is inside 
		//float just_teleported = false;

		//bool was_inside_last_frame = false;
		EntityID active_wormhole = INVALID_ENTITY_ID;
		//EntityID last_active_wormhole = INVALID_ENTITY_ID;

		//bool found_wormhole = false;

		// to store the pair of the wormholes
		std::unordered_map<EntityID, EntityID> wormhole_pairs;
		
		/*
		* @brief Check if player is interact with the wormholes
		* @param player to get transfromation component of player 
		* @param wormhole to get transfromation component of wormhole
		* @return true if player interact with wormhole, false if not 
		*/
		bool is_player_inside_wormhole(Transform2D& player, Transform2D& wormhole);

		/*
		* @brief Teleport player from one wormhole to another lineked wormhole
		* @param Player_ID pass through player id 
		* @param linked_wormhole entity id of the linked wormhole
		*/
		void teleport_player(EntityID Player_ID, EntityID linked_wormhole);

		/*
		* @brief To handle how the player teleport from one wormhole to another 
		* @param Player_ID pass through player id
		*/
		void handle_teleportation(EntityID Player_ID);
		/*
		* @brief Use the cheap code to teleport player for demostrate and debug purpose
		* @param pos_x player position for the x axis 
		* @param pos_y player position for the y axis 
		*/
		void Cheap_Code_Teleport_Wormhole(float pos_x, float pos_y);
		//void Show_E_Prompt_UI(bool show_ui, Transform2D& prompt_pos);
	};

}


#endif // !LOF_PLAYER_SCRIPT_H