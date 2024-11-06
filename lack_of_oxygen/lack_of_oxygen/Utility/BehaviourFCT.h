#pragma once

#ifndef _BEHAVIOUR_FCT_H_
#define _BEHAVIOUR_FCT_H_

#include "../System/Logic_System.h"
#include "../Utility/Type.h"
#include <functional>
#include "../Entity/Entity.h"
#include "../Manager/Input_Manager.h"
#include "../Manager/ECS_Manager.h"
#include "../Component/Component.h"

namespace lof {
	typedef std::function<void(Entity*)> InitBehaviour;
	typedef std::function<void(Entity*)> UpdateBehaviour;
	typedef std::function<void(Entity*)> EndBehaviour;

	class Behaviour_FCT {
	private:
		//3 global functoin pointers that define a script (on the game code side)
		//this is a containment method, trying to avoid inheritance and vtables
		//vs making these 3fct as virtual=() and forcing users to derive from
		//this class is to make custom scripts, on the game code side

		InitBehaviour m_init_behaviour;
		UpdateBehaviour m_update_behaviour;
		EndBehaviour m_end_behaviour;

		//....


	public:
		Behaviour_FCT(const InitBehaviour& init, const UpdateBehaviour& update, const EndBehaviour& end);

		void init(Entity* entity);

		void update(Entity* entity);

		void end(Entity* entity);

		void custom_update(Entity* entity);

		//was also from chat gpt
		//void movement_update(Entity* entity) {

		//	EntityID entityid = entity->get_id();
		//	if (entity && ECSM.has_component<Physics_Component>(entityid)) {
		//		auto& physics = ECSM.get_component<Physics_Component>(entityid);

		//		if (entityid == 5) {
		//			//movement controls
		//			if (IM.is_key_held(GLFW_KEY_A)) {
		//				physics.apply_force(Vec2D(-5000.f, 0.0f));
		//				if (physics.is_grounded) {
		//					ECSM.get_component<Audio_Component>(entityid).set_audio_state("moving left", PLAYING); //movement sound
		//				}
		//			}
		//			else if (IM.is_key_held(GLFW_KEY_D)) {
		//				physics.apply_force(Vec2D(5000.0f, 0.0f));
		//				if (physics.is_grounded) {
		//					ECSM.get_component<Audio_Component>(entityid).set_audio_state("moving right", PLAYING); //movement sound
		//				}
		//			}
		//			else if (IM.is_key_held(GLFW_KEY_SPACE) && physics.is_grounded) {
		//				physics.apply_force(Vec2D(0.0f, physics.jump_force * physics.mass));
		//				physics.is_grounded = false;
		//				physics.gravity.y = DEFAULT_GRAVITY;
		//				ECSM.get_component<Audio_Component>(entityid).set_audio_state("jumping", PLAYING);  //jumping sound
		//			}

		//			if (!physics.is_grounded) {
		//				physics.apply_force(physics.gravity * physics.mass);
		//			}
		//		}
		//	}
		//	LM.write_log("movement behaviour update");
		//}
	};
}





#endif