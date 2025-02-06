#pragma once
#ifndef LOF_MINING_SCRIPT_H
#define LOF_MINING_SCRIPT_H

#include "../Scripts/Script.h"
#include "../Component/Logic_Components.h"
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

		void set_player_id(EntityID entityid);

		EntityID get_player_id() const;

		void update_mining(Audio_Component& audio_comp);

		void update_tile(int block_to_remove, Particle_System* particle_system);

		void update_mining_audio(int block_to_remove, Audio_Component& audio_comp);

		void update_mining_sparks(Particle_System* particle_system);

		void update_mining_particle(Particle_System* particle_system, Transform2D& transform_comp, Animation_Component& animation_comp);

		void update_final_mining_particle(Particle_System* particle_system, Transform2D& transform_comp, Animation_Component& animation_comp);

		/**
		 * @brief Determines the mineral value based on the block entity type
		 * @param block_id The EntityID of the block being destroyed
		 * @return The mineral value associated with that block type
		 */
		int get_mineral_value(EntityID block_id) const;

		/**
		 * @brief Updates the mineral count text in the UI
		 * @param value_to_add The value to add to the current mineral count
		 */
		void update_mineral_count_text(int value_to_add);
	};
}

#endif 