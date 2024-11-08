#pragma once
/**
 * @file Force_Helper.h
 * @brief Defines the Force and Force_Helper classes used to manage forces in a physics simulation.
 *
 * This file provides the declarations for the Force and Force_Helper classes within the `lof` namespace.
 *
 * - **Force Class**: Represents an individual force, characterized by a direction, type, magnitude,
 *   and optional lifetime. The class provides methods for managing the force’s activity status,
 *   lifetime, and expiration.
 *
 * - **Force_Helper Class**: Manages a collection of Force objects, allowing for the addition,
 *   activation, deactivation, and updating of forces. It also provides a method for computing the
 *   resultant force vector from all active forces.
 *
 * @author Wai Lwin Thit (100%)
 * @date November 8, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */


#ifndef LOF_FORCE_HELPER_H
#define LOF_FORCE_HELPER_H

#include "Vector2D.h"
#include <map>
#include <vector>
#include <iostream>

namespace lof {

	enum ForceType {
		MOVE_LEFT,
		MOVE_RIGHT,
		JUMP_UP,
		DRAG,
		IMPULSE //for future purposes
	};

	/**
	 * @class Force
	 * @brief Represents a force with specific characteristics like direction, type, magnitude, and lifetime.
	 *
	 * This class models a force that can be applied to objects within a physics simulation. Each force has
	 * a direction, a type (e.g., MOVE_LEFT, JUMP_UP), a magnitude, and an optional lifetime.
	 */
	class Force {

	public:

		Vec2D direction;
		ForceType type;
		float magnitude;
		float lifetime;
		float age = 0.0f;
		bool is_active = false;

	public:
		/**
		 * @brief Constructs a Force object with specified direction, type, magnitude, and lifetime.
		 * @param direction The direction vector of the force.
		 * @param type The type of the force.
		 * @param magnitude The magnitude of the force.
		 * @param lifetime The active duration of the force (0.0 for infinite lifetime).
		 */
		Force(Vec2D direction, ForceType type, float magnitude,
			float lifetime);

		/**
		 * @brief Updates the age of the force over time and deactivates it if it expires.
		 * @param delta_time The time step by which to increment the age.
		 */
		void update_Age(float delta_time);

		/**
		 * @brief Activates or deactivates the force.
		 * @param active Boolean indicating if the force should be active.
		 */
		void set_active(bool active);

		/**
		 * @brief Sets a new lifetime for the force.
		 * @param life The new lifetime in seconds.
		 */
		void set_lifetime(float life);

		/**
		 * @brief Checks if the force has expired (i.e., exceeded its lifetime).
		 * @return True if the force has expired, false otherwise.
		 */
		bool isExpired() const;

		/**
		 * @brief Converts a string to its corresponding ForceType enumeration.
		 * @param type_str The string representing the force type.
		 * @return The matching ForceType enum.
		 */
		static ForceType string_to_ftype(const std::string& type_str) {
			if (type_str == "MOVE_LEFT") return MOVE_LEFT;
			if (type_str == "MOVE_RIGHT") return MOVE_RIGHT;
			if (type_str == "JUMP_UP") return JUMP_UP;
			if (type_str == "DRAG") return DRAG;
			if (type_str == "IMPULSE") return IMPULSE;
		}

		/**
		 * @brief Converts a ForceType enumeration to its string representation.
		 * @param type The ForceType enum to convert.
		 * @return The string representation of the force type.
		 */
		static std::string ftype_to_string(const ForceType type) {
			std::string type_str;

			switch (type) {
			case MOVE_LEFT:
				type_str = "MOVE_LEFT";
				break;
			case MOVE_RIGHT:
				type_str = "MOVE_RIGHT";
				break;
			case JUMP_UP:
				type_str = "JUMP_UP";
				break;
			case DRAG:
				type_str = "DRAG";
				break;
			case IMPULSE:
				type_str = "IMPULSE";
				break;
			default:
				type_str = "UNKNOWN";
				break;
			}

			return type_str;

		}

	};

	/**
	 * @class Force_Helper
	 * @brief Manages a collection of forces applied to objects in a physics simulation.
	 *
	 * The Force_Helper class is responsible for handling a set of Force objects, including
	 * adding new forces, updating their state, activating/deactivating specific force types,
	 * and calculating the resultant force vector.
	 */
	class Force_Helper
	{

	public:
		/**
		 * @brief Destructor that clears all managed forces.
		 */
		~Force_Helper();

		/**
		 * @brief Adds a new Force to the manager.
		 * @param force The Force object to add.
		 */
		void add_force(const Force& force);

		/**
		 * @brief Updates all forces, incrementing their age if active.
		 * @param delta_time The time increment for updating force ages.
		 */
		void update_force(float delta_time);

		/**
		 * @brief Activates all forces of a specified type.
		 * @param type The type of force to activate.
		 */
		void activate_force(ForceType type);

		/**
		 * @brief Deactivates all forces of a specified type.
		 * @param type The type of force to deactivate.
		 */
		void deactivate_force(ForceType type);

		/**
		 * @brief Clears all forces from the manager.
		 */
		void clear();

		/**
		 * @brief Computes the resultant force from all active forces.
		 * @return The resultant force as a Vec2D vector.
		 */		Vec2D get_resultant_Force() const;

		const std::vector<Force>& get_forces() const { return forces; }

	private:
		std::vector<Force> forces; /**< A vector storing all the forces managed by this class. */
	};

} //endof Namespace lof

#endif // !Force_Helper_H

