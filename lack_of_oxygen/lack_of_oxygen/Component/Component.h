/**
 * @file Component.h
 * @brief Defines the base Component class and specific component types for the Entity Component System (ECS).
 * @author Simon Chan (50%), Wai Lwin Thit (20%), Amanda Leow Boon Suan (20%), Saw Hui Shan (10%)
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
#include <map>
#include <unordered_map>

// Include Utility headers
#include "../Utility/Vector2D.h"
#include "../Utility/Vector3D.h"
#include "../Utility/Constant.h"
//#include "../Utility/Path_Helper.h"
// #include "../Utility/Path_Helper.h"
#include "../Utility/Force_Helper.h"
#include "../Manager/Log_Manager.h"
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
    //forward declaration
    class Force_Helper;
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

    private:
        Vec2D gravity;
        float damping_factor;
        float max_velocity;
        float max_velocity_sq;
        Vec2D accumulated_force; //to accumulate forces applied to the entity.
        Vec2D acceleration;

        bool is_grounded;
        bool has_jumped;

        bool jump_requested;

        float mass; //mass of entity
        float inv_mass;
        bool is_static; //to check if the entity is static or not
        float jump_force; //the force applied during a jump

    public:
        Force_Helper force_helper;


    public:
        /**
         * @brief Constructor for Physics_Component.
         *
         * Initializes the physics properties of the entity, including gravity, damping factor,
         * max velocity, mass, static state, and jump properties.
         *
         * @param gravity The gravity vector applied to the entity.
         * @param damping_factor The factor for velocity reduction to simulate friction.
         * @param max_velocity The maximum velocity allowed for the entity.
         * @param mass The mass of the entity.
         * @param is_static Determines if the entity is static.
         * @param has_jumped Indicates if the entity has jumped.
         * @param jump_requested Indicates if a jump was requested.
         * @param jump_force The force applied to initiate a jump.
         * @param is_grounded Indicates if the entity is grounded.
         */
        Physics_Component(Vec2D gravity = Vec2D(0, DEFAULT_GRAVITY),
            float damping_factor = DEFAULT_DAMPING_FACTOR,
            float max_velocity = DEFAULT_MAX_VELOCITY,
            float mass = 1.0f,
            bool is_static = false,
            bool has_jumped = false,
            bool jump_requested = false,
            float jump_force = DEFAULT_JUMP_FORCE,//adjust later
            bool is_grounded = false)

            : gravity(gravity),
            damping_factor(damping_factor),
            max_velocity(max_velocity), max_velocity_sq(max_velocity* max_velocity),
            accumulated_force(Vec2D(0, 0)),
            acceleration(Vec2D(0, 0)),
            mass(mass),
            inv_mass((mass > 0.0f) ? 1.0f / mass : 0.0f),
            is_static(is_static),
            jump_force(jump_force),
            is_grounded(is_grounded),
            has_jumped(has_jumped),
            jump_requested(jump_requested) {}

        //getters and setters
        //getters
        //non-const references 
        Vec2D& get_gravity() { return gravity; }
        float& get_damping_factor() { return damping_factor; }
        float& get_max_velocity() { return max_velocity; }
        float& get_max_velocity_sq() { return max_velocity_sq; }
        Vec2D& get_accumulated_force() { return accumulated_force; }
        Vec2D& get_acceleration() { return acceleration; }
        float& get_mass() { return mass; }
        float& get_inv_mass() { return inv_mass; }
        bool& get_is_static() { return is_static; }
        bool& get_is_grounded() { return is_grounded; }
        bool& get_has_jumped() { return has_jumped; }
        bool& get_jump_requested() { return jump_requested; }
        float& get_jump_force() { return jump_force; }

        const Vec2D& get_gravity() const { return gravity; }
        const float& get_damping_factor() const { return damping_factor; }
        const float& get_max_velocity() const { return max_velocity; }
        const float& get_max_velocity_sq() const { return max_velocity_sq; }
        const Vec2D& get_accumulated_force() const { return accumulated_force; }
        const Vec2D& get_acceleration()const { return acceleration; }
        const float& get_mass() const { return mass; }
        const float& get_inv_mass()const { return inv_mass; }
        const bool& get_is_static()const { return is_static; }
        const bool& get_is_grounded() const { return is_grounded; }
        const bool& get_has_jumped()const { return has_jumped; }
        const bool& get_jump_requested() const { return jump_requested; }
        const float& get_jump_force()const { return jump_force; }


        //setters
        void set_damping_factor(float df) { damping_factor = df; }
        void set_max_velocity(float mv) { max_velocity = mv; }
        void set_is_grounded(bool ground) { is_grounded = ground; }
        void set_gravity(Vec2D g) { gravity = g; }
        void set_has_jumped(bool jump) { has_jumped = jump; }
        void set_jump_requested(bool request) { jump_requested = request; }
        void set_accumulated_force(const Vec2D& af) { accumulated_force = af; }
        void set_acceleration(const Vec2D& ac) { acceleration = ac; }

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

        void set_is_static(bool s) { is_static = s; }
        void set_jump_force(float jf) { jump_force = jf; }

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

        void reset_jump_request() { jump_requested = false; }

    }; //END_PHYSICS_COMPONENT

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
            : model_name(DEFAULT_MODEL_NAME), color(DEFAULT_COLOR), texture_name(DEFAULT_TEXTURE_NAME), 
              shd_ref(DEFAULT_SHADER_REF), mdl_to_ndc_xform(DEFAULT_MDL_TO_NDC_MAT) {}

        /**
         * @brief Constructor for Graphics_Component.
         * @param model_name Name of model.
         * @param color Color of model.
         * @param texture_name Name of texture.
         * @param shd_ref Reference to shader.
         * @param mdl_to_ndc_xform Model-to-world-to-NDC transformation.
         */

        Graphics_Component(std::string mdl_name, glm::vec3 clr, std::string tex_name, GLuint shader, glm::mat3 xform) :
            model_name(mdl_name), color(clr), texture_name(tex_name), shd_ref(shader), mdl_to_ndc_xform(xform) {}

    };

    /**
    * @class Animation_Component
    * @brief Component representing an entity's graphical data.
    */
    class Animation_Component : public Component {
    public:
        std::map<std::string, std::string> animations; 
        unsigned int curr_animation_idx;
        unsigned int start_animation_idx;
        unsigned int curr_frame_index;
        unsigned int start_tile_health;
        unsigned int curr_tile_health;

        // Default constructor
        Animation_Component()
            : curr_animation_idx(std::stoi(DEFAULT_ANIMATION_IDX)), start_animation_idx(std::stoi(DEFAULT_ANIMATION_IDX)),
              curr_frame_index(DEFAULT_FRAME_INDEX), start_tile_health(DEFAULT_TILE_HEALTH), curr_tile_health(DEFAULT_TILE_HEALTH){
            animations.insert(std::make_pair(DEFAULT_ANIMATION_IDX, DEFAULT_ANIMATION_NAME));
        }

        /**
         * @brief Constructor for Animation_Component.
         * @param animations Collection of animations usable by entity.
         * @param curr_animation_idx Index of the current animation.
         * @param start_animation_idx Index of the starting animation.
         */

        Animation_Component(std::pair<std::string, std::string> animation, unsigned int curr_animation, unsigned int start_animation, unsigned int curr_frame, unsigned int start_health) :
            curr_animation_idx(curr_animation), start_animation_idx(start_animation), curr_frame_index(curr_frame), start_tile_health(start_health), curr_tile_health(start_health){
            animations.insert(animation);
        }
    };

    /**
    * @class Collision_Component
    * @brief Component representing an entity's component data.
    */
    class Collision_Component : public Component
    {
    public:
        float width, height;
        bool collidable;

        //constructor for collision components 
        Collision_Component(float width = 0.0f, float height = 0.0f, bool collidable = false)
            : width(width), height(height), collidable(collidable) {}
    };

    /**
    * @class Audio_Component
    * @brief Component representing an entity's audio data
    */
    class Audio_Component : public Component {
    private:

        /**
        * @struct SoundConfig
        * @brief Holds all the details in each sound component currently it has.
        */
        struct SoundConfig {
            std::string key = "";
            std::string filepath = "";
            PlayState audio_state = NONE;
            AudioType audio_type = NIL;
            float volume = 0.0f;
            float pitch = 1.0f;
            bool islooping = false;
        };

        std::vector<SoundConfig> sounds; ///< vectors of sound details


        //for future implementation
        bool is3d;      ///<check if the sound is 3D or 2D.

        Vec3D position; ///<position of where the sound is emitting from
        float mindist;  ///<the min range for listener to be in to hear the sound (closer)
        float maxdist;  ///<the max range for listener to be in to hear the sound (further)

    public:

        /**
         * @brief Constructor for Audio_Component.
         *        initializes the member values of Audio_Component
         */
        Audio_Component() : sounds(), is3d(false), position(), mindist(1.0f), maxdist(100.0f) {}

        /**
         * @brief Constructor for SoundConfig.
         * @param key The purpose of the sound which doubles as a key function.
         * @param filepath Contains the filepath to the audio
         * @param state Contains the state of the sound path
         * @param type Contains details regarding is it a BGM or SFX
         * @param volume Contains the volume level for the sound.
         * @param pitch Contains the pitch level for the sound.
         * @param islooping Contains the loop state of the sound
         */
        void add_sound(const std::string& key, const std::string& filepath, PlayState state, AudioType type,
            float volume, float pitch, bool islooping) {
            for (auto& sound : sounds) {
                if (sound.key == key) {
                    //if key already exist update properties
                    sound.filepath = filepath;
                    sound.audio_state = state;
                    sound.audio_type = type;
                    sound.volume = std::clamp(volume, 0.0f, 1.0f);  //FMOD can only take value 0.0 to 1.0f
                    sound.pitch = std::clamp(pitch, 0.5f, 2.0f);    //FMOD can only take pitch 0.5 to 2.0f (with 1.0f being normal)
                    sound.islooping = islooping;
                    return;
                }
            }

            SoundConfig new_sound;
            new_sound.key = key;
            new_sound.filepath = filepath;
            new_sound.audio_state = state;
            new_sound.audio_type = type;
            new_sound.volume = std::clamp(volume, 0.0f, 1.0f);  //FMOD can only take value 0.0 to 1.0f
            new_sound.pitch = std::clamp(pitch, 0.5f, 2.0f);    //FMOD can only take pitch 0.5 to 2.0f (with 1.0f being normal)
            new_sound.islooping = islooping;

            sounds.push_back(new_sound);
        }

        /**
        * @brief Getters for the vector of soundconfig
        */
        const std::vector<SoundConfig>& get_sounds() const { return sounds; }

        /**
        * @brief Getter for the soundconfig using param key
        * @param key The unique identifier of soundconfig
        */
        const SoundConfig* get_sound_by_key(const std::string& key) const {
            for (const auto& sound : sounds) {
                if (sound.key == key) {
                    return &sound;
                }
            }
            return nullptr;
        }

        
        void set_key(const std::string& old_key, std::string& new_key) {

            std::cout << "checking: old_key = " << old_key << "; new_key = " << new_key << std::endl;
            for (auto& sound : sounds) {
                if (sound.key == old_key) {
                    std::cout << "sound.key before: " << sound.key << std::endl;
                    sound.key = new_key;
                    std::cout << "sound.key after: " << sound.key << std::endl;
                }
            }
        }

        /**
        * @brief Setter for the filepath in soundconfig
        * @param key The unique identifier of soundconfig
        * @param path The new path.
        */
        void set_filepath(const std::string& key, std::string& path) {
            for (auto& sound : sounds) {
                if (sound.key == key) {
                    sound.filepath = path;
                }
            }
        }

        /**
        * @brief Getter for the filepath in soundconfig using param key
        * @param key The unique identifier of soundconfig
        */
        std::string get_filepath(const std::string& key) const {
            return get_sound_by_key(key)->filepath;
        }

        /**
        * @brief Setter for audio state in soundconfig
        * @param key The unique idenitifier for soundconfig
        * @param state The new state
        */
        void set_audio_state(const std::string& key, PlayState state) {
            for (auto& sound : sounds) {
                if (sound.key == key) {
                    sound.audio_state = state;
                }
            }
        }

        /**
        * @brief Getter for audio state in soundconfig using param key
        * @param key The unique identifier for soundconfig
        */
        PlayState get_audio_state(const std::string& key) const {
            return get_sound_by_key(key)->audio_state;
        }

        /**
        * @brief Setter for audio type in soundconfig
        * @param key The unique identifier of soundconfig
        * @param type The new type
        */
        void set_audio_type(const std::string& key, AudioType type) {
            for (auto& sound : sounds) {
                if (sound.key == key) {
                    sound.audio_type = type;
                }
            }
        }

        /**
        * @brief Getter for audio type in soundconfig
        * @param key The unique identifier for soundconfig
        */
        AudioType get_audio_type(const std::string& key) const {
            return get_sound_by_key(key)->audio_type;
        }

        /**
        * @brief Setter for volume in soundconfig
        * @param key The unique identifier for soundconfig
        * @param volume The new volume
        */
        void set_volume(const std::string& key, float volume) {
            for (auto& sound : sounds) {
                if (sound.key == key) {
                    sound.volume = std::clamp(volume, 0.0f, 1.0f);
                }
                std::cout << sound.volume << std::endl; LM.write_log("volume change %f", sound.volume);
            }
        }

        /**
        * @brief Getter for volume in soundconfig
        * @param key The unique identifier for soundconfig
        */
        float get_volume(const std::string& key) const {
            return get_sound_by_key(key)->volume;
        }

        /**
        * @brief Setter for pitch in soundconfig
        * @param key The unique identifier for soundconfig
        * @param pitch The new pitch
        */
        void set_pitch(const std::string& key, float pitch) {
            for (auto& sound : sounds) {
                if (sound.key == key) {
                    sound.pitch = std::clamp(pitch, 0.5f, 2.0f);
                }
            }
        }

        /**
        * @brief Getter for pitch in soundconfig
        * @param key The unique identifier for soundconfig
        */
        float get_pitch(const std::string& key) const {
            return get_sound_by_key(key)->pitch;
        }

        /**
        * @brief Setter for loop in soundconfig
        * @param key The unique identifier for soundconfig
        * @param islooping The new boolean value for loop
        */
        void set_loop(const std::string& key, bool islooping) {
            for (auto& sound : sounds) {
                if (sound.key == key) {
                    sound.islooping = islooping;
                }
            }
        }

        /**
        * @brief Getter for loop in soundconfig
        * @param key The unique identifier for soundconfig
        */
        bool get_loop(const std::string& key) const {
            return get_sound_by_key(key)->islooping;
        }

        /**
        * @brief Setter for is 3D
        * @param is_3d The new boolean value for 3D
        */
        void set_is3d(bool is_3d) { this->is3d = is_3d; }

        /**
        * @brief Getter for is 3D
        */
        bool get_is3d() const { return is3d; }

        /**
        * @brief Setter for position
        * @param pos The new vec3d position
        */
        void set_position(const Vec3D& pos) { position = pos; }

        /**
        * @brief Getter for position
        */
        Vec3D get_position() const { return position; }

        /**
        * @brief Setter for min distance
        * @param dist The new distance
        */
        void set_min_distance(float dist) { mindist = dist; }

        /**
        * @brief Getter for min distance
        */
        float get_min_distance() const { return mindist; }

        /**
        * @brief Setter for max distance
        * @param dist The new distance
        */
        void set_max_distance(float dist) { maxdist = dist; }

        /**
        * @brief Getter for max distance
        */
        float get_max_distance() const { return maxdist; }

    };

    /**
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

    class Logic_Component : public Component {
    public:
        enum class LogicType {
            MOVING_PLATFORM    // Platform that moves between points
        };

        enum class MovementPattern {
            LINEAR = 0,            // Move back and forth in a line
            CIRCULAR = 1           // Move in a circular path
        };

        LogicType logic_type;
        MovementPattern movement_pattern;
        bool is_active{ true };           // Whether object is currently active/visible
        float timer{ 0.0f };              // For movement timing
        float movement_speed{ 100.0f };   // Speed of movement
        float movement_range{ 200.0f };   // Range of movement
        Vec2D origin_pos;                 // Starting/center position
        bool reverse_direction{ false };   // For changing direction (horizontal/vertical)
        bool rotate_with_motion{ false }; // Whether object rotates to face movement direction

        Logic_Component(LogicType type = LogicType::MOVING_PLATFORM,
            MovementPattern pattern = MovementPattern::LINEAR)
            : logic_type(type)
            , movement_pattern(pattern) {}

        // Add helper method to set movement pattern
        void set_movement_pattern(MovementPattern pattern) {
            movement_pattern = pattern;
            // Reset timer when changing patterns
            timer = 0.0f;
        }
    };

    /**
    * @class Text_Component
    * @brief Component representing a text data.
    */
    class Text_Component : public Component {
    public:
        std::string font_name;
        std::string text;
        glm::vec3 color;
        glm::vec2 scale;

        // Default constructor
        Text_Component() : font_name(DEFAULT_FONT_NAME), text(DEFAULT_FONT_NAME), color(DEFAULT_COLOR), scale(glm::vec2(1.0f, 1.0f)) {}

        /**
         * @brief Constructor for Text_Component.
         * @param font_name Font type.
         * @param text Text that will be rendered.
         * @param color Color of the text.
         */

        Text_Component(std::string name, std::string text, glm::vec3 color, glm::vec2 scale) :
            font_name(name), text(text), color(color), scale(scale) {}

    };

} // namespace lof

#endif // LOF_COMPONENT_H
