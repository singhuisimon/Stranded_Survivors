/**
 * @file Audio_Manager.cpp
 * @brief 
 * @author Amanda Leow Boon Suan
 * @date September 27, 2024
 */

#include "../Manager/Audio_Manager.h"

namespace lof {

	Audio_Manager& Audio_Manager::get_instance() {
		static Audio_Manager instance;
		return instance;
	}

	Audio_Manager::Audio_Manager() : audio_system(nullptr), track1(nullptr), track2(nullptr), current_channel(nullptr) {
		audio_system = new Audio_System();
	}

	Audio_Manager::~Audio_Manager() {
		shutdown();
		delete audio_system;
		audio_system = nullptr;
	}

	int Audio_Manager::start_up() {
		if (is_started()) {
			return 0; // Already started
		}

		if (!audio_system->initialize()) {
			return -1;
		}
		m_is_started = true;
		return 0;
	}

	void Audio_Manager::update() {
		audio_system->update();
	}

	void Audio_Manager::shutdown() {
		if (!is_started()) {
			return; // Not started
		}

		if (track1) {
			track1->release();
			track1 = nullptr;
		}
		if (track2) {
			track2->release();
			track2 = nullptr;
		}
		audio_system->shutdown();

		LM.write_log("successfully shutdown audio");

		m_is_started = false;
	}

	void Audio_Manager::load_bgm(const std::string& track1_path, const std::string& track2_path) {
		track1 = audio_system->load_sound(track1_path);
		if (!track1) {
			LM.write_log("fail to load track1");
			return;
		}
		else {
			LM.write_log("successful in loading the track1");
		}
		track2 = audio_system->load_sound(track2_path);
		if (!track2) {
			LM.write_log("fail to load track2");
			return;
		}
		else {
			LM.write_log("successful in loading the track2");
		}
		
	}

	void Audio_Manager::play_bgm(int track_number) {
		if (current_channel) {
			current_channel->stop();
		}

		if (track_number == 1 && track1) {
			current_channel = audio_system->play_sound(track1, true);

		}
		else if (track_number == 2 && track2) {
			current_channel = audio_system->play_sound(track2, true);
		}
		else {
			std::cerr << "invalid number of track/ track not loaded" << std::endl;
		}
	}

	void Audio_Manager::stop_bgm() {
		if (current_channel) {
			current_channel->stop();
			current_channel = nullptr;
			LM.write_log("successfully stop bgm");
		}
	}

	//Audio_Manager& Audio_Manager::get_instance() {
	//	static Audio_Manager instance;
	//	return instance;
	//}

	//Audio_Manager::Audio_Manager(){
	//}

	//Audio_Manager::~Audio_Manager() {
	//	shut_down();
	//}

	//int Audio_Manager::start_up() {
	//	if (is_started()) {
	//		return 0;
	//	}

	//	if (audio_system->initialize()) {
	//		m_is_started = true;
	//		return 0;
	//	}

	//	return -1;
	//}

	//bool Audio_Manager::load_audio_bank(const std::string& bank_path) {
	//	FMOD::Studio::Bank* bank;
	//	if (audio_system->load_bank(bank_path, &bank)) {
	//		bank_map[bank_path] = bank;
	//		return true;
	//	}
	//	return false;
	//}

	//bool Audio_Manager::load_audio_event(const std::string& event_name, const std::string& event_path) {
	//	FMOD::Studio::EventDescription* event_desc = audio_system->get_event_description(event_path);
	//	if (event_desc) {
	//		FMOD::Studio::EventInstance* event_instance = audio_system->create_event_instance(event_desc);
	//		if (event_instance) {
	//			event_map[event_name] = event_instance;
	//			return true;
	//		}
	//	}
	//	return false;
	//}

	//bool Audio_Manager::load_core_sound(const std::string& sound_name, const std::string& sound_path, bool is_3d, bool is_looping, bool is_stream) {
	//	FMOD::Sound* sound = audio_system->load_sound(sound_path, is_3d, is_looping, is_stream);
	//	if (sound) {
	//		core_sound_map[sound_name] = sound;
	//		return true;
	//	}
	//	return false;
	//}

	//void Audio_Manager::play_sound(const std::string& event_name) {
	//	auto it = event_map.find(event_name);
	//	if (it != event_map.end()) {
	//		it->second->start();
	//	}
	//}

	//void Audio_Manager::play_core_sound(const std::string& sound_name) {
	//	auto it = core_sound_map.find(sound_name);
	//	if (it != core_sound_map.end()) {
	//		FMOD::Channel* channel = nullptr;
	//		audio_system->play_core_sound(it->second, &channel);
	//	}
	//}

	//void Audio_Manager::stop_sound(const std::string& event_name) {
	//	auto it = event_map.find(event_name);
	//	if (it != event_map.end()) {
	//		it->second->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
	//	}
	//}

	//void Audio_Manager::stop_core_sound(const std::string& sound_name) {
	//	auto it = core_sound_map.find(sound_name);
	//	if (it != core_sound_map.end()) {
	//		FMOD::Channel* channel = nullptr;
	//		channel->stop();
	//	}
	//}

	//void Audio_Manager::play_bgmusic(const std::string& bgm_event_name) {
	//	play_core_sound(bgm_event_name);
	//}

	//void Audio_Manager::stop_bgmusic(const std::string& bgm_event_name) {
	//	stop_core_sound(bgm_event_name);
	//}

	//void Audio_Manager::attach_audio_component(Entity& entity, const std::string& event_path, bool is_loop, float volume) {
	//	Audio_Component audio_comp;
	//	audio_comp.event_path = event_path;
	//	audio_comp.is_looping = is_loop;
	//	audio_comp.volume = volume;

	//	load_audio_event(event_path, event_path);
	//	audio_comp.event_instance = event_map[event_path];
	//	entity_audio_map[entity.get_id()] = audio_comp;
	//}

	////play the sound attached to the entity's audio component
	//void Audio_Manager::play_entity_sound(Entity& entity) {
	//	auto it = entity_audio_map.find(entity.get_id());
	//	if (it != entity_audio_map.end()) {
	//		Audio_Component& audio_comp = it->second;
	//		audio_comp.event_instance->setVolume(audio_comp.volume);
	//		audio_comp.event_instance->start();
	//	}
	//}

	//// stop the sound attached to the entity's audio component
	//void Audio_Manager::stop_entity_sound(Entity& entity) {
	//	auto it = entity_audio_map.find(entity.get_id());
	//	if (it != entity_audio_map.end()) {
	//		it->second.event_instance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
	//	}
	//}

	////! update the audio system
	//void Audio_Manager::update() {
	//	audio_system->update();
	//}

	////! shut down audio system
	//void Audio_Manager::shut_down() {
	//	for (auto& pair : bank_map) {
	//		pair.second->unload();
	//	}
	//	bank_map.clear();
	//	for (auto& pair : event_map) {
	//		pair.second->release();
	//	}
	//	event_map.clear();
	//	for (auto& pair : core_sound_map) {
	//		pair.second->release();
	//	}
	//	core_sound_map.clear();
	//	audio_system->shut_down();
	//}
}

//namespace lof {
//
//	Audio_Manager::Audio_Manager() : psound(nullptr), studio_system(nullptr), bgm_music_instance(nullptr) {}
//
//	//get the singleton instance of Audio_Manager
//	Audio_Manager& Audio_Manager::get_instance() {
//		static Audio_Manager instance;
//		return instance;
//	}
//
//	//initialize FMOD studio system
//	int Audio_Manager::start_up() {
//		FMOD_RESULT result = FMOD::Studio::System::create(&studio_system);
//		if (result != FMOD_OK) {
//			LM.write_log("FMOD Studio System creation failed:", FMOD_ErrorString(result));
//			return -1;
//		}
//		
//		//current avail audio channel is set to 256 for now
//		result = studio_system->initialize(32, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr);
//
//		if (result != FMOD_OK) {
//			LM.write_log("Failed to intiialize FMOD", FMOD_ErrorString(result));
//			return -1;
//		}
//
//		create_sound("C:\\Users\\Admin\\source\\repos\\Stranded_Survivors\\lack_of_oxygen\\lack_of_oxygen\\Data\\TapTapHeroes_Level 3&4_BGM_Linear_85-BPM.bank");
//
//		m_is_started = true;
//		LM.write_log("Audio_Manager started successfully");
//		return 0;
//	}
//
//	void Audio_Manager::create_sound(const std::string& audio_path) {
//		//FMOD_RESULT result = studio_system->loadBankFile(audio_path.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &bank);
//
//		FMOD_RESULT result = studio_system->
//
//		if (result != FMOD_OK) {
//			std::cerr << " Failed to load audio bank from bank" << audio_path << ": " << FMOD_ErrorString(result) << std::endl;
//			return;
//		}
//	}
//
//	//play background music using an FMOD event path
//	void Audio_Manager::play_bgmmusic(const std::string& bgm_eventpath) {
//		
//		//stop any previous background music
//		if (bgm_music_instance != nullptr) {
//			stop_bgmmusic();
//		}
//
//		FMOD::Studio::EventDescription* event_description = nullptr;
//		studio_system->getEvent(bgm_eventpath.c_str(), &event_description);
//
//		if (event_description) {
//			//create instance for background music event
//			event_description->createInstance(&bgm_music_instance);
//			bgm_music_instance->start();
//			std::cout << "Playing background music: " << bgm_eventpath << std::endl;
//		}
//		else {
//			std::cerr << "Failed to load background music: playbgm " << bgm_eventpath << std::endl;
//		}
//	}
//
//	//stop playing the current background music
//	void Audio_Manager::stop_bgmmusic() {
//		if (bgm_music_instance) {
//			bgm_music_instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
//			bgm_music_instance->release(); //release the event instance
//			bgm_music_instance = nullptr;
//			std::cout << "Background music stopped.\n";
//		}
//	}
//
//	//Grant direct access to FMOD system without requiring all operations through Audio_Manager
//	//essentially Studio::System::getCoreSystem (both works roughly the same just how we want approach the data)
//	FMOD::Studio::System* Audio_Manager::get_studio_system() {
//		return studio_system;
//	}
//
//	//update FMOD to keep it synchronized
//	void Audio_Manager::update() {
//		studio_system->update();
//	}
//
//	//shutdown FMOD and clean up resources
//	void Audio_Manager::shut_down() {
//
//		//checks if the audio has started
//		if (!is_started()) {
//			return;
//		}
//
//		if (bgm_music_instance) {
//			bgm_music_instance->release();
//			bgm_music_instance = nullptr;
//		}
//
//		studio_system->release();
//		LM.write_log("Audio_Manager::shut_down()");
//		std::cout << "FMOD Studio system shut down. \n";
//		m_is_started = false;
//	}
//}