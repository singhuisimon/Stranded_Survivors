//#include "GUI_System.h"
//#include "../Component/Component.h"
//#include "../Manager/Input_Manager.h"
//
//namespace lof {
//    GUI_System::GUI_System(ECS_Manager& ecs_manager)
//        : ecs_manager(ecs_manager)
//        , container_id(INVALID_ENTITY_ID)
//        , progress_bar_id(INVALID_ENTITY_ID)
//        , left_image_id(INVALID_ENTITY_ID)
//        , right_image_id(INVALID_ENTITY_ID) {}
//
//    void GUI_System::show_loading_screen() {
//        // Create container box
//        container_id = ecs_manager.clone_entity_from_prefab("gui_container");
//        if (container_id != INVALID_ENTITY_ID) {
//            auto& container_gui = ecs_manager.get_component<GUI_Component>(container_id);
//            container_gui.is_container = true;
//
//            // Create progress bar
//            progress_bar_id = ecs_manager.clone_entity_from_prefab("gui_progress_bar");
//            if (progress_bar_id != INVALID_ENTITY_ID) {
//                auto& progress_gui = ecs_manager.get_component<GUI_Component>(progress_bar_id);
//                progress_gui.is_progress_bar = true;
//                progress_gui.progress = 0.0f;
//
//                // Set progress bar position relative to container
//                auto& progress_transform = ecs_manager.get_component<Transform2D>(progress_bar_id);
//                progress_transform.position = Vec2D(0.0f, -50.0f); // Offset below center
//            }
//
//            // Create left image
//            left_image_id = ecs_manager.clone_entity_from_prefab("gui_image");
//            if (left_image_id != INVALID_ENTITY_ID) {
//                auto& left_transform = ecs_manager.get_component<Transform2D>(left_image_id);
//                left_transform.position = Vec2D(-100.0f, 50.0f); // Offset left and up
//            }
//
//            // Create right image
//            right_image_id = ecs_manager.clone_entity_from_prefab("gui_image");
//            if (right_image_id != INVALID_ENTITY_ID) {
//                auto& right_transform = ecs_manager.get_component<Transform2D>(right_image_id);
//                right_transform.position = Vec2D(100.0f, 50.0f); // Offset right and up
//            }
//        }
//    }
//
//    void GUI_System::hide_loading_screen() {
//        // Add logic to remove entities when implemented in ECS
//        container_id = INVALID_ENTITY_ID;
//        progress_bar_id = INVALID_ENTITY_ID;
//        left_image_id = INVALID_ENTITY_ID;
//        right_image_id = INVALID_ENTITY_ID;
//    }
//
//    void GUI_System::set_progress(float progress) {
//        if (progress_bar_id != INVALID_ENTITY_ID) {
//            auto& gui = ecs_manager.get_component<GUI_Component>(progress_bar_id);
//            gui.progress = std::clamp(progress, 0.0f, 1.0f);
//
//            // Update progress bar scale based on progress
//            auto& transform = ecs_manager.get_component<Transform2D>(progress_bar_id);
//            transform.scale.x = gui.progress;
//        }
//    }
//
//    void GUI_System::update(float delta_time) {
//        // Example: Press G to show/hide loading screen
//        if (IM.is_key_pressed(GLFW_KEY_G)) {
//            if (container_id == INVALID_ENTITY_ID) {
//                show_loading_screen();
//            }
//            else {
//                hide_loading_screen();
//            }
//        }
//
//        // Update positions of GUI elements relative to container
//        if (container_id != INVALID_ENTITY_ID) {
//            auto& container_transform = ecs_manager.get_component<Transform2D>(container_id);
//
//            // Update progress bar position
//            if (progress_bar_id != INVALID_ENTITY_ID) {
//                auto& progress_transform = ecs_manager.get_component<Transform2D>(progress_bar_id);
//                auto& progress_gui = ecs_manager.get_component<GUI_Component>(progress_bar_id);
//                progress_transform.position = container_transform.position + progress_gui.relative_pos;
//            }
//
//            // Update image positions
//            if (left_image_id != INVALID_ENTITY_ID) {
//                auto& left_transform = ecs_manager.get_component<Transform2D>(left_image_id);
//                auto& left_gui = ecs_manager.get_component<GUI_Component>(left_image_id);
//                left_transform.position = container_transform.position + left_gui.relative_pos;
//            }
//
//            if (right_image_id != INVALID_ENTITY_ID) {
//                auto& right_transform = ecs_manager.get_component<Transform2D>(right_image_id);
//                auto& right_gui = ecs_manager.get_component<GUI_Component>(right_image_id);
//                right_transform.position = container_transform.position + right_gui.relative_pos;
//            }
//        }
//    }
//
//    std::string GUI_System::get_type() const {
//        return "GUI_System";
//    }
//}