/**
 * @file Mini_Map_Script.h
 * @brief Define the Mini_Map_Script class
 * @author Saw Hui Shan (100%)
 * @date March 25, 2025
 * Copyright (C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */
#pragma once
#ifndef LOF_MINI_MAP_SCRIPT_H
#define LOF_MINI_MAP_SCRIPT_H

#include "../Scripts/Script.h"
#include "../Component/Component.h"

namespace lof {
	class Mini_Map_Script : public Script, public std::enable_shared_from_this<Mini_Map_Script> {
	public:
		/**
		* @brief The default constructor of the tutorial script class
		*/
		Mini_Map_Script() = default;

		/**
		* @brief Register the script and its functions
		*/
		void register_script() override;

		/**
		* @brief Get the script name
		* @return std::string containing the script name
		*/
		std::string get_type() const override;



	private:

		//the script name
		const std::string script_name = "mini_map_script";

		// to render mini_map_ui_lava
		void Mini_Map_UI_Lava();
		

	};
}

#endif 