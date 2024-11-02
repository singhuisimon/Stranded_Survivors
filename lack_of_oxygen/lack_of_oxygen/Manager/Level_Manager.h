/**
 * @file ...
 * @brief ...
 * @date ...
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#ifndef LOF_LEVEL_MANAGER_H
#define LOF_LEVEL_MANAGER_H

 // Macros for accessing manager singleton instances
#define LVLM lof::Level_Manager::get_instance()

// Include base Manager class
#include "Manager.h"

// Include other managers
#include "Log_Manager.h"
#include "ECS_Manager.h"

// Include utility function
#include "../Utility/Constant.h"
#include "../Utility/Path_Helper.h"

// Include RapidJSON headers
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include <rapidjson/istreamwrapper.h>


namespace lof {

    class Level_Manager : public Manager {
    private:

        Level_Manager();
        class ECS_Manager& ecs;

    public:
        
        // Delete copy constructor and assignment operator
        Level_Manager(ECS_Manager& ecs_manager);
        Level_Manager(const Level_Manager&) = delete;
        Level_Manager& operator=(const Level_Manager&) = delete;

        static Level_Manager& get_instance();
        int start_up() override;
        void shut_down() override;
        void update();

        //function that serialises data goes here...
        //void save_to_json(Entity* entity);
        //void save_to_json(Entity* entity, const char* path);

       
    };

} // namespace lof

#endif // LOF_LEVEL_MANAGER_H
