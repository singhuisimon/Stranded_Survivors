/**
 * @file Logic_System.h
 * @brief Defines the declaration for logic systems.
 * @author Simon Chan (100%)
 * @date November 07, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */
#pragma once
#ifndef LOF_LOGIC_SYSTEM_H
#define LOF_LOGIC_SYSTEM_H

#include <functional>
#include <string>
#include <memory>

#include "System.h"
#include "../Utility/Constant.h"
#include "../Entity/Entity.h"
#include "../Component/Component.h"
#include "../Component/Logic_Components.h"

#define LGS lof::Logic_System::get_instance()

namespace lof {

    class Script;

    class Logic_System : public System {
    public:

        static Logic_System& get_instance();

        /**
         * @brief Constructor for Logic_System.
         */
        Logic_System();

        ~Logic_System();

        /**
         * @brief Gets the type identifier for this system.
         * @return String identifying the system type.
         */
        std::string get_type() const override;

        /**
         * @brief Updates the logic state for all relevant entities.
         * @param delta_time Time elapsed since the last update.
         */
        void update(float delta_time) override;
        
        /**
         * @brief Updates the scripts for all relevant entities.
         * @param delta_time Time elapsed since the last update.
         */
        void update_script(float delta_time);

        /**
         * @brief Add scripts to the scripts_map.
		 * @param script_name The name of the script.
		 * @param script The script to be added.
         */
        void add_script(const std::string& script_name, std::shared_ptr<Script> script);

		void remove_script(const std::string& script_name);

		/**
		 * @brief Cleanup the scripts_map.
		 */
        void cleanup();

        /**
		 * @brief Retrieve a script from the scripts_map.
		 * @param script_name The name of the script to retrieve.
         */
        std::shared_ptr<Script> get_script(const std::string& script_name);
        
    private:

        

        /**
		 * @brief unique instance of Logic_System.
         */
        static std::unique_ptr<Logic_System> instance;

        /**
		 * @brief set the flag to create the instance of Logic_System.
         */
        static std::once_flag once_flag;

        
        std::unordered_map <std::string, std::weak_ptr<Script>> script_map; //using script name to access the scripts

    };
} // namespace lof
#endif // LOF_LOGIC_SYSTEM_H

//~Logic_System();