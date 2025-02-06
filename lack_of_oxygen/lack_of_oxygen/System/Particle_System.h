/**
 * @file Particle_System.h
 * @brief Defines the Particle_System class for the ECS.
 * @details Manages the entire lifetime and variables of each particles in the game
 * @author Chua Wen Bin Kenny (100%)
 * @date Febuary 04, 2025
 * Copyright (C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#ifndef LOF_PARTICLE_SYSTEM_H
#define LOF_PARTICLE_SYSTEM_H



 // Include file dependencies
#include "System.h"
#include "../Manager/ECS_Manager.h"
#include "../Entity/Entity.h"
#include "../Component/Component.h"

// Include Utility headers
#include "../Utility/constant.h"    // To access constants 
#include "../Utility/Clock.h"       // To monitor time delay

#include <array>
#include <random>


namespace lof {
    /**
     * @class Particle_System
     * @brief System responsible for updating animation and frames sequence.
     */
    class Particle_System : public System {
        //friend class Render_System;

        // Data for the particles
        struct Particle_Data {
            Vec2D position{};
            Vec3D color{};
            ParticleType type{};
            int id{};
            float curr_size {1.0f};
            float start_size {1.0f};
            float speed {0.0f};
            float direction {0.0f};
            float life_span {0.0f};
            float life_left {0.0f};
        };

        // Storage for the particle data and full set of particles
        using PARTICLES = std::array<Particle_Data, MAX_PARTICLES>;
        PARTICLES particles_storage; 
        std::unordered_map<std::string, Particle_Data> particle_base;
        unsigned int active_particles;

        // For random engine
        std::mt19937 part_rand_engine;
        std::uniform_int_distribution<std::mt19937::result_type> rand_distribution;

    public:
        /**
         * @brief Constructor for Particle_System.
         * @param manager Reference to the ECS_Manager.
         */
        Particle_System();

        /**
         * @brief Returns the system's type.
         * @return The string "Particle_System".
         */
        std::string get_type() const override;

        /**
         * @brief Updates the particles' data based on time.
         * @param delta_time The time elapsed since the last update, typically in seconds.
         */
        void update(float delta_time) override;

        // This creates and sets the parameters needed to emit particles for an event
        void particle_emit(std::string type, Vec2D pos, Vec3D col, float lifespan = 0.0f);

        // This destroys a particle once its lifetime is over
        void particle_destroy(int destroy_id);

        // Returns the particle count
        unsigned int get_particles_count();

        // Returns the particle storage
        PARTICLES& get_particle_storage();

        // Returns a random value between 0.0f to 1.0f
        float get_rand_float();

    };

} // namespace lof

#endif // LOF_ANIMATION_SYSTEM_H
