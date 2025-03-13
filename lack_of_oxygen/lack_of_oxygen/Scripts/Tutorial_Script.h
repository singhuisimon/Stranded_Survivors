#pragma once
#ifndef LOF_TUTORIAL_SCRIPT_H
#define LOF_TUTORIAL_SCRIPT_H

#include "../Scripts/Script.h"
#include "../Component/Component.h"

namespace lof {
	class Tutorial_Script : public Script, public std::enable_shared_from_this<Tutorial_Script> {
	public:
		Tutorial_Script() = default;

		void register_script() override;

		std::string get_type() const override;

		void check_keys();

		bool is_key_just_pressed(int key);
		bool is_key_just_released(int key);

	private:

		std::unordered_map<std::string, bool> tutorial_button_playing;
		std::unordered_map<int, bool> key_previous_state;


		int tutorial_page = 1;

		const std::string script_name = "tutorial_script";

		bool key_a_pressed = false;
		bool key_d_pressed = false;
		bool key_e_pressed = false;
		bool key_a_last_frame = false;
		bool key_d_last_frame = false;
		bool key_e_last_frame = false;
		bool key_backslash_pressed = false;
		bool key_backslash_last_frame = false;

		bool transitioning = false;

		float page_transition_cooldown = 0.0f;
		int prev_tut_page = 1;

		void set_tutorial_page(int new_page_num);
		bool get_transitioning();
		void update_button_visibility(EntityID entity_id, Graphics_Component& graphic_comp);

		void check_pressing_button(EntityID entity_id);
		void transit_next_scene();

	};
}

#endif 