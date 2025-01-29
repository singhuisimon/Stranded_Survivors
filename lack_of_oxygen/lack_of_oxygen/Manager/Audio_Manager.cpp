#include "Audio_Manager.h"


namespace lof {
	Audio_Manager& Audio_Manager::get_instance() {
		static Audio_Manager instance;
		return instance;
	}

	Audio_Manager::Audio_Manager() : core_system(nullptr), mastergroup(nullptr), bgmgroup(nullptr), sfxgroup(nullptr), uigroup(nullptr) {
		set_type("Audio_Manager");
	}

	Audio_Manager::~Audio_Manager() { shut_down(); }

	void Audio_Manager::initializegroups() {
		if (core_system) {
			errorcheck(core_system->getMasterChannelGroup(&mastergroup), "Audio_System::initializegroups()", "initializing mastergroup");
			core_system->createChannelGroup("BGM Group", &bgmgroup);
			core_system->createChannelGroup("SFX Group", &sfxgroup);
			core_system->createChannelGroup("UI Group", &uigroup);

			mastergroup->addGroup(bgmgroup);
			mastergroup->addGroup(sfxgroup);
			mastergroup->addGroup(uigroup);
		}
	}

	int Audio_Manager::start_up() {
		if (is_started()) {
			return 0; //Already started
		}

		if (initialize()) {
			LM.write_log("successfully initialize audio manager.");
		}

		m_is_started = true;
		return 0;
	}

	int Audio_Manager::errorcheck(FMOD_RESULT result, const std::string& function_name, const std::string& function_purpose) {
		if (result != FMOD_OK) {
			LM.write_log("%s failed to %s. FMOD Error: %s.", function_name.c_str(), function_purpose.c_str(), FMOD_ErrorString(result));
			return -1;
		}
		//LM.write_log("%s successfully executed %s.", function_name.c_str(), function_purpose.c_str());
		return 0;
	}

	bool Audio_Manager::initialize() {
		//set core_system using get core_system
		for (auto& system : ECSM.get_systems()) {
			if (system->get_type() == "Audio_System") {
				auto* audio_system = static_cast<Audio_System*>(system.get());
				core_system = audio_system->get_core_system();
			}
			else {
				continue;
			}
		}

		initializegroups();

		return true;
	}

	void Audio_Manager::shut_down() {

		if (!core_system) {
			LM.write_log("Audio_Manager::shut_down: core_system not even initialize and still nullptr");
			return;
		}

		stop_mastergroup();

		//release sounds
		for (auto& [key, sound] : sound_map) {
			if (sound) {
				sound->release();
				sound = nullptr;
			}
		}

		sound_map.clear();

		if (bgmgroup) {
			bgmgroup->release();
			bgmgroup = nullptr;
		}
		if (sfxgroup) {
			sfxgroup->release();
			sfxgroup = nullptr;
		}
		if (mastergroup) {
			mastergroup->release();
			mastergroup = nullptr;
		}

		core_system = nullptr; //set it to null to avoid hanging (check again with chatgpt)

	}

	void Audio_Manager::load_sound(const std::string& file_path, const AudioType audio_type, bool is3d) {
		if (sound_map.find(file_path) != sound_map.end()) {
			return; //sound is already loaded
		}

		if (!ASM.load_audio_file(file_path)) {
			LM.write_log("Audio_System::load_sound: Failed to find audio file %s", file_path.c_str());
			return;
		}

		std::string full_path = ASM.get_audio_path(file_path);
		LM.write_log("Audio_System::load_sound: Loading sound from %s", full_path.c_str());

		FMOD::Sound* sound = nullptr;
		FMOD_MODE mode1 = (audio_type == BGM) ? FMOD_CREATESTREAM : FMOD_DEFAULT;
		FMOD_MODE mode2 = is3d ? FMOD_3D : FMOD_2D;
		if (core_system) {
			FMOD_RESULT result = core_system->createSound(full_path.c_str(), mode1 | mode2, 0, &sound);
			if (errorcheck(result, "Audio_System::load_sound", "create sound") != 0) {
				return;
			}
		}

		sound_map[file_path] = sound;
		LM.write_log("Audio_System::load_sound: Successfully loaded sound");
	}

	void Audio_Manager::unload_sound(const std::string& file_path) {
		auto it = sound_map.find(file_path);
		if (it == sound_map.end()) {
			return;
		}

		FMOD_RESULT result = it->second->release();
		if (errorcheck(result, "Audio_System::unload_sound", "release sound" + file_path) != 0) {
			return;
		}

		sound_map.erase(it);	//erase it from the map
		LM.write_log("Audio_System::unload_sound: successfully unloaded the sound %s", file_path.c_str());
	}

	FMOD::Sound* Audio_Manager::get_sound(const std::string& file_path, AudioType audio_type, bool is3d) {
		auto it = sound_map.find(file_path);
		if (it != sound_map.end()) {
			return it->second;
		}

		load_sound(file_path, audio_type, is3d);

		it = sound_map.find(file_path);
		if (it != sound_map.end()) {
			return it->second;
		}

		return nullptr;
	}

	void Audio_Manager::play_now(EntityID entity_id, const std::string& audio_key, const Audio_Component& audio_component, bool bgm) {
		std::string file_path = audio_component.get_filepath(audio_key);
		std::string channel_key = file_path + std::to_string(entity_id) + audio_key;

		for (auto& system : ECSM.get_systems()) {
			if (system->get_type() == "Audio_System") {
				auto* audio_system = static_cast<Audio_System*>(system.get());
				if (!bgm) {
					audio_system->play_sfx_sound(file_path, channel_key, audio_key, audio_component);
				}
				else {
					audio_system->play_bgm_sound(file_path, channel_key, audio_key, audio_component);
				}
				
				LM.write_log("Audio_Manager::play_now: has successfully played sound %s in entity %u", file_path.c_str(), entity_id);
			}
			else {
				LM.write_log("Audio_Manager::play_now: looping though system currently %s", system->get_type().c_str());
				continue;
			}
		}
	}

	void Audio_Manager::mute_layer(EntityID entity_id, const std::string& audio_key, const std::string& file_path) {
		//TODO IMPLEMENT A MUTE FUNCTION/FADE OUT FUNCTION INSIDE THE AUDIO SYSTEM AND USE IT TO ADJUST THE AUDIO TO MINIMUM.
		
		for (auto& system : ECSM.get_systems()) {
			if (system->get_type() == "Audio_System") {
				auto* audio_system = static_cast<Audio_System*>(system.get());
				std::string channel_key = audio_system->generate_channel_key(entity_id, file_path, audio_key);

				audio_system->set_channel_mute(channel_key, true);

			}
		}

	}

	void Audio_Manager::unmute_layer(EntityID entity_id, const std::string& audio_key, const std::string& file_path) {
		//TODO IMPLEMENT A MUTE FUNCTION/FADE OUT FUNCTION INSIDE THE AUDIO SYSTEM AND USE IT TO ADJUST THE AUDIO TO MINIMUM.

		for (auto& system : ECSM.get_systems()) {
			if (system->get_type() == "Audio_System") {
				auto* audio_system = static_cast<Audio_System*>(system.get());
				std::string channel_key = audio_system->generate_channel_key(entity_id, file_path, audio_key);

				audio_system->set_channel_mute(channel_key, false);

			}
		}

	}

	void Audio_Manager::stop_now(EntityID entity_id, const std::string& audio_key, const std::string& file_path) {
		std::string channel_key = file_path + std::to_string(entity_id) + audio_key;

		for (auto& system : ECSM.get_systems()) {
			if (system->get_type() == "Audio_System") {
				auto* audio_system = static_cast<Audio_System*>(system.get());
				audio_system->stop_sound(channel_key);
				LM.write_log("Audio_Manager::play_now: has successfully stop sound %s in enstity %u", file_path.c_str(), entity_id);
			}
			else {
				LM.write_log("Audio_Manager::play_now: looping though system currently %s", system->get_type().c_str());
			}
		}
	}

	void Audio_Manager::update_bgm_layering(const int current_scene, const int oxygen_level) {
		
		EntityID background_id = ECSM.find_entity_by_name("background");
		
		if (background_id != INVALID_ENTITY_ID && ECSM.has_component<Audio_Component>(background_id)) {

			auto& audio_background = ECSM.get_component<Audio_Component>(background_id);
			if (current_scene == 1) {
				//play_now(background_id, "bgm1", audio_background, true);
				//std::cout << "file detected for bgm1 in scene 1: " << audio_background.get_filepath("bgm1") << std::endl;
			}
			else if (current_scene == 2) {

				if (oxygen_level <= 100) {
					play_now(background_id, "bgm surface", audio_background, true);
					play_now(background_id, "bgm base_1", audio_background, true);
					play_now(background_id, "bgm base_2", audio_background, true);
					play_now(background_id, "bgm base_3", audio_background, true);
					//play_now(background_id, "bgm 80", audio_background, true);
					//play_now(background_id, "bgm 50_1", audio_background, true);
					//play_now(background_id, "bgm 50_2", audio_background, true);
					//play_now(background_id, "bgm 35", audio_background, true);
					//play_now(background_id, "bgm 25", audio_background, true);
					//play_now(background_id, "bgm 20", audio_background, true);
				}
				//else if (oxygen_level <= 80) {
				//	play_now(background_id, "bgm 80", audio_background, true);
				//}
				//else if (oxygen_level <= 50) {
				//	play_now(background_id, "bgm 50_1", audio_background, true);
				//	play_now(background_id, "bgm 50_2", audio_background, true);
				//}
				//else if (oxygen_level <= 35) {
				//	play_now(background_id, "bgm 35", audio_background, true);
				//}
				//else if (oxygen_level <= 25) {
				//	play_now(background_id, "bgm 25", audio_background, true);
				//}
				//else {
				//	play_now(background_id, "bgm 20", audio_background, true);
				//	//TODO ? THINK ABOUT THE LOGIC AGAIN WHEN TO MUTE OR PAUSE EVEN. ALSO ADD ANOTHER 
				//	//PARAMETER INSIDE PLAYNOW THAT IS DEFAULT FALSE BUT TRUE FOR BGM
				//}
			}
		}
		return;
	}

	FMOD::ChannelGroup* Audio_Manager::get_mastergroup() const {
		return mastergroup;
	}
	FMOD::ChannelGroup* Audio_Manager::get_bgmgroup() const {
		return bgmgroup;
	}
	FMOD::ChannelGroup* Audio_Manager::get_sfxgroup() const {
		return sfxgroup;
	}
	FMOD::ChannelGroup* Audio_Manager::get_uigroup() const {
		return uigroup;
	}

	void Audio_Manager::stop_mastergroup() {
		bool playing;
		mastergroup->isPlaying(&playing);
		if (playing) {
			errorcheck(mastergroup->stop(), "Audio_System::stop_mastergroup", "stop master group");
		}
	}

	void Audio_Manager::pause_resume_mastergroup() {
		//get_mastergroup;
		bool pause;
		mastergroup->getPaused(&pause);

		if (level_editor_mode && pause) {
			return;
		}

		if (pause) {
			errorcheck(mastergroup->setPaused(false), "Audio_System::pause_resume_group", "resume mastergroup");
		}
		else {
			errorcheck(mastergroup->setPaused(true), "Audio_System::pause_resume_group", "pause mastergroup");
		}
	}

	void Audio_Manager::pause_group(GroupType grouptype) {
		FMOD::ChannelGroup* selected_group = nullptr;

		switch (grouptype) {
		case TYPE_BGM:
			selected_group = bgmgroup;
			break;
		case TYPE_SFX:
			selected_group = sfxgroup;
			break;
		case TYPE_UI:
			selected_group = uigroup;
			break;
		default:
			break;
		}

		if (selected_group) {
			bool pause = false;
			selected_group->getPaused(&pause);
			if (!pause) {
				errorcheck(selected_group->setPaused(true));
			}
		}
	}

	void Audio_Manager::resume_group(GroupType grouptype) {
		FMOD::ChannelGroup* selected_group = nullptr;

		switch (grouptype) {
		case TYPE_BGM:
			selected_group = bgmgroup;
			break;
		case TYPE_SFX:
			selected_group = sfxgroup;
			break;
		case TYPE_UI:
			selected_group = uigroup;
			break;
		default:
			break;
		}

		if (selected_group) {
			bool pause = false;
			selected_group->getPaused(&pause);
			if (!pause) {
				errorcheck(selected_group->setPaused(false));
			}
		}
	}

	void Audio_Manager::set_group_volume(GroupType grouptype, float volume) {
		FMOD::ChannelGroup* selected_group = nullptr;

		switch (grouptype) {
		case TYPE_BGM:
			selected_group = bgmgroup;
			break;
		case TYPE_SFX:
			selected_group = sfxgroup;
			break;
		case TYPE_UI:
			selected_group = uigroup;
			break;
		default:
			selected_group = mastergroup;
			break;
		}

		if (selected_group != nullptr) {
			errorcheck(selected_group->setVolume(volume), "Audio_System::set_channelgroup_volume", "set channelgroup volume");
		}
	}

	float Audio_Manager::get_group_volume(GroupType grouptype) {
		FMOD::ChannelGroup* selected_group = nullptr;

		switch (grouptype) {
		case TYPE_BGM:
			selected_group = bgmgroup;
			break;
		case TYPE_SFX:
			selected_group = sfxgroup;
			break;
		case TYPE_UI:
			selected_group = uigroup;
			break;
		default:
			selected_group = mastergroup;
			break;
		}

		float volume = 0.0f;

		if (selected_group) {
			errorcheck(selected_group->getVolume(&volume), "Audio_System::set_channelgroup_volume", "set channelgroup volume");
		}

		return volume;
	}

	std::vector<std::string> Audio_Manager::get_sound_map_filename() const {
		std::vector<std::string> filenames;
		for (const auto& pair : sound_map) {
			filenames.push_back(pair.first);
		}
		return filenames;
	}
}