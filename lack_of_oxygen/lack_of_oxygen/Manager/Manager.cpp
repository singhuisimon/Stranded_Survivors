/**
 * @file Manager.cpp
 * @brief Implements the Manager class methods.
 * @author Simon Chan
 * @date September 15, 2024
 */

// Include header file
#include "Manager.h"

namespace lof {

    Manager::Manager() : m_is_started(false) {}


    Manager::~Manager() {}


    void Manager::set_type(const std::string& new_type) {
        m_type = new_type;
    }


    std::string Manager::get_type() const {
        return m_type;
    }


    int Manager::start_up() {
        m_is_started = true;
        return 0;
    }


    void Manager::shut_down() {
        m_is_started = false;
    }


    bool Manager::is_started() const {
        return m_is_started;
    }

} // namespace lof