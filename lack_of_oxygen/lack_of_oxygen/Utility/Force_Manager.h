#pragma once


#ifndef LOF_FORCE_MANAGER_H
#define LOF_FORCE_MANAGER_H

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

	class Force {

	public:

		Vec2D direction;
		ForceType type;
		float magnitude;
		float lifetime;
		float age = 0.0f;
		bool is_active = false;

	public:
		//constructor
		Force(Vec2D direction, ForceType type, float magnitude,
			float lifetime);

		//update the force's age according to time
		void update_Age(float delta_time);

		//seting the force to activate or deactivate
		void set_active(bool active);

		//set the life time for the force
		void set_lifetime(float life);


		//checking if the force is expired or not
		bool isExpired() const;

		//utility function to help convert the string to ForceType
		static ForceType string_to_ftype(const std::string& type_str) {
			if (type_str == "MOVE_LEFT") return MOVE_LEFT;
			if (type_str == "MOVE_RIGHT") return MOVE_RIGHT;
			if (type_str == "JUMP_UP") return JUMP_UP;
			if (type_str == "DRAG") return DRAG;
			if (type_str == "IMPULSE") return IMPULSE;
		}
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


	class Force_Manager
	{

	public:

	
		~Force_Manager();
		void add_force(const Force& force);
		void update_force(float delta_time);
		void activate_force(ForceType type);
		void deactivate_force(ForceType type);
		void clear();

		Vec2D get_resultant_Force() const;

		const std::vector<Force>& get_forces() const { return forces; }

	private:
		std::vector<Force> forces;
	};

} //endof Namespace lof

#endif // !FORCE_MANAGER_H

