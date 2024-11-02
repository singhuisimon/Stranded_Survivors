//#pragma once
//#include "System.h"
//#include "../Manager/ECS_Manager.h"
//#include <algorithm> // For std::clamp
//#include "../Utility/Type.h"
//
//namespace lof {
//    class GUI_System : public System {
//    private:
//        ECS_Manager& ecs_manager;
//        EntityID container_id;     // Main container entity
//        EntityID progress_bar_id;  // Progress bar entity
//        EntityID left_image_id;    // Left image entity
//        EntityID right_image_id;   // Right image entity
//
//    public:
//        GUI_System(ECS_Manager& ecs_manager) : ecs_manager(ecs_manager) {
//            container_id = INVALID_ENTITY_ID;
//            progress_bar_id = INVALID_ENTITY_ID;
//            left_image_id = INVALID_ENTITY_ID;
//            right_image_id = INVALID_ENTITY_ID;
//
//            // Set up the required components
//            signature.set(ecs_manager.get_component_id<Transform2D>());
//            signature.set(ecs_manager.get_component_id<Graphics_Component>());
//            signature.set(ecs_manager.get_component_id<GUI_Component>());
//        }
//
//        void update(float delta_time) override;
//        std::string get_type() const override;
//
//        void show_loading_screen();
//        void hide_loading_screen();
//        void set_progress(float progress);
//    };
//}