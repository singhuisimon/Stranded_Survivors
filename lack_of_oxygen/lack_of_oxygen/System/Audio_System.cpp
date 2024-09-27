#include "Audio_System.h"

namespace lof {
	void Audio_System::init() {
		studio_system = Audio_Manager::get_instance().get_studio_system();
		if (!studio_system) {
			std::cerr << "Failed to initialize FOMD Studio System." << std::endl;
		}
		else {
			std::cout << "FMOD Studio System initialized successfully." << std::endl;
		}
	}

	void Audio_System::update(float delta_time) {
		for (Entity entity : entities_with_audio) {
			//check if entity has an audio component
			Audio_Component* audio_comp = get_audio_component(entity);
			if (audio_comp && audio_comp->event_instance) {

				studio_system->update();

				audio_comp->event_instance->setVolume(audio_comp->volume);

				if (audio_comp->is_looping) {
					FMOD::Studio::EventDescription* event_desc = nullptr;
					audio_comp->event_instance->getDescription(&event_desc);
					if (event_desc) {
						bool looping = false;
						event_desc->isOneshot(&looping);
						if (!looping) {
							audio_comp->event_instance->start(); //restart if not looping
						}
					}
				}
			}
		}

		//Update FMOD system
		Audio_Manager::get_instance().update();
	}

	void Audio_System::play_sound(Audio_Component& audio_comp) {
		if (audio_comp.event_instance == nullptr) {
			FMOD::Studio::EventDescription* event_description = nullptr;
			studio_system->getEvent(audio_comp.event_path.c_str(), &event_description);

			if (event_description) {
				event_description->createInstance(&audio_comp.event_instance);
				audio_comp.event_instance->setVolume(audio_comp.volume);
				audio_comp.event_instance->start();
				std::cout << "Playing sound: " << audio_comp.event_path << std::endl;
			}
			else {
				std::cerr << "Failed to load sound event: " << audio_comp.event_path << std::endl;
			}
		}
	}

	void Audio_System::stop_sound(Audio_Component& audio_comp) {
		if (audio_comp.event_instance) {
			audio_comp.event_instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
			audio_comp.event_instance->release();
			audio_comp.event_instance = nullptr;
			std::cout << "Sound stopped: " << audio_comp.event_path << std::endl;
		}
	}

	void Audio_System::register_entity(Entity entity) {
		entities_with_audio.push_back(entity);
		std::cout << "Entity with audio registered." << std::endl;
	}

	void Audio_System::shut_down() {
		for (Entity entity : entities_with_audio) {
			Audio_Component* audio_comp = get_audio_component(entity);
			if (audio_comp && audio_comp->event_instance) {
				audio_comp->event_instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
				audio_comp->event_instance->release();
			}
		}
		Audio_Manager::get_instance().shut_down();
		std::cout << "Audio system shut down." << std::endl;
	}
}