#ifndef NDEBUG
#include "Entity_Selector_Helper.h"
#include "../Manager/ECS_Manager.h"
#include "../Main/Main.h" // for extern window
#include <iostream>
#include "../Manager/IMGUI_Manager.h"
#include "../Utility/Win_Control.h"



namespace lof 
{
    std::unique_ptr<Entity_Selector_Helper> Entity_Selector_Helper::instance;
    std::once_flag Entity_Selector_Helper::once_flag;

    EntityInfo Entity_Selector_Helper::g_selected_entity_info;

    Entity_Selector_Helper& Entity_Selector_Helper::get_instance() {
        std::call_once(once_flag, []() {
            instance.reset(new Entity_Selector_Helper);
            });
        return *instance;
    }

    EntityInfo& Entity_Selector_Helper::get_selected_entity_info() {
        return g_selected_entity_info;
    }


    void Entity_Selector_Helper::Check_Selected_Entity()
    {
        bool entitySelected = false;
        EntityID selectedEntityID = static_cast<EntityID>(-1);

        const auto& entities = ECSM.get_entities();

        // a vector of entities with their sizes for sorting
        struct EntitySize {
            EntityID id;
            float size;  
        };
        std::vector<EntitySize> sortedEntities;

        // Collect all entities and their sizes
        for (const auto& entity : entities)
        {
            EntityID entityID = entity->get_id();

            if (!ECSM.has_component<Transform2D>(entityID)) {
                continue;
            }

            auto& transform = ECSM.get_component<Transform2D>(entityID);
            float width = transform.scale.x;
            float height = transform.scale.y;

            if (ECSM.has_component<Collision_Component>(entityID)) {
                auto& collision = ECSM.get_component<Collision_Component>(entityID);
                width = collision.width;
                height = collision.height;
            }

            // Calculate total area
            float area = width * height;
            sortedEntities.push_back({ entityID, area });
        }

        // Sort entities by size (smallest to largest) //lambda function
        std::sort(sortedEntities.begin(), sortedEntities.end(),
            [](const EntitySize& a, const EntitySize& b) {
                return a.size < b.size;
            });

        // Check entities from smallest to largest
        for (const auto& entitySize : sortedEntities)
        {
            EntityID entityID = entitySize.id;
            auto& transform = ECSM.get_component<Transform2D>(entityID);

            float entityX = transform.position.x;
            float entityY = transform.position.y;
            float entityWidth = transform.scale.x;
            float entityHeight = transform.scale.y;

            if (ECSM.has_component<Collision_Component>(entityID)) {
                auto& collision = ECSM.get_component<Collision_Component>(entityID);
                entityWidth = collision.width;
                entityHeight = collision.height;
            }

            
            Update_Selected_Entity_Info(entityID, entityX, entityY, entityWidth, entityHeight);

            if (g_selected_entity_info.isSelected) {
                entitySelected = true;
                selectedEntityID = entityID;
               
                break;  // Stop at first hit, which will be the smallest entity at that position
            }
        }

        // If nothing was selected, clear the selection
        if (!entitySelected) {
            g_selected_entity_info.isSelected = false;
            g_selected_entity_info.selectedEntity = static_cast<EntityID>(-1);
            
        }
    }

    
#if 1
    void Entity_Selector_Helper::Update_Selected_Entity_Info(EntityID entityID, float entityX, float entityY, float entityWidth, float entityHeight)
    {
        bool isSelected = false;
        if (level_editor_mode)
        {

            ImVec2 mousePos = IMGUIM.imgui_mouse_pos(); // for imgui
            isSelected = Mouse_Over_AABB(entityX, entityY, entityWidth, entityHeight, mousePos.x, mousePos.y);
            g_selected_entity_info.mousePos = mousePos;
        }
        else
        {
            Vec2D mousePos = Get_World_MousePos();
            isSelected = Mouse_Over_AABB(entityX, entityY, entityWidth, entityHeight, mousePos.x, mousePos.y);
            g_selected_entity_info.entitypos = mousePos;
        }
        

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


    Vec2D Entity_Selector_Helper::Get_World_MousePos()
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
   
    
        // Get camera position
        auto& camera = GFXM.get_camera();


        // Convert screen coordinates to world coordinates by adding camera position
        double world_x = mouse_x + camera.pos_x;
        double world_y = mouse_y + camera.pos_y;
        //std::cout << "cursor position x: " << world_x << " " << "cursor position y: " << world_y << "\n";
        return Vec2D(static_cast<float>(world_x), static_cast<float>(world_y));

    }

#if 1
    bool Entity_Selector_Helper::Mouse_Over_AABB(float box_x, float box_y, float width, float height, float mouseX, float mouseY)
    {
        
        unsigned int current_width =  WC.get_win_width();
        unsigned int current_height = WC.get_win_height();

        //auto & camera = GFXM.get_camera();

        //printf("window width, height (%.f, %.f)\n", current_width, current_height);
        float scaleX = static_cast<float>(current_width) / SM.get_scr_width();
        //float scaleX = static_cast<float>(SM.get_scr_width() / current_width);
        float scaleY = static_cast<float>(current_height) / SM.get_scr_height();
        //float scaleY = static_cast<float>(SM.get_scr_height() / current_height);

        float adjustX = (mouseX /scaleX);
        float adjustY = (mouseY / scaleY);

        //float camera_pos_x = camera.pos_x / scaleX;
        //float camera_pos_y = camera.pos_y / scaleY;

        //box_x -= camera_pos_x;
        //box_y -= camera_pos_y;

        //flioat adjustX = mouseX / scaleX;
        return (adjustX > (box_x - width / 2.0f) && adjustX < (box_x + width / 2.0f) &&
            adjustY >(box_y - height / 2.0f) && adjustY < (box_y + height / 2.0f));
    }
#endif


}
#endif