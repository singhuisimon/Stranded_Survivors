#pragma once
#ifndef LOF_PLAYER_SCRIPT_H
#define LOF_PLAYER_SCRIPT_H

#include "../Scripts/Script.h"
#include "../Component/Logic_Components.h"

namespace lof {

	class Player_Script : public Script {

	public:
		Player_Script();
		void register_script();

	private:

		const std::string script_name = "player_script";

		EntityID player_id;

		int forces_flag;
		
		void set_force_flag(int flag);

		int get_force_flag() const;

		void set_player_id(EntityID entityid);

		EntityID get_player_id() const;

		/*std::unordered_map<EntityID, MovementData> entity_data;
		void add_entity_data(EntityID id, const MovementData& data);
		bool get_entity_data(EntityID id, MovementData& out_data) const;
		void remove_entity_data(EntityID id);*/
		/**
		* @brief Updates linear movement for a given entity.
		* @param logic The Logic_Component containing movement parameters.
		* @param transform The Transform2D to update.
		* @param delta_time Time elapsed since the last update.
		*/
		void update_player_movement(Physics_Component& physic_comp);
		/**
		* @brief Updates circular movement for a given entity.
		* @param logic The Logic_Component containing movement parameters.
		* @param transform The Transform2D to update.
		* @param delta_time Time elapsed since the last update.
		*/
		void update_player_audio(Physics_Component& physic_comp, Audio_Component& audio_comp);

		void update_player_animation(Physics_Component& physic_comp);
	};

}


#endif // !LOF_PLAYER_SCRIPT_H