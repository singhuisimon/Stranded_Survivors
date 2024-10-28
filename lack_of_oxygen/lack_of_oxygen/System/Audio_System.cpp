/**
 * @file Audio_System.cpp
 * @brief Define of the Audio_System class for managing audio playback using FMOD.
 * @author Amanda Leow Boon Suan (100%)
 * @date September 27, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#include "Audio_System.h"

namespace lof {

	void Audio_System::initializegroups() {
		//core_system->createChannelGroup("Master Group", &mastergroup);
		core_system->getMasterChannelGroup(&mastergroup);
		core_system->createChannelGroup("BGM Group", &bgmgroup);
		core_system->createChannelGroup("SFX Group", &sfxgroup);

		mastergroup->addGroup(bgmgroup);
		mastergroup->addGroup(sfxgroup);
	}

	bool Audio_System::is_sound_loaded(const std::string& sound_path) const {
		return sounds.find(sound_path) != sounds.end();
	}

	Audio_System::Audio_System() : core_system(nullptr), studio_system(nullptr), ecs_manager(ecs_manager) {}
	Audio_System::Audio_System(ECS_Manager& ecs_manager) : ecs_manager(ecs_manager), core_system(nullptr), studio_system(nullptr) {}
	Audio_System::~Audio_System() {
		shutdown();
	}

	int Audio_System::errorcheck(std::string function_name, std::string function_purpose, FMOD_RESULT result) {
		if (result != FMOD_OK) {
			LM.write_log("%s: failure to %s. FMOD Error: %s.", function_name, function_purpose, FMOD_ErrorString(result));
			return -1;
		}
		LM.write_log("%s: %s successfully executed.", function_name, function_purpose);
		return 0;
	}

	bool Audio_System::initialize() {
		if (errorcheck("Audio_System", "create studio", FMOD::Studio::System::create(&studio_system)) != 0) {
			return false;
		}
		if (errorcheck("Audio_System", "initialize studio", studio_system->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr)) != 0) {
			return false;
		}
		if (errorcheck("Audio_System", "get system", studio_system->getCoreSystem(&core_system)) != 0) {
			return false;
		}
		/*if (errorcheck("Audio_System", "set 3D setting", core_system->set3DSettings()) != 0) {
			return false;
		}*/
		/*if (errorcheck("Audio_System", "initialize system", core_system->init(512, FMOD_INIT_NORMAL, 0)) != 0) {
			return false;
		}*/

		initializegroups();
		return true;
	}
	void Audio_System::update(float delta_time) {
		core_system->update();
		studio_system->update();
	}

	void Audio_System::shutdown() {
		if (core_system) {
			core_system->close();
			core_system->release();
			core_system = nullptr;
		}
		if (studio_system) {
			studio_system->release();
			studio_system = nullptr;
		}
	}

	void Audio_System::loadsound(const std::string filepath, FMOD::Sound*& sound) {
		if (!is_sound_loaded(filepath)) {
			if (errorcheck("Audio_System::loadsound", "load sound", core_system->createSound(filepath.c_str(), FMOD_DEFAULT, nullptr, &sound)) != 0) {
				sound = nullptr;
			}

		}
	}

	void Audio_System::loadbank(const std::string filepath, FMOD::Studio::Bank*& bank) {
		errorcheck("Audio_System::loadbank", "load bank", studio_system->loadBankFile(filepath.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &bank));
	}

	void Audio_System::loadevent(const std::string& event_name, FMOD::Studio::EventInstance*& instance) {
		FMOD::Studio::EventDescription* event_desc = get_event_description(event_name);
		if (event_desc) {
			errorcheck("Audio_System::loadevent", "load event", event_desc->createInstance(&instance));
			if (instance) {
				event_instances[event_name] = instance;
			}
		}
		
	}
	
	void Audio_System::play_sound(const std::string& sound_id, float volume, bool is_looping, FMOD::Channel*& channel, FMOD::ChannelGroup*& channelgrp ) {
		FMOD::Sound* sound = sounds[sound_id];
		if (sound) {
			if (errorcheck("Audio_System::play_sound", "play sound", core_system->playSound(sound, channelgrp, false, &channel)) != 0) {
				return;
			}
			channel->setVolume(volume);
			if (is_looping) {
				channel->setMode(FMOD_LOOP_NORMAL);
				channel->setLoopCount(-1); //infinite loop

			}
		}
		else {
			LM.write_log("Audio_System::play_sound failed to play. Error: Sound not found %s.", sound_id.c_str());
		}
		LM.write_log("Audio_System::play_sound successfully executed.");
	}

	void Audio_System::stop_sound(FMOD::Channel* channel) {
		if (channel) {
			errorcheck("Audio_System::stop_sound", "stop sound", channel->stop());
		}
		LM.write_log("Audio_System::stop_sound failed to execute due to invalid channel.");
	}

	void Audio_System::pause_sound(FMOD::Channel* channel) {
		if (channel) {
			errorcheck("Audio_System::pause_sound", "pause sound", channel->setPaused(true));
		}
		LM.write_log("Audio_System::paused_sound failed to execute due to invalid channel.");
	}

	void Audio_System::resume_sound(FMOD::Channel* channel) {
		if (channel) {
			errorcheck("Audio_System::resume_sound", "resume sound", channel->setPaused(false));
		}
		LM.write_log("Audio_System::resume_sound failed to execute due to invalid channel.");
	}

	void Audio_System::play_event(FMOD::Studio::EventInstance* instance) {
		if (instance) {
			errorcheck("Audio_System::play_event", "play event", instance->start());
		}
		LM.write_log("Audio_System::play_event failed to execute due to invalid instance.");
	}

	void Audio_System::stop_event(FMOD::Studio::EventInstance* instance) {
		if (instance) {
			errorcheck("Audio_System::stop_event", "stop event", instance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT));
		}
		LM.write_log("Audio_System::stop_event failed to execute due to invalid instance.");
	}

	void Audio_System::pause_event(FMOD::Studio::EventInstance* instance) {
		if (instance) {
			errorcheck("Audio_System::pause_event", "pause event", instance->setPaused(true));
		}
		LM.write_log("Audio_System::pause_event failed to execute due to invalid instance.");
	}

	void Audio_System::resume_event(FMOD::Studio::EventInstance* instance) {
		if (instance) {
			errorcheck("Audio_System::resume_event", "resume event", instance->setPaused(false));
		}
		LM.write_log("Audio_System::resume_event failed to execute due to invalid instance.");
	}
	
	void Audio_System::pause_bgm_group() {
		errorcheck("Audio_System::pause_bgm_group","pause group",bgmgroup->setPaused(true));
	}
	void Audio_System::resume_bgm_group() {
		errorcheck("Audio_System::resume_bgm_group", "resume group", bgmgroup->setPaused(false));
	}
	void Audio_System::pause_sfx_group() {
		errorcheck("Audio_System::pause_sfx_group", "pause group", sfxgroup->setPaused(true));
	}
	void Audio_System::resume_sfx_group() {
		errorcheck("Audio_System::resume_sfx_group", "resume group", sfxgroup->setPaused(false));
	}

	float Audio_System::get_bgmgroup_volume() const {
		float volume = 1.0f;
		//errorcheck("Audio_System::get_bgmgroup_volume","get bgm group volume", bgmgroup->getVolume(&volume));
		bgmgroup->getVolume(&volume);
		return volume;
	}
	void Audio_System::set_bgmgroup_volume(float volume) {
		errorcheck("Audio_System::set_bgmgroup_volume", "set bgm group volume", bgmgroup->setVolume(volume));
	}

	float Audio_System::get_sfxgroup_volume() const {
		float volume = 1.0f;
		sfxgroup->getVolume(&volume);
		return volume;
	}
	void Audio_System::set_sfxgroup_volume(float volume) {
		errorcheck("Audio_System::set_sfxgroup_volume", "set sfx group volume", sfxgroup->setVolume(volume));
	}

	void Audio_System::play_audio_component(Audio_Component& audio_component){
		FMOD::ChannelGroup* channelgrp = nullptr;
		if (audio_component.get_audio_type() == SFX) {
			channelgrp = sfxgroup;
		}
		else if (audio_component.get_audio_type() == BGM) {
			channelgrp = bgmgroup;
		}

		FMOD::Channel* channel = nullptr;

		if (audio_component.get_file_format() == WAV) {
			play_sound(audio_component.get_filename(), audio_component.get_volume(), audio_component.get_is_looping(), channel, channelgrp);
			//audio_component.set_channel(channel);
		}
		else if (audio_component.get_audio_type() == BGM) {
			FMOD::Studio::EventInstance* event_instance = nullptr;
			loadevent(audio_component.get_filename(), event_instance);
			if (event_instance) {
				play_event(event_instance);
				audio_component.add_event_instance(audio_component.get_filename(), event_instance);
			}
		}
		else {
			LM.write_log("Audio_System::play_audio_component unable to play due to unsupported file format %s.", audio_component.get_filename());
		}
	}
	
	void Audio_System::stop_audio_component(Audio_Component& audio_component){
		//int channel = audio_component.get_channel()
		if (audio_component.get_event_instance(audio_component.get_filename())) {
			stop_event(audio_component.get_event_instance(audio_component.get_filename()));
		}
		/*else if (channel) {
			stop_sound(channel);
		}*/
	}
	
	void Audio_System::set_audio_component_volume(Audio_Component& audio_component, float volume) {}

	void Audio_System::add_event_description(const std::string& event_id, FMOD::Studio::EventDescription* event_description) {}
	void Audio_System::remove_event_description(const std::string& event_id) {}
	void Audio_System::clear_event_description() {}
	void Audio_System::print_event_descriptions() {} //debugging purposes.

	void Audio_System::add_sound(const std::string& sound_id, FMOD::Sound* sound){}
	void Audio_System::remove_sound(const std::string& sound_id) {}
	void Audio_System::clear_sounds() {}
	void Audio_System::print_sounds() {}	//debugging purposes.

	void Audio_System::setchannel_3D_pos(FMOD::Channel* channel, const Vec3D& pos) {
		if (channel) {
			FMOD_VECTOR fmod_pos = { pos.x, pos.y, pos.z };
			channel->set3DAttributes(&fmod_pos, nullptr);
		}
	}

	FMOD::System* Audio_System::get_core_system() const {}
	FMOD::Studio::System* Audio_System::get_studio_system() const {}

	FMOD::Studio::EventDescription* Audio_System::get_event_description(const std::string& event_id){
		FMOD::Studio::EventDescription* description = nullptr;
		errorcheck("", "", studio_system->getEvent(event_id.c_str(), &description));
		return description;
	}
	FMOD::Sound* Audio_System::get_sound(const std::stringstream& sound_id) {}

	FMOD::ChannelGroup* Audio_System::get_bgmgroup() {
		return bgmgroup;
	}
	FMOD::ChannelGroup* Audio_System::get_sfxgroup() {
		return sfxgroup;
	}
	FMOD::ChannelGroup* Audio_System::get_mastergroup() {
		return mastergroup;
	}

	std::string Audio_System::get_type() const {
		return "Audio_System";
	}

	//Audio_System::Audio_System() : core_system(nullptr), studio_system(nullptr), ecs_manager(ecs_manager), bgmgroup(nullptr), sfxgroup(nullptr), mastergroup(nullptr)
	//{
	//	LM.write_log("default audio system constructor used.");
	//}

	//Audio_System::Audio_System(ECS_Manager& ecs_manager) : core_system(nullptr), studio_system(nullptr), ecs_manager(ecs_manager), bgmgroup(nullptr), sfxgroup(nullptr), mastergroup(nullptr)
	//{
	//	LM.write_log("param constructor used in audio system.");
	//}

	//Audio_System::~Audio_System() {
	//	shutdown();
	//}
	//
	//bool Audio_System::initialize() {
	//	FMOD_RESULT result;

	//	result = FMOD::Studio::System::create(&studio_system);

	//	//check if the creatoin of the studio system is successful
	//	if (result != FMOD_OK) {
	//		LM.write_log("Fail to create studio system. Error: ", FMOD_ErrorString(result));
	//		return false;
	//	}

	//	result = studio_system->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr);

	//	//check if the initialization of the studio system is successful
	//	if (result != FMOD_OK) {
	//		LM.write_log("Fail to initialize studio system. Error: ", FMOD_ErrorString(result));
	//		return false;
	//	}

	//	result = studio_system->getCoreSystem(&core_system);

	//	//check if creation of the core system is successful
	//	if (result != FMOD_OK) {
	//		LM.write_log("Audio_System::initialize() Fail to create core system. Error: ");
	//		LM.write_log(FMOD_ErrorString(result));
	//		return false;
	//	}

	//	result = core_system->init(512, FMOD_INIT_NORMAL, 0);

	//	//check if the initialization of the core system is successful
	//	if (result != FMOD_OK) {
	//		LM.write_log("Audio_System::initialize(): Fail to initalize core system. Error:");
	//		LM.write_log(FMOD_ErrorString(result));
	//		return false;
	//	}

	//	

	//	LM.write_log( "FMOD core and studio system successfully initialize.");

	//	initializegroups();

	//	return true;
	//}


}

	