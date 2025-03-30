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

		// lava splatter visual effect particle data
		Particle_Data lava = {
			Vec2D(),					// position
			Vec3D(),					// color
			ParticleType::lava,			// type
			0,							// id
			4.0f,						// current size
			4.0f,						// starting size
			2.0f,						// speed
			90.0f,						// direction
			5.0f,						// lifespan
			5.0f						// life left
		};
		particle_base.emplace("lava", lava);

		// sweat on player particle data
		Particle_Data sweat_player = {
			Vec2D(),					// position
			Vec3D(),					// color
			ParticleType::sweat_player,		// type
			0,							// id
			2.0f,						// current size
			2.0f,						// starting size
			5.0f,						// speed
			270.0f,						// direction
			0.1f,						// lifespan
			0.1f						// life left
		};
		particle_base.emplace("sweat_player", sweat_player);

		// sweat on screen particle data
		Particle_Data sweat_screen = {
			Vec2D(),					// position
			Vec3D(),					// color
			ParticleType::sweat_screen,	// type
			0,							// id
			25.0f,						// current size
			25.0f,						// starting size
			20.0f,						// speed
			270.0f,						// direction
			0.5f,						// lifespan
			0.5f						// life left
		};
		particle_base.emplace("sweat_screen", sweat_screen);

		// Ship takeoff dirt particle data
		Particle_Data ship_takeoff_dirt = {
			Vec2D(),							// position
			Vec3D(),							// color
			ParticleType::ship_takeoff_dirt,	// type
			0,									// id
			2.0f,								// current size
			2.0f,								// starting size
			0.75f,								// speed
			135.0f,								// direction
			1.0f,								// lifespan
			1.0f								// life left
		};
		particle_base.emplace("ship_takeoff_dirt", ship_takeoff_dirt);

		// Ship takeoff flame particle data
		Particle_Data ship_takeoff_flame = {
			Vec2D(),							// position
			Vec3D(),							// color
			ParticleType::ship_takeoff_flame,	// type
			0,									// id
			3.0f,								// current size
			3.0f,								// starting size
			0.5f,								// speed
			225.0f,								// direction
			1.0f,								// lifespan
			1.0f								// life left
		};
		particle_base.emplace("ship_takeoff_flame", ship_takeoff_flame);

		// Ship takeoff smoke particle data
		Particle_Data ship_takeoff_smoke = {
			Vec2D(),							// position
			Vec3D(),							// color
			ParticleType::ship_takeoff_smoke,	// type
			0,									// id
			3.0f,								// current size
			3.0f,								// starting size
			0.25f,								// speed
			180.0f,								// direction
			3.0f,								// lifespan
			3.0f								// life left
		};
		particle_base.emplace("ship_takeoff_smoke", ship_takeoff_smoke);
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

			/*
				Particles size guide
					1) Default: Decrease in size over time
					2) TNT VFX and lava: No decrease at all
					3) sweat screen: Decrease in size over time, down to half its original size
			*/
			// Decrease particle size by lifespan (For all particles except TNT VFX and lava)
			if (particles_storage[i].type != tnt_vfx && particles_storage[i].type != sweat_screen /*&& particles_storage[i].type != lava*/) {
				particles_storage[i].curr_size = particles_storage[i].start_size * (particles_storage[i].life_left / particles_storage[i].life_span);
			}
			else if (particles_storage[i].type == sweat_screen) {
				// From full to half size within lifespan
				float half_size = particles_storage[i].start_size / 2.0f;
				particles_storage[i].curr_size = half_size + (half_size * particles_storage[i].life_left / particles_storage[i].life_span);
			}

			/*
				Particles direction guide
					1) Default: Direction based fixed direction value of particle
					2) mining and tnt explode: Randomize direction every frame
					3) lava: Fix direction to vertical movement
					4) ship_takeoff_dirt: Move left or right diagonally up and fall after half of lifetime
					5) ship_takeoff_smoke: Move left or right with those closer to have slower speed
			*/
			// Update movement and direction
			float angle{};
			if (particles_storage[i].type == mining || particles_storage[i].type == tnt_explode) { // Randomize values for mining and TNT explosion
				angle = (particles_storage[i].direction * get_rand_float()) * (PI_VALUE / 180.0f);
			}
			else if (particles_storage[i].type == lava) { // Lava's fixed movement
				angle = 90.0f * (PI_VALUE / 180.0f);
			}
			else {
				angle = particles_storage[i].direction * (PI_VALUE / 180.0f);
			}

			/*
				Particles movement guide
					1) Default: Movement based direction and fixed speed value of particle
					2) lava: Movement changes from going up to down when lifespan is <50%
			*/
			// Ensure that particle is set to move
			if (particles_storage[i].direction != 0.0f && particles_storage[i].type != lava) {
				particles_storage[i].position.x += (cos(angle) * particles_storage[i].speed);
				particles_storage[i].position.y += (sin(angle) * particles_storage[i].speed);
			}
			else if (particles_storage[i].type == lava) { // Lava particle movement logic
				float life_percentage = particles_storage[i].life_left / particles_storage[i].life_span;
				if (life_percentage > 0.5f) {
					particles_storage[i].position.x += (cos(angle) * particles_storage[i].speed);
					particles_storage[i].position.y += (sin(angle) * particles_storage[i].speed);

					// Decelerate at the end 
					if (0.6f >= life_percentage && life_percentage > 0.50f) { // Accelerate
						particles_storage[i].speed -= particles_storage[i].speed * delta_time;
					}
				}
				else {
					particles_storage[i].position.x -= (cos(angle) * particles_storage[i].speed);
					particles_storage[i].position.y -= (sin(angle) * particles_storage[i].speed);

					// Accelerate at the start
					if (0.5f >= life_percentage && life_percentage > 0.4f) { // Decelerate
						particles_storage[i].speed += particles_storage[i].speed * delta_time;
					}
				}

				// Rotation for lava particle
				if (particles_storage[i].id % 2 == 0) {
					particles_storage[i].direction += 180.0f * delta_time;
				}
				else {
					particles_storage[i].direction -= 180.0f * delta_time;
				}

			}
		}


	}

	// This creates and sets the parameters needed to emit particles for an event
	void Particle_System::particle_emit(std::string type, Vec2D pos, Vec3D col,
		float lifespan, float direction) { // Optional parameters

		// Create particles if there are space
		if (active_particles < MAX_PARTICLES) {

			// Set particle data according to type
			particles_storage[active_particles] = particle_base[type];

			// Set particle lifespan (if given)
			if (lifespan > 0.0f) {
				particles_storage[active_particles].life_left = lifespan;
				particles_storage[active_particles].life_span = lifespan;
			}

			// Set particle direction (if given)
			if (direction > -1.0f) {
				particles_storage[active_particles].direction = direction;
			}

			// Set particle position 
			particles_storage[active_particles].position = pos;

			// Set particle color
			particles_storage[active_particles].color = col;

			// Set particle id
			particles_storage[active_particles].id = active_particles;

			// Special case for ship_takeoff_smoke's speed
			if (particles_storage[active_particles].type == ship_takeoff_smoke) {
				float offset = std::fabs(600.0f - particles_storage[active_particles].position.x);
				float temp_speed = particles_storage[active_particles].speed;
				particles_storage[active_particles].speed = offset / 30.0f * temp_speed;
			}

			// Increment active particles count
			active_particles++;

			LM.write_log("Active particles count now is %d after adding particle at pos: %f, %f", active_particles, pos.x, pos.y);
		}
		else {
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
