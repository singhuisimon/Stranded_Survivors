#include "Audio_System.h"

namespace lof {

	Audio_System::Audio_System() : core_system(nullptr) {}
	Audio_System::~Audio_System() {
		shutdown();
	}

	bool Audio_System::initialize() {

		FMOD_RESULT result;
		result = FMOD::System_Create(&core_system);
		if (result != FMOD_OK) {
			LM.write_log("Fail to create core system. Error: ");
			LM.write_log(FMOD_ErrorString(result));
			return false;
		}
		result = core_system->init(512, FMOD_INIT_NORMAL, 0);
		if (result != FMOD_OK) {
			LM.write_log("Fail to initalize system");
			return false;
		}

		LM.write_log("FMOD system successfully initialize");

		return true;
	}

	void Audio_System::update() {
		core_system->update();
	}

	FMOD::Sound* Audio_System::load_sound(const std::string& file_path) {
		FMOD::Sound* sound = nullptr;
		FMOD_MODE mode = FMOD_DEFAULT;
		FMOD_RESULT result = core_system->createSound(file_path.c_str(), mode, 0, &sound);
		
		if (result != FMOD_OK) {
			LM.write_log("failed to create sound");
			return nullptr;
		}

		LM.write_log("successfully loaded the sound from system");
		return sound;
	}

	FMOD::Channel* Audio_System::play_sound(FMOD::Sound* sound, bool loop) {
		FMOD::Channel* channel = nullptr;
		if (loop) {
			sound->setMode(FMOD_LOOP_NORMAL);
		}
		else {
			sound->setMode(FMOD_LOOP_OFF);
		}
		
		FMOD_RESULT result = core_system->playSound(sound, nullptr, false, &channel);

		if (result != FMOD_OK) {
			LM.write_log("failed to play sound");
			LM.write_log(FMOD_ErrorString(result));
			return nullptr;
		}
		return channel;
	}

	void Audio_System::stop_all_sounds() {
		core_system->close();
	}

	void Audio_System::shutdown() {
		if (core_system) {
			core_system->release();
			LM.write_log("Audio_System successfully released");
		}

		LM.write_log("Audio_system failed to release");
	}

}

	/*Audio_System& Audio_System::get_instance() {
		static Audio_System instance;
		return instance;
	}*/

//	Audio_System::Audio_System() : studio_system(nullptr), core_system(nullptr) {}
//
//	Audio_System::~Audio_System() {
//		shut_down();
//	}
//
//	bool Audio_System::initialize() {
//		FMOD::Debug_Initialize(FMOD_DEBUG_DISPLAY_THREAD);
//		FMOD_RESULT result;
//
//		result = FMOD::Studio::System::create(&studio_system);
//		if (result != FMOD_OK) {
//			std::cout << "Failed to create FMOD Studio. " << FMOD_ErrorString(result) << std::endl;
//			LM.write_log("Failed to create FMOD Studio system", result);
//			return false;
//		}
//
//		result = studio_system->getCoreSystem(&core_system);
//		if (result != FMOD_OK) {
//			LM.write_log("Failed to get FMOD Core System.");
//			return false;
//		}
//
//		result = studio_system->initialize(32, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr);
//		if (result != FMOD_OK) {
//			LM.write_log("Failed to initialize FMOD Studio System", result);
//			return false;
//		}
//
//		return true;
//	}
//
//	bool Audio_System::load_bank(const std::string& bank_path, FMOD::Studio::Bank** bank) {
//		FMOD_RESULT result = studio_system->loadBankFile(bank_path.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, bank);
//		if (result != FMOD_OK) {
//			LM.write_log("Failed to load bank: ", bank_path, result);
//			return false;
//		}
//		return true;
//	}
//
//	FMOD::Sound* Audio_System::load_sound(const std::string& sound_path, bool is_3d, bool is_looping, bool is_stream) {
//		FMOD_MODE mode = FMOD_DEFAULT;
//		if (is_3d) mode |= FMOD_3D;
//		if (is_looping) mode |= FMOD_LOOP_NORMAL;
//		if (is_stream) mode |= FMOD_CREATESTREAM;
//
//		FMOD::Sound* sound = nullptr;
//		FMOD_RESULT result = core_system->createSound(sound_path.c_str(), mode, nullptr, &sound);
//		if (result != FMOD_OK) {
//			LM.write_log("Failed to load sound: ", sound_path, result);
//			return nullptr;
//		}
//		return sound;
//	}
//
//	void Audio_System::play_core_sound(FMOD::Sound* sound, FMOD::Channel** channel) {
//		if (!sound) {
//			LM.write_log("Cannot play null sound");
//			return;
//		}
//
//		FMOD_RESULT result = core_system->playSound(sound, nullptr, false, channel);
//		if (result != FMOD_OK) {
//			LM.write_log("Failed to play core sound.", result);
//		}
//	}
//
//	FMOD::Studio::EventDescription* Audio_System::get_event_description(const std::string& event_path) {
//		FMOD::Studio::EventDescription* event_desc = nullptr;
//		FMOD_RESULT result = studio_system->getEvent(event_path.c_str(), &event_desc);
//		if (result != FMOD_OK) {
//			LM.write_log("Failed to get event description for: ", event_path, result);
//			return nullptr;
//		}
//		return event_desc;
//	}
//
//	FMOD::Studio::EventInstance* Audio_System::create_event_instance(FMOD::Studio::EventDescription* event_desc) {
//		if (!event_desc) {
//			LM.write_log("Cannot create event instance from a null event description.");
//			return nullptr;
//		}
//
//		FMOD::Studio::EventInstance* instance = nullptr;
//		FMOD_RESULT result = event_desc->createInstance(&instance);
//		if (result != FMOD_OK) {
//			LM.write_log("Failed to create event instance", result);
//			return nullptr;
//		}
//		return instance;
//	}
//
//	void Audio_System::update() {
//		studio_system->update();
//	}
//
//	void Audio_System::shut_down() {
//		if (is_shut_down) {
//			LM.write_log("Audio_System is already shut down.");
//			return;
//		}
//
//		if (studio_system != nullptr) {
//			studio_system->release();
//			studio_system = nullptr;
//			core_system = nullptr;
//			LM.write_log("Audio_System shut down successfully");
//		}
//
//		is_shut_down = true;
//	}
//}


//namespace lof {
//	void Audio_System::init() {
//		studio_system = Audio_Manager::get_instance().get_studio_system();
//		if (!studio_system) {
//			std::cerr << "Failed to initialize FOMD Studio System." << std::endl;
//		}
//		else {
//			std::cout << "FMOD Studio System initialized successfully." << std::endl;
//		}
//	}
//
//	void Audio_System::update() {
//		for (Entity entity : entities_with_audio) {
//			//check if entity has an audio component
//			Audio_Component* audio_comp = get_audio_component(entity);
//			if (audio_comp && audio_comp->event_instance) {
//
//				studio_system->update();
//
//				audio_comp->event_instance->setVolume(audio_comp->volume);
//
//				if (audio_comp->is_looping) {
//					FMOD::Studio::EventDescription* event_desc = nullptr;
//					audio_comp->event_instance->getDescription(&event_desc);
//					if (event_desc) {
//						bool looping = false;
//						event_desc->isOneshot(&looping);
//						if (!looping) {
//							audio_comp->event_instance->start(); //restart if not looping
//						}
//					}
//				}
//			}
//		}
//
//		//Update FMOD system
//		Audio_Manager::get_instance().update();
//	}
//
//	void Audio_System::play_sound(Audio_Component& audio_comp) {
//		if (audio_comp.event_instance == nullptr) {
//			FMOD::Studio::EventDescription* event_description = nullptr;
//			studio_system->getEvent(audio_comp.event_path.c_str(), &event_description);
//
//			if (event_description) {
//				event_description->createInstance(&audio_comp.event_instance);
//				audio_comp.event_instance->setVolume(audio_comp.volume);
//				audio_comp.event_instance->start();
//				std::cout << "Playing sound: " << audio_comp.event_path << std::endl;
//			}
//			else {
//				std::cerr << "Failed to load sound event: " << audio_comp.event_path << std::endl;
//			}
//		}
//	}
//
//	void Audio_System::stop_sound(Audio_Component& audio_comp) {
//		if (audio_comp.event_instance) {
//			audio_comp.event_instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
//			audio_comp.event_instance->release();
//			audio_comp.event_instance = nullptr;
//			std::cout << "Sound stopped: " << audio_comp.event_path << std::endl;
//		}
//	}
//
//	void Audio_System::add_entity(EntityID entity) {
//		entities_with_audio.push_back(entity);
//	}
//
//	void Audio_System::register_entity(Entity entity) {
//		entities_with_audio.push_back(entity);
//		std::cout << "Entity with audio registered." << std::endl;
//	}
//
//	void Audio_System::shut_down() {
//		for (Entity entity : entities_with_audio) {
//			Audio_Component* audio_comp = get_audio_component(entity);
//			if (audio_comp && audio_comp->event_instance) {
//				audio_comp->event_instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
//				audio_comp->event_instance->release();
//			}
//		}
//		Audio_Manager::get_instance().shut_down();
//		std::cout << "Audio system shut down." << std::endl;
//	}
//}