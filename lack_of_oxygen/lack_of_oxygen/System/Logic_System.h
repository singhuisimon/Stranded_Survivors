/**
 * @file Logic_System.h
 * @brief Defines the declaration for logic systems.
 * @author Simon Chan (50%), Amanda Leow Boon Suan (50%)
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

namespace lof {

    class Script;

    class Logic_System : public System {
    public:

        /**
         * @brief Constructor for Logic_System.
         */
        Logic_System();

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
         */
        void process_script();

        /**
		* @brief Initializes the scripts for all relevant entities.
		* @param entity_id The ID of the entity to initialize the script for.
		* @param logic_data The logic data associated with the entity.
		* @param script The script to be initialized.
        */
        void initialize_script(EntityID entity_id, std::shared_ptr<Logic_Component::LogicData> logic_data, std::shared_ptr<Script> script);
        
		/**
		 * @brief Updates the script for all relevant entities.
		 * @param entity_id The ID of the entity to update the script for.
		 * @param logic_data The logic data associated with the entity.
		 * @param script The script to be updated.
		 */
        void update_script(EntityID entity_id, std::shared_ptr<Logic_Component::LogicData> logic_data, std::shared_ptr<Script> script);

		/**
		 * @brief Terminates the script for all relevant entities.
		 * @param entity_id The ID of the entity to terminate the script for.
		 * @param logic_data The logic data associated with the entity.
		 * @param script The script to be terminated.
		 */
        void terminate_script(EntityID entity_id, std::shared_ptr<Logic_Component::LogicData> logic_data, std::shared_ptr<Script> script);

    private:

    };
} // namespace lof
#endif // LOF_LOGIC_SYSTEM_H