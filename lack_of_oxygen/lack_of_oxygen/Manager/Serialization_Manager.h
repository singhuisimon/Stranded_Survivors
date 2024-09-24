/**
 * @file Serialization_Manager.h
 * @brief Defines the Serialization_Manager class for handling serialization and configuration data.
 * @date September 22, 2024
 */

#ifndef LOF_SERIALIZATION_MANAGER_H
#define LOF_SERIALIZATION_MANAGER_H

 // Macros for accessing manager singleton instances
#define SM  lof::Serialization_Manager::get_instance()

// Include base headers
#include "Manager.h"

// Include RapidJSON headers
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

// Include standard headers
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

// Forward declare Model class
namespace lof {
    class Model;
}

namespace lof {

    // Define data structures to hold component and entity configurations
    struct ComponentConfig {
        std::string type;
        rapidjson::Value properties;

        // Default constructor
        ComponentConfig()
            : properties(rapidjson::kObjectType) {} // Initialize as an object

        // Move constructor
        ComponentConfig(ComponentConfig&& other) noexcept
            : type(std::move(other.type)), properties(std::move(other.properties)) {}

        // Move assignment operator
        ComponentConfig& operator=(ComponentConfig&& other) noexcept {
            if (this != &other) {
                type = std::move(other.type);
                properties = std::move(other.properties);
            }
            return *this;
        }

        // Delete copy constructor and copy assignment operator
        ComponentConfig(const ComponentConfig&) = delete;
        ComponentConfig& operator=(const ComponentConfig&) = delete;
    };

    struct EntityConfig {
        unsigned int id = 0; // Initialized to 0 by default
        std::string name;
        std::string model_filename;
        std::vector<ComponentConfig> components;

        // Default constructor
        EntityConfig() = default;

        // Move constructor
        EntityConfig(EntityConfig&& other) noexcept
            : id(other.id),
            name(std::move(other.name)),
            model_filename(std::move(other.model_filename)),
            components(std::move(other.components)) {}

        // Move assignment operator
        EntityConfig& operator=(EntityConfig&& other) noexcept {
            if (this != &other) {
                id = other.id;
                name = std::move(other.name);
                model_filename = std::move(other.model_filename);
                components = std::move(other.components);
            }
            return *this;
        }

        // Delete copy constructor and copy assignment operator
        EntityConfig(const EntityConfig&) = delete;
        EntityConfig& operator=(const EntityConfig&) = delete;
    };

    /**
     * @class Serialization_Manager
     * @brief Manages loading and providing access to configuration and serialization data.
     */
    class Serialization_Manager : public Manager {
    private:
        // Private constructor for singleton pattern
        Serialization_Manager();

        // Member variables
        unsigned int m_scr_width;
        unsigned int m_scr_height;
        float m_fps_display_interval;

        // Data directory path
        std::string m_data_directory;

        // RapidJSON document for general configuration
        rapidjson::Document m_document;

        // Store entity configurations
        std::vector<EntityConfig> m_entities_config;

        // Map of loaded models
        std::unordered_map<std::string, std::shared_ptr<Model>> m_models;

        // Helper methods
        bool load_config(const std::string& filename);
        bool load_mesh(const std::string& filename);
        bool load_object(const std::string& filename);

    public:
        /**
         * @brief Get the singleton instance of Serialization_Manager.
         * @return Reference to the singleton instance.
         */
        static Serialization_Manager& get_instance();

        // Delete copy constructor and assignment operator
        Serialization_Manager(const Serialization_Manager&) = delete;
        Serialization_Manager& operator=(const Serialization_Manager&) = delete;

        /**
         * @brief Start up the Serialization_Manager by loading configuration files.
         * @return 0 on success, negative value on failure.
         */
        int start_up() override;

        /**
         * @brief Shut down the Serialization_Manager.
         */
        void shut_down() override;

        /**
         * @brief Get the screen width from the configuration.
         * @return Screen width.
         */
        unsigned int get_scr_width() const;

        /**
         * @brief Get the screen height from the configuration.
         * @return Screen height.
         */
        unsigned int get_scr_height() const;

        /**
         * @brief Get the FPS display interval from the configuration.
         * @return FPS display interval.
         */
        float get_fps_display_interval() const;

        /**
         * @brief Get the loaded entity configurations.
         * @return Vector of EntityConfig.
         */
        const std::vector<EntityConfig>& get_entities_config() const;

        /**
         * @brief Get the loaded models.
         * @return Map of model names to shared_ptr<Model>.
         */
        const std::unordered_map<std::string, std::shared_ptr<Model>>& get_models() const;

        /**
         * @brief Get a specific model by name.
         * @param model_name Name of the model.
         * @return Shared pointer to the model, or nullptr if not found.
         */
        std::shared_ptr<Model> get_model(const std::string& model_name) const;
    };

} // namespace lof

#endif // LOF_SERIALIZATION_MANAGER_H
