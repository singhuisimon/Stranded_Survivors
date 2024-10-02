/**
 * @file ComponentParser.cpp
 * @brief Declares the ComponentParser utility class methods.
 * @author Simon Chan
 * @date October 1, 2024
 * Copyright (C) 20xx DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */
#pragma once
#ifndef COMPONENT_PARSER_H
#define COMPONENT_PARSER_H

// Include Managers
#include "../Manager/ECS_Manager.h"
#include "../Manager/Log_Manager.h"

// Include rapidjson for JSON parsing
#include "rapidjson/document.h"

namespace lof {

    class Component_Parser {
    public:
        /**
         * @brief Parse components from JSON and add them to an entity.
         * @param ecs_manager Reference to the ECS_Manager.
         * @param entity The entity ID.
         * @param components The JSON value containing component data.
         */
        static void add_components_from_json(ECS_Manager& ecs_manager, EntityID entity, const rapidjson::Value& components);
    };

} // namespace lof

#endif // COMPONENT_PARSER_H
