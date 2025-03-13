#pragma once
#ifndef LOF_TUTORIAL_LIGHTS_SCRIPT_H
#define LOF_TUTORIAL_LIGHTS_SCRIPT_H

#include "../Scripts/Script.h"
#include "../Component/Component.h"
#include "../System/Particle_System.h"


// 64 pixels/sec (Speed)

namespace lof {
	class Tutorial_Light_Script : public Script, public std::enable_shared_from_this<Tutorial_Light_Script> {
	public:
		//std::vector<int> frames{ 0, 1, 2, 3 };
		//std::vector<int> frameduration = { 15, 2, 2, 2 };
		int current_frame_index = 0;
		int elapsed_frames = 0;	//tracks how many frames have passed
		std::string base_texture = "Siren_Bulb_Batch_14_";

		Tutorial_Light_Script();

		void register_script() override;

		std::string get_type() const override;

		void init_light();

		void update_frame();

	private:
		const std::string script_name = "tutorial_light_script";

		void lights_animation();

		

	};
}

#endif 