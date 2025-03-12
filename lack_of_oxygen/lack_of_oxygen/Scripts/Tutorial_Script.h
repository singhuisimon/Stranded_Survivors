#pragma once
#ifndef LOF_TUTORIAL_SCRIPT_H
#define LOF_TUTORIAL_SCRIPT_H

#include "../Scripts/Script.h"
#include "../Component/Component.h"

namespace lof {
	class Tutorial_Script : public Script, public std::enable_shared_from_this<Tutorial_Script> {
	public:
		Tutorial_Script();

		void register_script() override;

		std::string get_type() const override;

		void check_keys();

		bool is_key_just_pressed(int key);

		bool is_key_being_hold(int key);

		bool is_key_just_released(int key);

	private:

		std::unordered_map<std::string, bool> tutorial_sound_playing;

		int tutorial_page = 1;

		const std::string script_name = "tutorial_script";

		bool left_mouse_pressed = false;
		bool left_mouse_last_frame = false;
		bool key_a_pressed = false;
		bool key_d_pressed = false;
		bool key_e_pressed = false;
		bool key_a_last_frame = false;
		bool key_d_last_frame = false;
		bool key_e_last_frame = false;
		bool key_esc_pressed = false;
		bool key_esc_last_frame = false;

		bool transitioning = false;
		bool button_e_shown = false;
		bool button_a_shown = false;
		bool button_d_shown = false;

		float siren_audio_cooldown;
		//float current_cooldown = 0.0f;
		//float transition_cooldown = 0.5f;
		float page_transition_cooldown = 0.0f;
		//bool page_transition_active = false;

		void set_tutorial_page(int new_page_num);
		void update_button_visibility(EntityID entity_id, Graphics_Component& graphic_comp);
		void update_button(EntityID entity_id);

	};
}

#endif 