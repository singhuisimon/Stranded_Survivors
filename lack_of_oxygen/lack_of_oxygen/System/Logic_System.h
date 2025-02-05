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
         * @brief Gets the type identifier for this system.
         * @return String identifying the system type.
         */
        std::string get_type() const override;

        /**
         * @brief Updates the logic state for all relevant entities.
         * @param delta_time Time elapsed since the last update.
         */
        void update(float delta_time) override;

        void add_script(const std::string& script_name, std::shared_ptr<Script> script);

        std::shared_ptr<Script> get_script(const std::string& script_name);
        
    private:

        /**
         * @brief Constructor for Logic_System.
         */
        Logic_System();

        static std::unique_ptr<Logic_System> instance;
        static std::once_flag once_flag;

        //using script name to access the scripts
        std::unordered_map <std::string, std::shared_ptr<Script>> script_map;
        std::vector<Logic_Component*> script_components;

    };
} // namespace lof
#endif // LOF_LOGIC_SYSTEM_H

//~Logic_System();