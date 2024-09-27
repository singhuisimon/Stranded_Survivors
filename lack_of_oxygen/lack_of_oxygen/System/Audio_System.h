#pragma once
#include <fmod.hpp>
#include <fmod_studio.hpp>
#include <vector>
#include "../Entity/Entity.h"
#include "../Manager/Audio_Manager.h"

namespace lof {

	//responsible for playing, stopping, and updating sounds associated with entities
	class Audio_System {

	public:
		//initialize audio system
		void init();

		//update all entities with audio component
		void update(float delta_time);

		//play sound for a specific entity
		void play_sound(Audio_Component& audio_comp);

		//stop sound for a specific entity
		void stop_sound(Audio_Component& audio_comp);

		//shutdown and clean up
		void shut_down();

		void register_entity(Entity entity);

	private:
		FMOD::Studio::System* studio_system;
		std::vector<Entity> entities_with_audio;	//list of entities that have audio
	};
}
