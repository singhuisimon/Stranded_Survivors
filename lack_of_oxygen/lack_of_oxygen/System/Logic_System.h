#pragma once
#ifndef LOF_LOGIC_SYSTEM_H
#define LOF_LOGIC_SYSTEM_H

#include "System.h"
#include <unordered_map>
#include "../Component/Component.h"
#include "../Utility/BehaviourFCT.h"
#include "../Manager/ECS_Manager.h"
#include "../Manager/Input_Manager.h"

namespace lof {
	class Logic_System : public System {

	public:

		Logic_System();

		bool initialize();
		void update(float delta_time) override;
		void shutdown();

		~Logic_System(); //destroy/delete all the BehaviourFCT* object

		//void AddBehaviour(Behaviour_FCT* behaviour);

		void move_left_behaviour(Entity* entity);

		std::string get_type() const override;

	private:	
		
		//containers for the behaviour and the behaviour fct (essentially the script + contains the funcition pointers)
		//std::vector<Behaviour_FCT*> m_behaviours; //can use the script multiple times //causing error unidentified identifier
		std::vector<BehaviourComponent> m_behaviour_components;

	};
}

#endif