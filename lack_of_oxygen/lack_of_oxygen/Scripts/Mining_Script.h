/**
 * @file Mining_Script.h
 * @brief Define the mining_script class
 * @author Amanda Leow Boon Suan (70%), Chua Wen Bin Kenny (25%), Wai Lwin Thit (10%)
 * @date February 24, 2025
 * Copyright (C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */
#pragma once
#ifndef LOF_MINING_SCRIPT_H
#define LOF_MINING_SCRIPT_H

#include "../Scripts/Script.h"
#include "../Component/Component.h"
#include "../System/Particle_System.h"

namespace lof {
	class Mining_Script : public Script, public std::enable_shared_from_this<Mining_Script> {
	public:
		/**
		 * @brief Constructor for mining script
		 */
		Mining_Script();

		/**
		 * @brief Register the script as well as its function
		 */
		void register_script() override;

		/**
		 * @brief Cheat code to increase mining strength
		 */
		std::string get_type() const override;

	private:

		//the name of the script
		const std::string script_name = "mining_script";

		//the player id
		EntityID player_id;

		//the mining strength to be used
		unsigned int mining_strength;

		//for the mining cooldown and direction
		float mining_cooldown;
		std::string new_direction = "none";
		std::string current_mining_direction = "none";

		//for checking the mining keys
		bool left_key_pressed = false;
		bool left_key_last_frame = false;
		bool right_key_pressed = false;
		bool right_key_last_frame = false;
		bool up_key_pressed = false;
		bool up_key_last_frame = false;
		bool down_key_pressed = false;
		bool down_key_last_frame = false;

		//for cheat keys
		bool key_g_pressed = false;
		bool key_g_last_frame = false;
		bool key_h_pressed = false;
		bool key_h_last_frame = false;

		// Stores the TNTs that are activated with it's entity name and fuse time
		std::unordered_map<std::string, float> tnt_to_destroy;

		/**
		 * @brief Set the playere id for the entity
		 * @param entityid Consisting the player id in it.
		 */
		void set_player_id(EntityID entityid);

		/**
		 * @brief Retrieve playerid
		 * @return The entityid to the player
		 */
		EntityID get_player_id() const;

		/**
		 * @brief Update the boolean
		 */
		void check_keys();

		/**
		 * @brief Cheat code to increase mining strength
		 */
		void increase_mining_strength_cheat();

		/**
		 * @brief Cheat code to increase mineral
		 */
		void increase_mineral_count_cheat();

		/**
		 * @brief Check is key just pressed
		 * @param direction The direction in which the player mined
		 */
		bool is_key_just_pressed(const std::string& direction) const;

		/**
		 * @brief Update the surrounding tiles if it is tnt and if player collides with it
		 * @param particle_system The system used to generate particle
		 */
		void update_surrounding_blocks(Particle_System* particle_system);

		/**
		 * @brief Update the mining
		 * @param audio_comp The audio component of the player
		 * @param particle_system The system used to generate particle
		 */
		void update_mining(Audio_Component& audio_comp, Particle_System* particle_system);

		/**
		 * @brief Update the player animation regarding the mining direction
		 */
		void update_mining_direction();

		/**
		 * @brief Updates the tile
		 * @param block_to_remove The entityid of the block to be removed
		 * @param particle_system The system used to generate particle
		 */
		void update_tile(EntityID block_to_remove, Particle_System* particle_system);

		/**
		 * @brief Play the audio depending on the keys
		 * @param block_to_remove The entityid of the block to be removed
		 * @param audio_comp The audio component of the player
		 * @param key_1 The key to the audio in which audio will play when mineral value is > 0
		 * @param key_2 The key to the audio in which audio will play when mineral value is not > 0
		 */
		void update_mining_audio(EntityID block_to_remove, Audio_Component& audio_comp, const std::string key_1, const std::string key_2);

		/**
		 * @brief Generate particles when the block is being mined
		 * @param particle_system The system used to generate particle
		 * @param transform_comp The transform component of the entity to be destroyed
		 * @param animation_comp The animation component of the entity to be destroyed
		 * @return The mineral value associated with that block type
		 */
		void update_mining_particle(Particle_System* particle_system, Transform2D& transform_comp, Animation_Component& animation_comp);

		/**
		 * @brief Generate particles when the block is about to be destroyed
		 * @param particle_system The system used to generate particle
		 * @param transform_comp The transform component of the entity to be destroyed
		 * @param animation_comp The animation component of the entity to be destroyed
		 * @return The mineral value associated with that block type
		 */
		void update_final_mining_particle(Particle_System* particle_system, Transform2D& transform_comp, Animation_Component& animation_comp);

		/**
		 * @brief Determines the mineral value based on the block entity type
		 * @param block_id The EntityID of the block being destroyed
		 * @return The mineral value associated with that block type
		 */
		int get_mineral_value(EntityID block_id) const;

		/**
		 * @brief Determines the mineral value based on the block entity type
		 * @param block_id The EntityID of the block being destroyed
		 * @return The mineral value associated with that block type
		 */
		void update_mineral_count_text(int value_to_add);
	};
}

#endif 