#include "Force_Manager.h"


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

	//Force Manager 

	void Force_Manager::add_force(const Force& force) {
		forces.push_back(force);
	}



	void Force_Manager::activate_force(ForceType type) {
		for (auto& force : forces) {
			if (force.type == type) {
				force.set_active(true);
				break;
			}
		}
	}

	void Force_Manager::deactivate_force(ForceType type) {
		for (auto& force : forces) {
			if (force.type == type) {
				force.set_active(false);
				break;
			}
		}
	}


	void Force_Manager::update_force(float delta_time) {

		for (auto& force : forces) {
			force.update_Age(delta_time);
		}

	}

	Vec2D Force_Manager::get_resultant_Force() const {

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
	Force_Manager::~Force_Manager() {
		forces.clear();
	}

	void Force_Manager::clear() {
		forces.clear();
	}

}//namespace lof