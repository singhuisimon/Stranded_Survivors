/**
 * @file Component.h
 * @brief Defines the base Component class and specific component types for the Entity Component System (ECS).
 * @author Simon Chan (40%), Amanda Leow Boon Suan (29%), Wai Lwin Thit (20%), Saw Hui Shan (10%), Liliana Hanawardani (1%)
 * @date September 15, 2024s
 * Copyright (C) 2025 DigiPen Institute of Technology.
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
#include <optional>

// Include Utility headers
#include "../Utility/Vector2D.h"
#include "../Utility/Vector3D.h"
#include "../Utility/Constant.h"
#include "../Utility/Force_Helper.h"
#include "../Manager/Log_Manager.h"

// For Graphics Component
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
        glm::vec4 color;
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

        Graphics_Component(std::string mdl_name, glm::vec4 clr, std::string tex_name, GLuint shader, glm::mat3 xform) :
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
         * @param start_tile_health Starting tile health for tile objects.
         * @param curr_tile_health Current tile health for tile objects.
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
            std::string key = DEFAULT_AUDIO_KEY;
            std::string filepath = DEFAULT_AUDIO_FILEPATH;
            AudioType audio_type = DEFAULT_AUDIO_TYPE;
            int max_simultaneous = MIN_SIMULTANEOUS;
            float volume = DEFAULT_AUDIO_FLOAT;
            float pitch = DEFAULT_AUDIO_FLOAT;
            bool islooping = DEFAULT_LOOP;
            //bool isactive = DEFAULT_ACTIVE;
            //unsigned int playcount = DEFAULT_PLAYCOUNT;
            bool is3d = DEFAULT_IS_3D;
        };

        std::vector<SoundConfig> sounds; ///< vectors of sound details

        //for future implementation
        Vec3D position; ///<position of where the sound is emitting from
        float mindist;  ///<the min range for listener to be in to hear the sound (closer)
        float maxdist;  ///<the max range for listener to be in to hear the sound (further)

    public:

        /**
         * @brief Constructor for Audio_Component.
         *        initializes the member values of Audio_Component
         */
        Audio_Component() : sounds(), position(), mindist(1.0f), maxdist(100.0f) {}

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
        /*void add_sound(const std::string& key, const std::string& filepath, PlayState state, AudioType type,
            float volume, float pitch, bool islooping) {*/
        /*void add_sound(const std::string & key, const std::string & filepath, AudioType type, int num,
            float volume, float pitch, bool islooping, bool isactive, unsigned int playcount, bool is3d) {*/
        void add_sound(const std::string& key, const std::string& filepath, AudioType type, int num,
            float volume, float pitch, bool islooping, bool is3d) {

            // Check if the sound key already exists in the list
            auto it = std::find_if(sounds.begin(), sounds.end(), [&](const SoundConfig& sound) {
                return sound.key == key;
            });

            if (it != sounds.end()) {
                // Key already exists, update existing sound properties
                it->filepath = filepath;
                it->audio_type = type;
                it->max_simultaneous = num;
                it->volume = std::clamp(volume, 0.0f, 1.0f);
                it->pitch = std::clamp(pitch, 0.5f, 2.0f);
                it->islooping = islooping;
                //it->isactive = isactive;
                //it->playcount = playcount;
                it->is3d = is3d;
                return;  // Exit since we updated an existing sound
            }

            sounds.push_back({ key, filepath, type, num, std::clamp(volume, 0.0f, 1.0f), std::clamp(pitch, 0.5f, 2.0f),
                islooping, is3d });

            /*sounds.push_back({ key, filepath, type, num, std::clamp(volume, 0.0f, 1.0f), std::clamp(pitch, 0.5f, 2.0f),
                islooping, isactive, playcount, is3d });*/

     //       for (auto& sound : sounds) {
     //           if (sound.key == key) {
     //               //if key already exist update properties
     //               sound.filepath = filepath;
     //               sound.audio_type = type;
     //               sound.max_simultaneous = num;
     //               sound.volume = std::clamp(volume, 0.0f, 1.0f);  //FMOD can only take value 0.0 to 1.0f
     //               sound.pitch = std::clamp(pitch, 0.5f, 2.0f);    //FMOD can only take pitch 0.5 to 2.0f (with 1.0f being normal)
     //               sound.islooping = islooping;
					//sound.isactive = isactive;
					//sound.toplay = toplay;
     //               sound.is3d = is3d;
     //               return;
     //           }
     //       }

   //         SoundConfig new_sound;
   //         new_sound.key = key;
   //         new_sound.filepath = filepath;
   //         //new_sound.audio_state = state;
   //         new_sound.audio_type = type;
   //         new_sound.max_simultaneous = num;
   //         new_sound.volume = std::clamp(volume, 0.0f, 1.0f);  //FMOD can only take value 0.0 to 1.0f
   //         new_sound.pitch = std::clamp(pitch, 0.5f, 2.0f);    //FMOD can only take pitch 0.5 to 2.0f (with 1.0f being normal)
   //         new_sound.islooping = islooping;
			//new_sound.isactive = isactive;
   //         new_sound.toplay = toplay;
   //         new_sound.is3d = is3d;

            //sounds.push_back(new_sound);
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

        /**
        * @brief Setter for the key in soundconfig, using old key to identify which sound it is.
        * @param old_key The old unique identifier of soundconfig to be repalced.
        * @param new_key The new unique identifier of soundconfig to replace the old one.
        */
        void set_key(const std::string& old_key, const std::string& new_key) {

            for (auto& sound : sounds) {
                if (sound.key == old_key) {
                    sound.key = new_key;
                }
            }
        }

        /**
        * @brief Setter for the filepath in soundconfig
        * @param key The unique identifier of soundconfig
        * @param path The new path.
        */
        void set_filepath(const std::string& key, const std::string& path) {
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
            const SoundConfig* sound = get_sound_by_key(key);
            return sound ? sound->filepath : "";
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
        * @brief Setter for max simultaneous in soundconfig
        * @param key The unique idenitifier for soundconfig
        * @param num The new max simultaneous number
        */
        void set_max_simultaneous(const std::string& key, const int num) {
            for (auto& sound : sounds) {
                if (sound.key == key) {
                    sound.max_simultaneous = num;
                }
            }
        }

        /**
        * @brief Getter for max simultaneous in soundconfig using param key
        * @param key The unique identifier for soundconfig
        */
        int get_max_simultaneous(const std::string& key) const {
            return get_sound_by_key(key)->max_simultaneous;
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

        ///**
        //* @brief Getter for active in soundconfig
        //* @param key The unique identifier for soundconfig
        //*/
        //bool get_active(const std::string& key) const {
        //    return get_sound_by_key(key)->isactive;
        //}

        ///**
        //* @brief Setter for isactive
        //* @param is_active The new boolean value for isactive
        //*/
        //void set_isactive(const std::string& key, bool is_active) {
        //    for (auto& sound : sounds) {
        //        if (sound.key == key) {
        //            sound.isactive = is_active;
        //        }
        //    }
        //}

        ///**
        //* @brief Getter for toplay in soundconfig
        //* @param key The unique identifier for soundconfig
        //*/
        //unsigned int get_playcount(const std::string& key) const {
        //    return get_sound_by_key(key)->playcount;
        //}

        ///**
        //* @brief Increase playcount
        //* @param playcount The new unsigned int value for playcount
        //*/
        //void increase_playcount(const std::string& key) {
        //    for (auto& sound : sounds) {
        //        if (sound.key == key) {
        //            sound.playcount++;
        //        }
        //    }
        //}

        ///**
        //* @brief Setter for is toplay
        //* @param key The unique identifier for soundconfig
        //* @param new_count The new unsigned int value for playcount
        //*/
        //void set_playcount(const std::string& key, unsigned int new_count) {
        //    for (auto& sound : sounds) {
        //        if (sound.key == key) {
        //            sound.playcount = new_count;
        //        }
        //    }
        //}

        /**
        * @brief Setter for is 3D
        * @param is_3d The new boolean value for 3D
        */
        void set_is3d(const std::string& key, bool is_3d) {
            for (auto& sound : sounds) {
                if (sound.key == key) {
                    sound.is3d = is_3d;
                }
            }
        }

        /**
        * @brief Getter for is 3D
        */
        bool get_is3d(const std::string& key) const {
            return get_sound_by_key(key)->is3d;
        }

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
         * @param font_name Name of font type.
         * @param text The text that will be rendered.
         * @param color Color of the text.
         * @param scale Scale value of the text.
         */

        Text_Component(std::string name, std::string text, glm::vec3 color, glm::vec2 scale) :
            font_name(name), text(text), color(color), scale(scale) {}
    };

    using ScriptData = std::unordered_map<std::string, std::variant<int, float, std::string, bool, Vec2D>>;

    /**
	* @class Logic_Component
	* @brief Component representing an entity's logic data.
    */
    class Logic_Component : public Component {
    public:

        /**
         * @struct LogicData
		 * @brief Holds all the details in each logic component currently it has.
         */
        struct LogicData {
            std::string script_name;
            std::string init_func;
            std::string update_func;
            std::string end_func;
            ScriptData script_data;
            ExecutionState state;
            bool is_active;

            /**
			 * @brief Constructor for LogicData.
             */
            LogicData(const std::string& name, const std::string& init, const std::string& update,
                const std::string& end, const ScriptData& data, ExecutionState set_state, bool active = true)
                : script_name(name), init_func(init), update_func(update),
                end_func(end), script_data(data), is_active(active), state(set_state) {
            }
        };

        std::vector<std::shared_ptr<LogicData>> logic_datas;

        /**
         * @brief Default constructor
         */
        Logic_Component() = default;

		/**
		 * @brief Destructor
		 */
        ~Logic_Component() {
            logic_datas.clear();
            //LM.write_log("Logic_Component::clean up complete");
        }

		/**
		 * @brief Getter for the vector of logic data
		 * @return The vector of logic data
		 */
        const std::vector<std::shared_ptr<LogicData>>& get_logic_datas() const { return logic_datas; }

        /**
		 * @brief Add the script to the logic component
		 * @param name The name of the script
		 * @param init The name of the init function
		 * @param update The name of the update function
         */
        void add_script(const std::string& name, const std::string& init, const std::string& update,
            const std::string& end, const ScriptData& data, ExecutionState set_state, bool active = true) {
            auto script = std::make_shared<LogicData>(name, init, update, end, data, set_state, active);

            logic_datas.push_back(script);
        }

        /**
		 * @brief Retrieve a logic data from the logic_datas vector.
		 * @param script_name The name of the script.
		 * @param update_func The name of the update function.
		 * @return The logic data if found, nullptr otherwise.
         */
        std::shared_ptr<LogicData> find_logic_data(const std::string& script_name, const std::string& update_func) const {
            auto it = std::find_if(logic_datas.begin(), logic_datas.end(),
                [&](const std::shared_ptr<LogicData>& logic_data) {
                    return logic_data->script_name == script_name &&
                        logic_data->update_func == update_func;
                });
            return (it != logic_datas.end()) ? *it : nullptr;
        }

        // Getter and Setter for the script data
        /**
		 * @brief Retrieve the script data from the logic component.
		 * @param script_name The name of the script.
		 * @param update_func The name of the update function.
         */
        const ScriptData& get_script_data(const std::string& script_name, const std::string& update_func) const {
            auto logic_data = find_logic_data(script_name, update_func);
            if (logic_data) {
                return logic_data->script_data;
            }
            //throw std::runtime_error("Script not found");
            LM.write_log("get_script_data in logic component: script not found");
        }

        /**
		 * @brief Retrieve the individual script data from the logic component.
		 * @param script_data The script data to retrieve from.
		 * @param data_name The name of the data to retrieve.
         */
        template<typename T>
        T get_script_individual_data(const ScriptData& script_data, const std::string& data_name) {
            auto it = script_data.find(data_name);
            if (it == script_data.end()) {
                throw std::out_of_range("Key not found in ScriptData: " + data_name);
            }
            if (auto value = std::get_if<T>(&it->second)) {
                return *value;
            }
            else {
                throw std::bad_variant_access();
            }
        }

        /**
		 * @brief Set the script data in the logic component.
		 * @param script_name The name of the script.
		 * @param update_func The name of the update function.
		 * @param new_data The new data to set.
         */
        void set_script_data(const std::string& script_name, const std::string& update_func, const ScriptData& new_data) {
            auto logic_data = find_logic_data(script_name, update_func);
            if (logic_data) {
                logic_data->script_data = new_data;
            }
            else {
                //throw std::runtime_error("Script not found");
                LM.write_log("set_script_data in logic component: script not found");
            }
        }

        /**
		 * @brief Set the active state of the script.
         * @param script_name The name of the script.
         * @param update_func The name of the update function.
		 * @param active The new active state to set.
         */
        void set_active(const std::string& script_name, const std::string& update_func, bool active) {
            for (auto& logic_data : logic_datas) {
                if (logic_data->script_name == script_name && logic_data->update_func == update_func) {
                    logic_data->is_active = active;
                    return;
                }
            }

            //throw std::runtime_error("Script instance not found: " + script_name + "( " + update_func + ")");
        }

        /**
		 * @brief Remove the script from the logic component.
         * @param script_name The name of the script.
         * @param update_func The name of the update function.
         */
        void remove_script(const std::string& script_name, const std::string& update_func) {
            auto it = std::remove_if(logic_datas.begin(), logic_datas.end(),
                [&](const std::shared_ptr<LogicData>& logic_data) {
                    return logic_data->script_name == script_name &&
                        logic_data->update_func == update_func;
                });
            if (it != logic_datas.end()) {
                logic_datas.erase(it, logic_datas.end());
            }
        }

        /**
		 * @brief Get the script state.
         * @param script_name The name of the script.
         * @param update_func The name of the update function.
		 * @return The state of the script.
         */
        ExecutionState get_state(const std::string& script_name, const std::string& update_func) const {
            for (auto& logic_data : logic_datas) {
                if (logic_data->script_name == script_name && logic_data->update_func == update_func) {
                    return logic_data->state;
                }
            }
            return ExecutionState::Terminated;
        }

        /**
         * @brief Set the script state
         * @param script_name The name of the script.
         * @param update_func The name of the update function.
		 * @param new_state The new state to set.
         */
        void set_state(const std::string& script_name, const std::string& update_func, ExecutionState new_state) {
            for (auto& logic_data : logic_datas) {
                if (logic_data->script_name == script_name && logic_data->update_func == update_func) {
                    logic_data->state = new_state;
                    return;
                }
            }
        }

    };

} // namespace lof

#endif // LOF_COMPONENT_H
