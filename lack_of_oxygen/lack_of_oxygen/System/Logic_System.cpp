#include "Logic_System.h"

namespace lof {

	Logic_System::Logic_System() {
		signature.set(ECSM.get_component_id<BehaviourComponent>());
	}

	bool Logic_System::initialize() {



		return true;
	}

	void Logic_System::update(float delta_time) {
		for (auto& entity_ptr : ECSM.get_entities()) {
			Entity* entity = entity_ptr.get();
			EntityID entityid = entity->get_id();

			if (ECSM.has_component<BehaviourComponent>(entityid)) {
				auto& behaviour_component = ECSM.get_component<BehaviourComponent>(entityid);
				unsigned int behaviourindex = behaviour_component.get_behaviour_index();

				/*if (behaviourindex < m_behaviours.size()) {
					m_behaviours[behaviourindex]->update(entity);
				}*/
			}
		}
	}
	void Logic_System::shutdown() {
		//m_behaviours.clear();
		m_behaviour_components.clear();
	}

	Logic_System::~Logic_System() {
		//m_behaviours.clear();
		m_behaviour_components.clear();
	}

	/*void Logic_System::AddBehaviour(Behaviour_FCT* behaviour) {
		m_behaviours.push_back(behaviour);
	}*/

	//from chatgpt 
	void Logic_System::move_left_behaviour(Entity* entity) {		
		
		auto left_behaviour = std::make_unique<Behaviour_FCT>(
			[](Entity* entity) {
				//init code
			},
			[](Entity* entity) {

				if (ECSM.has_component<Physics_Component>(entity->get_id())) {
					auto& physics = ECSM.get_component<Physics_Component>(entity->get_id());

					//movement logic 
					physics.apply_force(Vec2D(-5000.f, 0.0f));
					
					//set audio riger
					if (physics.is_grounded) {
						ECSM.get_component<Audio_Component>(entity->get_id()).set_audio_state("moving left", PLAYING); //movement sound
					}
				}

				//entity->get_behaviour()->movement_update()
				//what we need to do here is check for if it has behaviour component
				//once done retrieve it and thereafter see if can link to 
				//behaviour fct which will allow us to use movement update
				//maybe using movement_behaviour.
			},
			[](Entity* entity) {
				//clean up logic if needed
			}
		);

		//AddBehaviour(left_behaviour);
		//entity->setbehaviour(0, std::move(movement_behaviour));
	}

	std::string Logic_System::get_type() const {
		return "Logic_System";
	}

}