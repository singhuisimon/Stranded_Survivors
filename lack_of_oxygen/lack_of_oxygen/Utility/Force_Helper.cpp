/**
 * @file Force_Helper.cpp
 * 
 * @brief Implementation of classes and methods for managing forces in a physics simulation, 
 * specifically the `Force` and `Force_Helper` classes.
 *
 * This file contains the functions used to handle individual forces and a collection
 * of forces, enabling the application, activation, deactivation, and management of forces in a game physics context.
 *  *
 * @author Wai Lwin Thit (100%)
 * @date November 8, 2024
 * 
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */


#include "Force_Helper.h"

namespace lof {

	//Force Class
	/**
	 * @brief Constructor for Force class.
	 *
	 * Initializes a Force with a direction, type, magnitude, and lifetime.
	 *
	 * @param direction Direction vector of the force.
	 * @param type Enum representing the type of force.
	 * @param magnitude The magnitude of the force.
	 * @param lifetime The lifespan of the force.
	 */
	Force::Force(Vec2D direction, ForceType type, float magnitude,
		float lifetime) :
		direction(direction), type(type), magnitude(magnitude),
		lifetime(lifetime), age(0.0f), is_active(false)
	{}

	/**
	 * @brief Updates the age of the force based on delta time.
	 *
	 * Checks if the force is active and updates its age, resetting it if lifetime is exceeded.
	 *
	 * @param delta_time Time elapsed since the last update.
	 */	
	void Force::update_Age(float delta_time) {
		if (is_active && lifetime > 0.0f) {
			age += delta_time;
			if (age >= lifetime) {
				is_active = false;
				age = 0.0f; //reset age
			}
		}
	}


	/**
	 * @brief Sets the active state of the force.
	 *
	 * @param active Boolean value to activate or deactivate the force.
	 */
	void Force::set_active(bool active) {
		is_active = active;
	}

	/**
	 * @brief Sets the lifetime of the force.
	 *
	 * @param life New lifetime for the force.
	 */	
	void Force::set_lifetime(float life) {
		lifetime = life;
	}

	/**
	 * @brief Checks if the force has expired based on its age and lifetime.
	 *
	 * @return true if the force has expired, false otherwise.
	 */	
	bool Force::isExpired() const {
		return lifetime > 0.0f && age >= lifetime;
	}

	//Force Helper
	/**
	 * @brief Adds a new force to the force list.
	 *
	 * @param force The force to be added.
	 */	
	void Force_Helper::add_force(const Force& force) {
		forces.push_back(force);
	}

	/**
	 * @brief Activates a force of a given type.
	 *
	 * @param type The type of force to activate.
	 */
	void Force_Helper::activate_force(ForceType type) {
		for (auto& force : forces) {
			if (force.type == type) {
				force.set_active(true);
				break;
			}
		}
	}
	/**
	 * @brief Deactivates a force of a given type.
	 *
	 * @param type The type of force to deactivate.
	 */
	void Force_Helper::deactivate_force(ForceType type) {
		for (auto& force : forces) {
			if (force.type == type) {
				force.set_active(false);
				break;
			}
		}
	}

	/**
	 * @brief Updates the age of each force in the helper based on delta time.
	 *
	 * @param delta_time Time elapsed since the last update.
	 */
	void Force_Helper::update_force(float delta_time) {

		for (auto& force : forces) {
			force.update_Age(delta_time);
		}

	}
	/**
	 * @brief Computes and returns the resultant force from all active forces.
	 *
	 * @return Vec2D Resultant force vector.
	 */
	Vec2D Force_Helper::get_resultant_Force() const {

		Vec2D resultant(0.0, 0.0);
		for (const auto& force : forces) {
			if (force.is_active) {
				resultant += force.direction * force.magnitude;
				std::cout << "Activate force type: " << Force::ftype_to_string(force.type)
					<< " Direction: " << force.direction.x << "," << force.direction.y
					<< " Magnitude: " << force.magnitude << std::endl;
			}
		}

		return resultant;

	}

	/**
	 * @brief Destructor for the Force_Helper class.
	 *
	 * Clears the list of forces upon destruction.
	 */
	Force_Helper::~Force_Helper() {
		forces.clear();
	}

	/**
	 * @brief Clears all forces from the helper.
	 */
	void Force_Helper::clear() {
		forces.clear();
	}

}//namespace lof