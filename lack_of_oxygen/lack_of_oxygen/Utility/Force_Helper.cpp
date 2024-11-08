/**
 * @file Force_Helper.cpp
 * @brief Implements the Force and Force_Helper classes used for managing forces in a physics simulation.
 *
 * This file contains the definitions of the functions declared in `Force_Helper.h`:
 *
 * Force Class Methods**: Implements functions to update the age of a force, manage its
 *   activation state, set its lifetime, and check for expiration.
 *
 * Force_Helper Class Methods**: Implements functions to add forces, activate and deactivate
 *   forces by type, update forces over time, compute the resultant force, and clear the stored
 *   forces.
 *
 * @author Wai Lwin Thit (100%)
 * @date November 8, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */


#include "Force_Helper.h"

namespace lof {

	//Force Class
	Force::Force(Vec2D direction, ForceType type, float magnitude,
		float lifetime) :
		direction(direction), type(type), magnitude(magnitude),
		lifetime(lifetime), age(0.0f), is_active(false)
	{}

	void Force::update_Age(float delta_time) {
		if (is_active && lifetime > 0.0f) {
			age += delta_time;
			if (age >= lifetime) {
				is_active = false;
				age = 0.0f; //reset age
			}
		}
	}


	void Force::set_active(bool active) {
		is_active = active;
	}

	void Force::set_lifetime(float life) {
		lifetime = life;
	}

	bool Force::isExpired() const {
		return lifetime > 0.0f && age >= lifetime;
	}

	//Force Helper

	void Force_Helper::add_force(const Force& force) {
		forces.push_back(force);
	}



	void Force_Helper::activate_force(ForceType type) {
		for (auto& force : forces) {
			if (force.type == type) {
				force.set_active(true);
				break;
			}
		}
	}

	void Force_Helper::deactivate_force(ForceType type) {
		for (auto& force : forces) {
			if (force.type == type) {
				force.set_active(false);
				break;
			}
		}
	}


	void Force_Helper::update_force(float delta_time) {

		for (auto& force : forces) {
			force.update_Age(delta_time);
		}

	}

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
	Force_Helper::~Force_Helper() {
		forces.clear();
	}

	void Force_Helper::clear() {
		forces.clear();
	}

}//namespace lof