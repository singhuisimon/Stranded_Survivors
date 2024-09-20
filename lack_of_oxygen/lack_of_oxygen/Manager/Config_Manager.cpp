// Include base headers
#include "Config_Manager.h"

// Include standard headers
#include <fstream>
#include <sstream>

// Jason file headers
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace lof {

    Config_Manager::Config_Manager() {
        set_type("Config_Manager");
    }

    Config_Manager& Config_Manager::get_instance() {
        static Config_Manager instance;
        return instance;
    }

    int Config_Manager::start_up() {
        m_is_started = true;
        return 0;
    }

    void Config_Manager::shut_down() {
        m_config_values.clear();
        m_is_started = false;
    }

    bool Config_Manager::load_game_config(const std::string& filename) {
        std::ifstream infile(filename);
        if (!infile.is_open()) {
            // Handle error
            return false;
        }

        std::string line;
        while (std::getline(infile, line)) {
            // Trim whitespace
            line.erase(0, line.find_first_not_of(" \t"));
            // Ignore comments and empty lines
            if (line.empty() || line[0] == '#' || line[0] == ';') {
                continue;
            }
            // Find the delimiter
            size_t delimiter_pos = line.find('=');
            if (delimiter_pos != std::string::npos) {
                // Extract key and value
                std::string key = line.substr(0, delimiter_pos);
                std::string value = line.substr(delimiter_pos + 1);
                // Trim whitespace from key and value
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                // Store in map
                m_config_values[key] = value;
            }
        }

        infile.close();
        return true;
    }

    template<typename T>
    T Config_Manager::get_value(const std::string& key, const T& default_value) const {
        auto it = m_config_values.find(key);
        if (it != m_config_values.end()) {
            std::istringstream iss(it->second);
            T value;
            if (iss >> value) {
                return value;
            }
        }
        return default_value;
    }

    // Explicit template instantiations
    template int Config_Manager::get_value<int>(const std::string&, const int&) const;
    template unsigned int Config_Manager::get_value<unsigned int>(const std::string&, const unsigned int&) const;
    template float Config_Manager::get_value<float>(const std::string&, const float&) const;
    template std::string Config_Manager::get_value<std::string>(const std::string&, const std::string&) const;

} // namespace lof
