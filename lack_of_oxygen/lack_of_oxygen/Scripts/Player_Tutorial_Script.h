#pragma once
#ifndef LOF_PLAYER_TUTORIAL_SCRIPT_H
#define LOF_PLAYER_TUTORIAL_SCRIPT_H

#include "../Scripts/Script.h"
#include "../Component/Component.h"


// 64 pixels/sec (Speed)
namespace lof {
	class Player_Tutorial_Script : public Script, public std::enable_shared_from_this<Player_Tutorial_Script> {
	public:
		Player_Tutorial_Script();

		void register_script() override;

		std::string get_type() const override;

	private:
		const std::string script_name = "player_tutorial_script";
		float moving_speed;

		/**
		 * @brief Controls the cloud movement in the foreground
		 */
		void player_tutorial_update() const;

	};
}

#endif 