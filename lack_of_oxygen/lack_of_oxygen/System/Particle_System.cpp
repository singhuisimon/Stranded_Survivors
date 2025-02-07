/**
 * @file Particle_System.cpp
 * @brief Implements the Particle_System class for the ECS that
 *        manages the entire lifetime and variables of each particles in the game.
 * @author Chua Wen Bin Kenny (100%)
 * @date Febuary 04, 2025
 * Copyright (C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

 // Include header file
#include "Particle_System.h"

namespace lof {

	// Constructor for Particle_System
	Particle_System::Particle_System() {

		// Begin the counter for number of active particles
		active_particles = 0;

		// Initialize random engine
		part_rand_engine.seed(std::random_device()());

		// Create and store particle base for usage later
		// Walking particle data
		Particle_Data walking = {
			Vec2D(),				// position
			Vec3D(),				// color
			ParticleType::walking,	// type
			0,						// id
			2.0f,					// current size
			2.0f,					// starting size
			0.5f,					// speed
			270.0f,					// direction
			0.1f,					// lifespan
			0.1f					// life left
		};				
		particle_base.emplace("walking", walking);

		// Mining particle data
		Particle_Data mining = {
			Vec2D(),				// position
			Vec3D(),				// color
			ParticleType::mining,	// type
			0,						// id
			4.0f,					// current size
			4.0f,					// starting size
			10.0f,					// speed
			360.0f,					// direction
			0.2f,					// lifespan
			0.2f					// life left
		};
		particle_base.emplace("mining", mining);

		// Rock particle data
		Particle_Data rock = {
			Vec2D(),				// position
			Vec3D(),				// color
			ParticleType::rock,		// type
			0,						// id
			4.0f,					// current size
			4.0f,					// starting size
			0.0f,					// speed
			0.0f,					// direction
			1.0f,					// lifespan
			1.0f					// life left
		};
		particle_base.emplace("rock", rock);

		// Dirt particle data
		Particle_Data dirt = {
			Vec2D(),				// position
			Vec3D(),				// color
			ParticleType::dirt,		// type
			0,						// id
			4.0f,					// current size
			4.0f,					// starting size
			0.0f,					// speed
			0.0f,					// direction
			1.0f,					// lifespan
			1.0f					// life left
		};
		particle_base.emplace("dirt", dirt);

		// Quartz particle data
		Particle_Data quartz = {
			Vec2D(),				// position
			Vec3D(),				// color
			ParticleType::quartz,	// type
			0,						// id
			3.0f,					// current size
			3.0f,					// starting size
			0.0f,					// speed
			0.0f,					// direction
			1.0f,					// lifespan
			1.0f					// life left
		};
		particle_base.emplace("quartz", quartz);

		// Emerald particle data
		Particle_Data emerald = {
			Vec2D(),				// position
			Vec3D(),				// color
			ParticleType::emerald,	// type
			0,						// id
			3.0f,					// current size
			3.0f,					// starting size
			0.0f,					// speed
			0.0f,					// direction
			1.0f,					// lifespan
			1.0f					// life left
		};
		particle_base.emplace("emerald", emerald);

		// Sapphire particle data
		Particle_Data sapphire = {
			Vec2D(),				// position
			Vec3D(),				// color
			ParticleType::sapphire,	// type
			0,						// id
			3.0f,					// current size
			3.0f,					// starting size
			0.0f,					// speed
			0.0f,					// direction
			1.0f,					// lifespan
			1.0f					// life left
		};
		particle_base.emplace("sapphire", sapphire);

		// Amethyst particle data
		Particle_Data amethyst = {
			Vec2D(),				// position
			Vec3D(),				// color
			ParticleType::amethyst,	// type
			0,						// id
			3.0f,					// current size
			3.0f,					// starting size
			0.0f,					// speed
			0.0f,					// direction
			1.0f,					// lifespan
			1.0f					// life left
		};
		particle_base.emplace("amethyst", amethyst);

		// Citrine particle data
		Particle_Data citrine = {
			Vec2D(),				// position
			Vec3D(),				// color
			ParticleType::citrine,	// type
			0,						// id
			3.0f,					// current size
			3.0f,					// starting size
			0.0f,					// speed
			0.0f,					// direction
			1.0f,					// lifespan
			1.0f					// life left
		};
		particle_base.emplace("citrine", citrine);

		// Alexandrite particle data
		Particle_Data alexandrite = {
			Vec2D(),					// position
			Vec3D(),					// color
			ParticleType::alexandrite,	// type
			0,							// id
			3.0f,						// current size
			3.0f,						// starting size
			0.0f,						// speed
			0.0f,						// direction
			1.0f,						// lifespan
			1.0f						// life left
		};
		particle_base.emplace("alexandrite", alexandrite);

		// TNT fuse particle data
		Particle_Data tnt = {
			Vec2D(),				// position
			Vec3D(),				// color
			ParticleType::tnt,		// type
			0,						// id
			4.0f,					// current size
			4.0f,					// starting size
			0.0f,					// speed
			0.0f,					// direction
			0.5f,					// lifespan
			0.5f					// life left
		};
		particle_base.emplace("TNT", tnt);

		// TNT explode particle data
		Particle_Data tnt_explode = {
			Vec2D(),					// position
			Vec3D(),					// color
			ParticleType::tnt_explode,	// type
			0,							// id
			1.0f,						// current size
			1.0f,						// starting size
			0.3f,						// speed
			180.0f,						// direction
			0.5f,						// lifespan
			0.5f						// life left
		};
		particle_base.emplace("TNT_Explode", tnt_explode);

		// TNT fuse visual effect particle data
		Particle_Data tnt_vfx = {
			Vec2D(),					// position
			Vec3D(),					// color
			ParticleType::tnt_vfx,		// type
			0,							// id
			2.0f,						// current size
			2.0f,						// starting size
			0.0f,						// speed
			180.0f,						// direction
			2.0f,						// lifespan
			2.0f						// life left
		};
		particle_base.emplace("TNT_VFX", tnt_vfx);
	}

	// Returns the system's type
    std::string Particle_System::get_type() const {
        return "Particle_System";
    }

	// Updates the particles' data based on time
	void Particle_System::update(float delta_time) {
		
		// Update every particles
		for (unsigned int i = 0; i < active_particles; i++) {

			// Decrease lifespan by delta time 
			particles_storage[i].life_left -= delta_time;
			//LM.write_log("Particle ID %d decreasing life by %f", i, delta_time);

			// Destroy particle if its lifespan is over
			if (particles_storage[i].life_left <= 0.0f) {
				LM.write_log("Particle ID %d life has ended", i);
				particle_destroy(particles_storage[i].id);
				continue;
			}

			// Decrease particle size by lifespan 
			if (particles_storage[i].type != tnt_vfx) {
				particles_storage[i].curr_size = particles_storage[i].start_size * (particles_storage[i].life_left / particles_storage[i].life_span);
			}

			// Update movement and direction
			float angle{};
			if (particles_storage[i].type == mining || particles_storage[i].type == tnt_explode) {
				angle = (particles_storage[i].direction * get_rand_float()) * (PI_VALUE / 180.0f);
			} else {
				angle = particles_storage[i].direction * (PI_VALUE / 180.0f);
			}

			// Ensure that particle is set to move
			if (particles_storage[i].direction != 0.0f) {
				particles_storage[i].position.x += (cos(angle) * particles_storage[i].speed);
				particles_storage[i].position.y += (sin(angle) * particles_storage[i].speed);
			}
		}


	}

	// This creates and sets the parameters needed to emit particles for an event
	void Particle_System::particle_emit(std::string type, Vec2D pos, Vec3D col, float lifespan) {

		// Create particles if there are space
		if (active_particles < MAX_PARTICLES) {

			// Set particle data according to type
			particles_storage[active_particles] = particle_base[type];
			
			// Set particle id and increment active particles count
			if (lifespan > 0.0f) {
				particles_storage[active_particles].life_left = lifespan;
				particles_storage[active_particles].life_span = lifespan;
			}

			particles_storage[active_particles].position = pos;
			particles_storage[active_particles].color = col;
			particles_storage[active_particles].id = active_particles;
			active_particles++;

			LM.write_log("Active particles count now is %d after adding particle at pos: %f, %f", active_particles, pos.x, pos.y);
		} else {
			active_particles = MAX_PARTICLES;
		}

	}

	// Destroys a particle
	void Particle_System::particle_destroy(int destroy_id) {
		particles_storage[active_particles - 1].id = particles_storage[destroy_id].id;
		particles_storage[destroy_id] = particles_storage[active_particles - 1];
		active_particles--;

		LM.write_log("Particle ID %d is destroyed. Particle count is %d", destroy_id, active_particles);
	}

	// Returns the count of particles
	unsigned int Particle_System::get_particles_count() { return active_particles; }

	// Returns a reference to the particle storage
	Particle_System::PARTICLES& Particle_System::get_particle_storage() { return particles_storage; }

	// Gets a random float value between 0.0f to 1.0f
	float Particle_System::get_rand_float() {
		return (float)rand_distribution(part_rand_engine) / (float)std::numeric_limits<uint32_t>::max(); 
	}

} // namespace lof
