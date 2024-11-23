#include "Entity_Selector_System.h"
#include "../Manager/ECS_Manager.h"
#include "../Main/Main.h" // for extern window
#include <iostream>
#include "../Manager/IMGUI_Manager.h"



namespace lof 
{
    std::unique_ptr<Entity_Selector_System> Entity_Selector_System::instance;
    std::once_flag Entity_Selector_System::once_flag;

    EntityInfo Entity_Selector_System::g_selected_entity_info;

    Entity_Selector_System& Entity_Selector_System::get_instance() {
        std::call_once(once_flag, []() {
            instance.reset(new Entity_Selector_System);
            });
        return *instance;
    }

    EntityInfo& Entity_Selector_System::get_selected_entity_info() {
        return g_selected_entity_info;
    }

#if 1
    void Entity_Selector_System::Check_Selected_Entity()
    {
        bool entitySelected = false;
        EntityID selectedEntityID = -1;

        const auto& entities = ECSM.get_entities();

        for (const auto& entity : entities)
        {
            EntityID entityID = entity->get_id();

            
            if (!ECSM.has_component<Transform2D>(entityID) ){
                //sstd::cout << "Entity " << entityID << " does not have the required components.\n";
                continue;
            }

            if (!ECSM.has_component<Collision_Component>(entityID)) {
               // std::cout << "Entity " << entityID << " does not have a Collision_Component.\n";
                continue;
            }

            // std::cout << "Testing what is this entity id in entity selector system " << entityID << "\n";
            auto& transform = ECSM.get_component<Transform2D>(entityID);
            auto& collision = ECSM.get_component<Collision_Component>(entityID);

            float entityX = transform.position.x;
            float entityY = transform.position.y;
            float entityWidth = collision.width;
            float entityHeight = collision.height;

           
           

            Update_Selected_Entity_Info(entityID, entityX, entityY, entityWidth, entityHeight);
            EntityInfo selectedInfo = g_selected_entity_info;

            if (selectedInfo.isSelected) {
                entitySelected = true;
                selectedEntityID = selectedInfo.selectedEntity;  // Store the selected entity ID
                //std::cout << "x pos of the entity " << entityX << " y pos " << entityY << "\n";
                //std::cout << " yees !!! is selected in entitty selector system  " << selectedEntityID << "\n";
                //std::cout << "mouse x: " << selectedInfo.mousePos.x << " mouse y: " << selectedInfo.mousePos.y << "\n\n";
                break;  // Exit the loop early if an entity is selected 
            }

        }
    }
#endif

    
#if 1
    void Entity_Selector_System::Update_Selected_Entity_Info(EntityID entityID, float entityX, float entityY, float entityWidth, float entityHeight)
    {

        ImVec2 mousePos = IMGUIM.imgui_mouse_pos();
        
        g_selected_entity_info.mousePos = mousePos;

        bool isSelected = Mouse_Over_AABB(entityX, entityY, entityWidth, entityHeight, mousePos.x, mousePos.y);
        g_selected_entity_info.isSelected = isSelected;

        if (g_selected_entity_info.isSelected) {
            //std::cout << "Entity " << entityID << " is selected!\n";
            g_selected_entity_info.selectedEntity = entityID;  // Store the selected entity's ID
        }
        else {
            g_selected_entity_info.selectedEntity = static_cast<EntityID>(-1);  // no entity is being selected
        }


    }

#endif

#if 0
    bool Entity_Selector_System::Update_Selected_Entity_Info(EntityID entityID, float entityX, float entityY, float entityWidth, float entityHeight)
    {
        Vec2D mousePos = Get_World_MousePos();
        g_selected_entity_info.mousePos = mousePos;

        bool isSelected = Mouse_Over_AABB(entityX, entityY, entityWidth, entityHeight, mousePos.x, mousePos.y);

        if (isSelected) {
            g_selected_entity_info.selectedEntity = entityID;  // Store the selected entity's ID
            return true; // Return true if the entity is selected
        }
        else {
            g_selected_entity_info.selectedEntity = static_cast<EntityID>(-1);  // No entity is being selected
            return false; // Return false if no entity is selected
        }
    }
#endif

    Vec2D Entity_Selector_System::Get_World_MousePos()
    {

        if (!window)
        {
            return Vec2D(0.0f, 0.0f);
        }


        double screen_width = SM.get_scr_width();
        double screen_height = SM.get_scr_height(); //1020 /2
        //std::cout << "scene width: " << SM.get_scr_width() << " scene height: " << SM.get_scr_height() << "\n";
        // 1980 x 1020

        
        double mouse_x, mouse_y;
        glfwGetCursorPos(window, &mouse_x, &mouse_y);
        mouse_x -= (screen_width / 2);
        mouse_y -= (screen_height / 2);
        mouse_x = mouse_x;
        mouse_y = mouse_y;
        mouse_y = -mouse_y;
       /* int framebuffer_width, framebuffer_height;
        glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);

        int window_width, window_height;
        glfwGetWindowSize(window, &window_width, &window_height);

        double mouse_x, mouse_y;
        glfwGetCursorPos(window, &mouse_x, &mouse_y);

        double x_scale = static_cast<double>(framebuffer_width) / window_width;
        double y_scale = static_cast<double>(framebuffer_height) / window_height;

        mouse_x *= x_scale;
        mouse_y *= y_scale;

        double screen_width = framebuffer_width;
        double screen_height = framebuffer_height;

        mouse_x -= (screen_width / 2);
        mouse_y -= (screen_height / 2);
        mouse_y = -mouse_y;*/
    
        // Get camera position
        auto& camera = GFXM.get_camera();


        // Convert screen coordinates to world coordinates by adding camera position
        double world_x = mouse_x + camera.pos_x;
        double world_y = mouse_y + camera.pos_y;
        //std::cout << "cursor position x: " << world_x << " " << "cursor position y: " << world_y << "\n";
        return Vec2D(static_cast<float>(world_x), static_cast<float>(world_y));

    }

  
    bool Entity_Selector_System::Mouse_Over_AABB(float box_x, float box_y, float width, float height, int mouseX, int mouseY)
    {
        return (mouseX > (box_x - width / 2.0f) && mouseX < (box_x + width / 2.0f) &&
            mouseY >(box_y - height / 2.0f) && mouseY < (box_y + height / 2.0f));
    }

  
}