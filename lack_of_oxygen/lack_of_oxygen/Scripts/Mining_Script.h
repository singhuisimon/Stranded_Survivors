#pragma once
#ifndef LOF_MINING_SCRIPT_H
#define LOF_MINING_SCRIPT_H

#include "../Scripts/Script.h"
#include "../Component/Component.h"
#include "../System/Particle_System.h"

namespace lof {
	class Mining_Script : public Script {
	public:
		Mining_Script();

		void register_script() override;

	private:

		const std::string script_name = "mining_script";

		EntityID player_id;

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

		void set_player_id(EntityID entityid);

		EntityID get_player_id() const;

		void check_keys();

		void increase_mining_strength_cheat();

		void increase_mineral_count_cheat();

		bool is_key_just_pressed(const std::string& direction) const;

		void update_surrounding_blocks(Particle_System* particle_system);

		void update_mining(Audio_Component& audio_comp, Particle_System* particle_system);

		void update_mining_direction();

		void update_tile(int block_to_remove, Particle_System* particle_system);

		void update_mining_audio(int block_to_remove, Audio_Component& audio_comp, const std::string key_1, const std::string key_2);

		void update_mining_particle(Particle_System* particle_system, Transform2D& transform_comp, Animation_Component& animation_comp);

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