#include "BehaviourFCT.h"


namespace lof {

	Behaviour_FCT::Behaviour_FCT(const InitBehaviour& init, const UpdateBehaviour& update, const EndBehaviour& end) :
		m_init_behaviour(init), m_update_behaviour(update), m_end_behaviour(end) {
		//logic_system.addbehaviour(this);
	}

	void Behaviour_FCT::init(Entity* entity) {
		if (m_init_behaviour) {
			m_init_behaviour(entity);
		}
	}

	void Behaviour_FCT::update(Entity* entity) {
		if (m_update_behaviour) {
			m_update_behaviour(entity);
		}
	}

	void Behaviour_FCT::end(Entity* entity) {
		if (m_end_behaviour) {
			m_end_behaviour(entity);
		}
	}

	void Behaviour_FCT::custom_update(Entity* entity) {
		//movement_update(entity);
	}
}