#pragma once
#ifndef LOF_TNT_SCRIPT_H
#define LOF_TNT_SCRIPT_H

#include <unordered_map>
#include <string>

#include "../Scripts/Script.h"
#include "../Component/Logic_Components.h"
#include "../System/Particle_System.h"

extern std::unordered_map<std::string, float> tnt_to_destroy;

namespace lof {
	class TNT_Script : public Script {
	public:
		TNT_Script();
		~TNT_Script() override;

		void register_script() override;

		void clean_up();

	private:
		const std::string script_name = "tnt_script";

		// Stores the TNTs that are activated with it's entity name and fuse time
		std::unordered_map<std::string, float> tnt_to_destroy;

		void check_tnt_explosion(Particle_System* particle_system);
		void update_tnt_animation();
		void update_tnt_audio(Physics_Component& physic_comp, Audio_Component& audio_comp);
		void update_tnt_particle();
	};
}

#endif