/**
 * @file Manager.cpp
 * @brief Implements the base manager class function declarations.
 * @author Simon Chan
 * @date September 15, 2024
 * Copyright (C) 20xx DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

// Include header file
#include "Manager.h"

namespace lof {

    Manager::Manager() : m_is_started(false) {}


    Manager::~Manager() {}


    void Manager::set_type(const std::string& new_type) {
        // Set Manager Type
        m_type = new_type;
    }


    std::string Manager::get_type() const {
        return m_type;
    }


    int Manager::start_up() {
        // Base implementation sets the started flag to true
        m_is_started = true;
        return 0;   // Success
    }


    void Manager::shut_down() {
        // Base implementation sets the started flag to false
        m_is_started = false;
    }


    bool Manager::is_started() const {
        // Check if the Manager is started
        return m_is_started;
    }

} // namespace lof