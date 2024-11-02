/**
 * @file Component.h
 * @brief Defines the base Component class and specific component types for the Entity Component System (ECS).
 * @author Simon Chan (48.453%), Wai Lwin Thit (44.329%), Saw Hui Shan (7.216%)
 * @date September 15, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */
#pragma once
#ifndef LOF_COMPONENT_H
#define LOF_COMPONENT_H

// Include standard headers
#include <cstdint>
#include <string>
#include <memory>
#include <variant>
#include <algorithm>
#include <unordered_map>

// Include Utility headers
#include "../Utility/Vector2D.h"
#include "../Utility/Vector3D.h"
#include "../Utility/Constant.h"
#include "../Utility/Path_Helper.h"

// FOR TESTING 
#include "../Glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm-0.9.9.8/glm/glm.hpp>
#include <glm-0.9.9.8/glm/gtc/type_ptr.hpp>

namespace lof {

    /**
     * @typedef ComponentID
     * @brief Alias for the data type used to represent component identifiers.
     */
    using ComponentID = std::uint8_t;

    /**
     * @class Component
     * @brief Base class for all components in the ECS.
     */
    class Component {
    public:
        /**
         * @brief Virtual destructor for the Component class.
         */
        virtual ~Component() = default;
    };


    /**
     * @class Transform2D
     * @brief Represents the position, rotation, and scale of an entity in 2D space.
     */
    class Transform2D : public Component {
    public:
        Vec2D position;     ///< Position of the entity in world space.
        Vec2D prev_position;
        Vec2D orientation;  ///< Orientation of the entity in degrees.
        Vec2D scale;        ///< Scale of the entity.

        /**
         * @brief Default constructor initializing position, rotation, and scale.
         */
        Transform2D()
            : position(0.0f, 0.0f), prev_position(0.0f,0.0f), orientation(0.0f, 0.0f), scale(1.0f, 1.0f) {}

        /**
         * @brief Parameterized constructor.
         * @param pos Initial position.
         * @param rot Initial rotation in degrees.
         * @param scl Initial scale.
         */
        Transform2D(const Vec2D& pos, Vec2D& prev_pos, Vec2D& ori, const Vec2D& scl)
            : position(pos), prev_position(prev_pos), orientation(ori), scale(scl) {}
    };


    /**
     * @class Velocity_Component
     * @brief Component representing an entity's velocity.
     */
    class Velocity_Component : public Component {
    public:
        Vec2D velocity; ///< Velocity of the entity.

        /**
         * @brief Constructor for Velocity_Component.
         * @param vx Initial velocity along the X-axis.
         * @param vy Initial velocity along the Y-axis.
         */
        Velocity_Component(float vx = 0.0f, float vy = 0.0f)
            : velocity(vx, vy) {}
    };

    /**
    * @class Physics_Component
    * @brief Component representing global physics properties
    */
    class Physics_Component : public Component {
    public:
        Vec2D gravity;
        float damping_factor;
        float max_velocity;
        Vec2D accumulated_force; //to accumulate forces applied to the entity.
        bool is_grounded;

        float mass; //mass of entity
        float inv_mass;
        bool is_static; //to check if the entity is static or not
        float jump_force; //the force applied during a jump


    public:
        /**
     * @brief Constructor for Physics_Component.
     *
     * Initializes the physics properties of the entity, including gravity, damping factor, max velocity,
     * mass, static state, movability, and jump force.
     *
     * @param gravity The gravity vector affecting the entity. Default is (0, DEFAULT_GRAVITY).
     * @param damping_factor The factor to dampen velocity over time. Default is DEFAULT_DAMPING_FACTOR.
     * @param max_velocity The maximum velocity of the entity. Default is DEFAULT_MAX_VELOCITY.
     * @param mass The mass of the entity. Default is 1.0f.
     * @param is_static Indicates if the entity is static. Default is false.
     * @param is_moveable Indicates if the entity can be moved. Default is false.
     * @param jump_force The force applied during a jump. Default is DEFAULT_JUMP_FORCE.
     */
        Physics_Component(Vec2D gravity = Vec2D(0, DEFAULT_GRAVITY),
            float damping_factor = DEFAULT_DAMPING_FACTOR,
            float max_velocity = DEFAULT_MAX_VELOCITY,
            float mass = 1.0f,
            bool is_static = false,
            float jump_force = DEFAULT_JUMP_FORCE,//adjust later
            bool is_grounded = false)

            : gravity(gravity),
            damping_factor(damping_factor),
            max_velocity(max_velocity),
            accumulated_force(Vec2D(0, 0)),
            mass(mass),
            inv_mass((mass > 0.0f) ? 1.0f / mass : 0.0f),
            is_static(is_static),
            jump_force(jump_force),
            is_grounded(is_grounded){}

        /**
         * @brief Applies a force to the entity.
         *
         * Adds the given force to the accumulated forces acting on the entity.
         *
         * @param force The force vector to be applied.
         */

        void apply_force(const Vec2D& force) {
            accumulated_force += force;
        }
        /**
         * @brief Resets all accumulated forces acting on the entity.
         *
         * Sets the accumulated force to zero, preparing the entity for the next physics update.
         */
        void reset_forces() {
            accumulated_force = Vec2D(0, 0);
        }

           /**
            * @brief Sets the mass of the entity.
            *
            * Adjusts the mass and its inverse. If the mass is zero or negative, the inverse mass is set to zero.
            *
            * @param m The new mass of the entity.
            */        
        void set_mass(float m) {
            mass = m;
            inv_mass = (m > 0.0f) ? 1.0f / m : 0.0f;
        }
    };
    /**
    * @class Graphics_Component
    * @brief Component representing an entity's graphical data.
    */
    class Graphics_Component : public Component {
    public:
        std::string model_name;
        glm::vec3 color;
        std::string texture_name; 
        GLuint shd_ref;
        glm::mat3 mdl_to_ndc_xform;

        // Default constructor
        Graphics_Component()
            : model_name(DEFAULT_MODEL_NAME), color(DEFAULT_COLOR), texture_name(DEFAULT_TEXTURE_NAME), shd_ref(DEFAULT_SHADER_REF), mdl_to_ndc_xform(DEFAULT_MDL_TO_NDC_MAT) {}

        /**
         * @brief Constructor for Graphics_Component.
         * @param mdl_to_ndc_xform Model-to-world-to-NDC transformation.
         * @param mdl_ref Reference to model
         * @param shd_ref Reference to shader
         */

        Graphics_Component(std::string mdl_name, glm::vec3 clr, std::string tex_name, GLuint shader,glm::mat3 xform) : 
                           model_name(mdl_name), color(clr), texture_name(tex_name), shd_ref(shader), mdl_to_ndc_xform(xform) {}

    }; 

    /**
    * @class Collision_Component
    * @brief Component representing an entity's component data.
    */
    class Collision_Component : public Component
    {
    public:
        float width, height;

        //constructor for collision components 
        Collision_Component(float width = 0.0f, float height = 0.0f)
            : width(width), height(height) {}
    };

    /**
    * @class Audio_Component
    * @brief Component representing an entity's audio data
    */
    class Audio_Component : public Component {
    private:
        //std::string entityid; //convert entityID into a string to be used as key
        std::string filename;
        //std::vector<std::string> filenames;//can be vector//need rethink abit more
        PlayState audio_state;
        AudioType audio_type;
        //FileFormat file_format;
        //AudioCommand audio_command;

        float volume;   //range of using FMOD is 0.0f to 1.0f <- has already been clamp to not exceed
        float pitch;
        bool islooping;
        bool is3d;

        Vec3D position; //position of where the sound is emitting from
        float mindist;  //the min range for listener to be in to hear the sound (closer)
        float maxdist;  //the max range for listener to be in to hear the sound (further)
        
    public:

        Audio_Component() : filename(""), audio_state(PLAYING), audio_type(SFX), volume(1.0f),
            pitch(1.0f), islooping(false), is3d(false), position(), mindist(1.0f), maxdist(100.0f) {}

        Audio_Component(const std::string& filename, AudioType audio_type, float volume, float pitch_v, bool is_3d = false, bool islooping = false) :
            filename(filename), audio_state(PLAYING), audio_type(audio_type),
            volume(std::clamp(volume, 0.0f, 1.0f)), pitch(std::clamp(pitch_v, 0.5f, 2.0f)), islooping(islooping), is3d(is_3d), position(), mindist(1.0f), maxdist(100.0f) {}

        void set_filename(const std::string filepath) { this->filename = Path_Helper::get_executable_directory() + filepath;}
        const std::string& get_filename() const { return filename; }

        void set_audio_state(PlayState state) { this->audio_state = state; }
        PlayState get_audio_state() const { return audio_state; }

        void set_audio_type(AudioType type) { this->audio_type = type;}
        AudioType get_audio_type() const { return audio_type; }

        //void set_file_format(FileFormat type) { this->file_format = type; }
        //FileFormat get_file_format() const { return file_format; }

        //void set_audio_command(AudioCommand command) { this->audio_command = command; }
        //AudioCommand get_audio_command() const { return audio_command; }

        void set_volume(float new_volume) { this->volume = std::clamp(new_volume, 0.0f, 1.0f); }
        float get_volume() const { return volume;  }

        void set_pitch(float new_pitch) { this->pitch = std::clamp(new_pitch, 0.5f, 2.0f); }
        float get_pitch() const { return pitch;  }

        void set_is_looping(bool loop) { this->islooping = loop; }
        bool get_is_looping() const { return islooping; }

        void set_is3d(bool is_3d) { this->is3d = is_3d; }
        bool get_is3d() const { return is3d; };

        void set_position(const Vec3D& pos) { position = pos; }
        Vec3D get_position() const { return position; }

        void set_min_distance(float dist) { mindist = dist; }
        float get_min_distance() const { return mindist; }

        void set_max_distance(float dist) { maxdist = dist; }
        float get_max_distance() const { return maxdist; }

    };
     * @class GUI_Component
     * @brief Component representing GUI element data
     */
    class GUI_Component : public Component {
    public:
        float progress;      ///< Progress value for loading bars (0.0f to 1.0f)
        bool is_progress_bar;///< Whether this GUI element is a progress bar
        bool is_container;   ///< Whether this is a container element
        bool is_visible;     ///< Visibility state of the GUI element
        Vec2D relative_pos;  ///< Position relative to parent container

        GUI_Component(bool is_progress = false, bool is_container = false)
            : progress(0.0f)
            , is_progress_bar(is_progress)
            , is_container(is_container)
            , is_visible(true)
            , relative_pos(0.0f, 0.0f) {}
    };

} // namespace lof

#endif // LOF_COMPONENT_H
