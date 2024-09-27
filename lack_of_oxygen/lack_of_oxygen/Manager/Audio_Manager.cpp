#include "../Manager/Audio_Manager.h"


namespace lof {

	Audio_Manager::Audio_Manager() : studio_system(nullptr), bgm_music_instance(nullptr) {}

	//get the singleton instance of Audio_Manager
	Audio_Manager& Audio_Manager::get_instance() {
		static Audio_Manager instance;
		return instance;
	}

	//initialize FMOD studio system
	void Audio_Manager::init() {
		FMOD::Studio::System::create(&studio_system);
		//current avail audio channel is set to 256 for now
		studio_system->initialize(256, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr);
	}

	//play background music using an FMOD event path
	void Audio_Manager::play_bgmmusic(const std::string& bgm_eventpath) {
		
		//stop any previous background music
		if (bgm_music_instance != nullptr) {
			stop_bgmmusic();
		}

		FMOD::Studio::EventDescription* event_description = nullptr;
		studio_system->getEvent(bgm_eventpath.c_str(), &event_description);

		if (event_description) {
			//create instance for background music event
			event_description->createInstance(&bgm_music_instance);
			bgm_music_instance->start();
			std::cout << "Playing background music: " << bgm_eventpath << std::endl;
		}
		else {
			std::cerr << "Failed to load background music: " << bgm_eventpath << std::endl;
		}
	}

	//stop playing the current background music
	void Audio_Manager::stop_bgmmusic() {
		if (bgm_music_instance) {
			bgm_music_instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
			bgm_music_instance->release(); //release the event instance
			bgm_music_instance = nullptr;
			std::cout << "Background music stopped.\n";
		}
	}

	//Grant direct access to FMOD system without requiring all operations through Audio_Manager
	FMOD::Studio::System* Audio_Manager::get_studio_system() {
		return studio_system;
	}

	//update FMOD to keep it synchronized
	void Audio_Manager::update() {
		studio_system->update();
	}

	//shutdown FMOD and clean up resources
	void Audio_Manager::shut_down() {
		if (bgm_music_instance) {
			bgm_music_instance->release();
			bgm_music_instance = nullptr;
		}
		studio_system->release();
		std::cout << "FMOD Studio system shut down. \n";
	}
}