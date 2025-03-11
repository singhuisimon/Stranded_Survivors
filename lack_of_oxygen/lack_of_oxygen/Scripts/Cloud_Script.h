#pragma once
#ifndef LOF_CLOUD_SCRIPT_H
#define LOF_CLOUD_SCRIPT_H

#include "../Scripts/Script.h"
#include "../Component/Component.h"
#include "../System/Particle_System.h"


// 64 pixels/sec (Speed)

namespace lof {
	class Cloud_Script : public Script, public std::enable_shared_from_this<Cloud_Script> {
	public:
		Cloud_Script();

		void register_script() override;

		std::string get_type() const override;

	private:
		const std::string script_name = "cloud_script";
		float moving_speed;

		/**
		 * @brief Controls the cloud movement in the foreground
		 */
		void cloud_movement_update() const;

	};
}

#endif 