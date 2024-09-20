/**
* @file Config_Manager.h
* @brief Defines the Config_Manager class for loading and accessing configuration data from a file.
* @author Simon Chan
* @date September 21, 2024
*/

#pragma once

#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include "Manager.h"

// Include standard headers
#include <string>
#include <unordered_map>

namespace lof {

#define CM lof::Config_Manager::get_instance()

    /**
     * @class Config_Manager
     * @brief Manages loading and accessing configuration settings from a file.
     *
     * The Config_Manager is implemented as a singleton and is responsible for loading 
     * configuration settings from a file at startup. It provides methods to retrieve 
     * configuration values of various types, with default values if the configuration key is not found.
     */
    class Config_Manager : public Manager {
    private:
        /**
         * @brief Private constructor to prevent instantiation (singleton pattern).
         */
        Config_Manager();

        // Delete copy constructor and assignment operator to prevent copying.
        Config_Manager(Config_Manager const&) = delete;
        void operator=(Config_Manager const&) = delete;

        /**
         * @brief Map to store configuration key-value pairs.
         */
        std::unordered_map<std::string, std::string> m_config_values;

    public:
        /**
         * @brief Get the singleton instance of the Config_Manager.
         * @return Reference to the Config_Manager instance.
         */
        static Config_Manager& get_instance();

        /**
         * @brief Start up the Config_Manager.
         * @return 0 if successful, else negative number.
         */
        int start_up() override;

        /**
         * @brief Shut down the Config_Manager.
         */
        void shut_down() override;

        /**
         * @brief Load game configuration settings from a file.
         * @param filename The name of the configuration game file.
         * @return True if the configuration was loaded successfully, false otherwise.
         */
        bool load_game_config(const std::string& filename);

        /**
         * @brief Get the value associated with a configuration key.
         * @tparam T The type of the value to retrieve (e.g., int, float, std::string).
         * @param key The configuration key.
         * @param default_value The default value to return if the key is not found or conversion fails.
         * @return The value associated with the key, or the default value if the key is not found or conversion fails.
         */
        template<typename T>
        T get_value(const std::string& key, const T& default_value) const;
    };

} // namespace lof

#endif // CONFIG_MANAGER_H
