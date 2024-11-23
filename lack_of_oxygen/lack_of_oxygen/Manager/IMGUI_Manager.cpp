/**
 * @file IMGUI_Manager.cpp
 * @brief Declaration of the IMGUI_Manager class for running the IMGUI level editor.
 * @author Liliana Hanawardani (100%)
 * @date November 8, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

 // Include header file
#include "IMGUI_Manager.h"

// Include other managers
#include "Log_Manager.h"
#include "ECS_Manager.h"
#include "Game_Manager.h"
#include "Graphics_Manager.h"

#include <iostream>
#include <random>
#include <chrono>
#include <filesystem>
#include <vector>

// Include utility function
#include "../Utility/Constant.h"
#include "Assets_Manager.h"
#include "../System/Entity_Selector_System.h"

namespace lof {

    //constants to replace
    int selected_file_index = -1;
    int selected_object_index = -1;
    bool load_selected = false;
    bool show_window = false;
    bool remove_game_obj = false;
    bool create_game_obj = false;
    bool filled = false;

    static bool mouse_clicked_or_dragged = false;

    //mouse position in terms of game world
    ImVec2 mouse_texture_coord_world{};

    //Vector of assigned names
    std::vector<std::string> assigned_names;


    IMGUI_Manager::IMGUI_Manager() : ecs(ECSM) {}

    IMGUI_Manager::IMGUI_Manager(ECS_Manager& ecs_manager) : ecs(ecs_manager) {
        set_type("IMGUI_Manager");
    }

    IMGUI_Manager& IMGUI_Manager::get_instance() {
        static IMGUI_Manager instance;
        return instance;
    }

    //Inherited start_up, not using
    int IMGUI_Manager::start_up() {
        throw std::runtime_error("No-parameter start_up() is disabled in IMGUI_Manager. start_up() now has a parameter GLFWwindow*& window");
    }

    int IMGUI_Manager::start_up(GLFWwindow*& window) {
        if (is_started()) {
            LM.write_log("IMGUI_Manager::start_up(): Already started.");
            return 0; // Already started
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init();

        LM.write_log("IMGUI_Manager::start_up(): IMGUI_Manager started successfully.");
        return 0;
    }

    void IMGUI_Manager::display_loading_options() {

        int current_object_index = 0;

        ImGui::Begin("File List");
        selected_file_index = current_object_index;
        if (ImGui::Button("Load Scene")) {
            load_selected = !load_selected;
        }
        ImGui::End();

        if (load_selected && (selected_file_index != -1)) {
            const std::string SCENES = "Scenes";;
            if (SM.load_scene(ASM.get_full_path(SCENES, "scene1.scn").c_str())) {

                //LM.write_log("IMGUI_Manager::display_loading_options(): %s is loaded.", Path_Helper::get_scene_path());

                load_selected = !load_selected;
            }
            else {

                //LM.write_log("IMGUI_Manager::display_loading_options(): Failed to load %s.", Path_Helper::get_scene_path());

                load_selected = !load_selected;
            }
        }
    }

    void IMGUI_Manager::start_frame() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }


    //Gets mouse position in terms of game world
    ImVec2 IMGUI_Manager::get_imgui_mouse_pos(ImVec2 texture_pos, ImVec2 mouse_pos, unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT) {

        // If Mouse within texture
        if (mouse_pos.x >= texture_pos.x && mouse_pos.x <= (texture_pos.x + SCR_WIDTH / 2) &&
            mouse_pos.y >= texture_pos.y && mouse_pos.y <= (texture_pos.y + SCR_HEIGHT / 2)) {

            //Gets position of mouse in terms of viewports
            ImVec2 mouse_texture_coord_screen{};
            mouse_texture_coord_screen.x = (mouse_pos.x - texture_pos.x);
            mouse_texture_coord_screen.y = (mouse_pos.y - texture_pos.y);

            //Get camera position and changes
            auto& camera = GFXM.get_camera();

            //Gets position of mouse in terms of game world
            mouse_texture_coord_world.x = mouse_texture_coord_screen.x;
            if (mouse_texture_coord_screen.x < (SCR_WIDTH / 4)) {
                mouse_texture_coord_world.x = -((SCR_WIDTH / 4) - mouse_texture_coord_screen.x);
                mouse_texture_coord_world.x *= 2;
                mouse_texture_coord_world.x += camera.pos_x;
            }
            else {
                mouse_texture_coord_world.x = mouse_texture_coord_screen.x - (SCR_WIDTH / 4);
                mouse_texture_coord_world.x *= 2;
                mouse_texture_coord_world.x += camera.pos_x;
            }
            mouse_texture_coord_world.y = mouse_texture_coord_screen.y;
            if (mouse_texture_coord_screen.y <= (SCR_HEIGHT / 4)) {
                mouse_texture_coord_world.y = (SCR_HEIGHT / 4) - mouse_texture_coord_screen.y;
                mouse_texture_coord_world.y *= 2;
                mouse_texture_coord_world.y += camera.pos_y;
            }
            else {
                mouse_texture_coord_world.y = -(mouse_texture_coord_screen.y - (SCR_HEIGHT / 4));
                mouse_texture_coord_world.y *= 2;
                mouse_texture_coord_world.y += camera.pos_y;
            }

            //Display debug information
            /*ImGui::Text("0,0 starts at center");
            ImGui::Text("Mouse in texture at: (%.2f, %.2f)", mouse_texture_coord_world.x, mouse_texture_coord_world.y);
            ImGui::Separator();
            ImGui::Text("");
            ImGui::Text("Camera: (%.2f, %.2f)", camera.pos_x, camera.pos_y);*/

        }
        else {

            //Display debug information
            /*ImGui::Text("Mouse outside texture at: (%.2f, %.2f)", mouse_pos.x, mouse_pos);*/
        }

        //Return mouse in terms of game world
        return mouse_texture_coord_world;

    }

    ImVec2 IMGUI_Manager::imgui_mouse_pos() {
        return Mouse_Pos;
    }


    bool select_entity = false; // to ensure mouse click selected
    EntityID selectedEntityID = -1;

    //Rendering UI
    void IMGUI_Manager::render_ui(unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT) {

        //Docking and window flags
        static ImGuiDockNodeFlags docking_flags = ImGuiDockNodeFlags_None;
        static ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking
        | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        //Set up Viewport
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        ImGui::Begin("Level Editor Mode", nullptr, window_flags);

        //Set up Dockspace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), docking_flags);
        }

        //Set up Menu
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Options"))
            {
                if (ImGui::MenuItem("Flag: NoSplit", "", (docking_flags & ImGuiDockNodeFlags_NoSplit) != 0)) {
                    docking_flags ^= ImGuiDockNodeFlags_NoSplit;
                }
                if (ImGui::MenuItem("Flag: NoResize", "", (docking_flags & ImGuiDockNodeFlags_NoResize) != 0)) {
                    docking_flags ^= ImGuiDockNodeFlags_NoResize;
                }

                ImGui::Separator();
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        
        ImGui::PopStyleVar(2);
        ImGui::End();





        //Game Viewport
        if (GFXM.get_editor_mode() == 1) {

            ImGui::Begin("Game Viewport", nullptr);

            // Ensure the texture is updated before displaying
            auto texture = GFXM.get_framebuffer_texture();

            // Get the position where the texture will be rendered
            ImVec2 texture_pos = ImGui::GetCursorScreenPos();

            if (texture) {
                ImGui::Image((ImTextureID)(intptr_t)GFXM.get_framebuffer_texture(), ImVec2(SCR_WIDTH / 2, SCR_HEIGHT / 2), ImVec2(0, 1), ImVec2(1, 0));
            }

            // Get the mouse position in terms of IMGUI screen
            ImVec2 mouse_pos = ImGui::GetIO().MousePos;

            //Gets mouse position in terms of game world
            Mouse_Pos = get_imgui_mouse_pos(texture_pos, mouse_pos, SCR_WIDTH, SCR_HEIGHT);

            //Check if entity is hovered on
            ESS.Check_Selected_Entity();

            // Check if the left mouse button was pressed
            EntityInfo& selectedEntityInfo = ESS.get_selected_entity_info();




            // If Mouse within texture
            if (mouse_pos.x >= texture_pos.x && mouse_pos.x <= (texture_pos.x + SCR_WIDTH / 2) &&
                mouse_pos.y >= texture_pos.y && mouse_pos.y <= (texture_pos.y + SCR_HEIGHT / 2)) {

                //Click with  mouse
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {

                    mouse_clicked_or_dragged = true;
                    if (selectedEntityInfo.isSelected) {
                        select_entity = true;
                        selectedEntityID = selectedEntityInfo.selectedEntity;

                        std::cout << "Selected Entity ID : " << selectedEntityInfo.selectedEntity << "\n";
                        LM.write_log("Selected Entity ID system: %d", selectedEntityInfo.selectedEntity);

                    }
                    else {

                        select_entity = false;

                        selectedEntityID = selectedEntityInfo.selectedEntity;
                        std::cout << "No entity is selected.\n";
                        std::cout << "mouse position x: " << selectedEntityInfo.mousePos.x << " ,mouse position y: " << selectedEntityInfo.mousePos.y << "\n\n";

                  
                    }
                }
                else {

                    mouse_clicked_or_dragged = false;
                    select_entity = false;
                }

                if (select_entity) {
                    selected_object_index = selectedEntityID;
                }

                static ImVec2 previousMousePos;    // Previous mouse position
                static bool mouse_was_down = false;

                
                bool isDragging = false;
                Vec2D Drag_Offset;
                auto& entities = ecs.get_entities();
                ImVec2 currentMousePos = ImGui::GetMousePos();

                //calculate
                if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {

                    mouse_clicked_or_dragged = true;

                    if (!mouse_was_down) {

                        mouse_was_down = true;
                        previousMousePos = currentMousePos;

                    }
                    else {

                        // Get the mouse position in terms of IMGUI screen
                        ImVec2 mouse_pos = ImGui::GetIO().MousePos;
                        ImVec2 texture_pos = ImGui::GetCursorScreenPos();

                        //Gets mouse position in terms of game world
                        Mouse_Pos = get_imgui_mouse_pos(texture_pos, mouse_pos, SCR_WIDTH, SCR_HEIGHT);

                        
                        auto& entities_change = ecs.get_entities();

                        if (selectedEntityInfo.isSelected) {

                            if (entities_change[selectedEntityID]->has_component(ecs.get_component_id<Transform2D>())) {

                                Transform2D& transform = ecs.get_component<Transform2D>(entities_change[selectedEntityID].get()->get_id());

                                auto& position = transform.position;

                                position.x = Mouse_Pos.x;
                                position.y = Mouse_Pos.y;

                                auto& prev_position = transform.prev_position;
                                prev_position = position;

                            }
                            

                            selected_object_index = selectedEntityID;
                        }

                        /*if (selectedEntityInfo.isSelected) {
                            if (entities_change[selectedEntityID]) {

                                std::cout << "entity dragged;" << std::endl;

                                if (entities_change[selectedEntityID]->has_component(ecs.get_component_id<Transform2D>())) {
                                    std::cout << "entity has transformation component;" << std::endl;

                                    Transform2D& transform = ecs.get_component<Transform2D>(entities_change[selectedEntityID].get()->get_id());

                                    auto& position = transform.position;

                                    position.x = Mouse_Pos.x;
                                    position.y = Mouse_Pos.y;

                                    auto& prev_position = transform.prev_position;
                                    prev_position = position;

                                }
                            }

                            selected_object_index = selectedEntityID;
                        }*/

                    }

                    //selected_object_index = selectedEntityID;

                }
                else {

                    mouse_clicked_or_dragged = false;
                    mouse_was_down = false;
                }
            
            }


            ImGui::Separator();
            if (selectedEntityID == -1)
            {
                ImGui::Text("Selected Entity: None");
            }else
            {
                ImGui::Text("Selected Entity: %d", selectedEntityID);
            }
         
            ImGui::End();
        }

        //asset manager to be added later
        ImGui::Begin("Console");
        ImGui::Text("Console stuff:\nSelected Object Index: %.f\n", selected_object_index);
        ImGui::Text("selectedEntityID: %.f", selectedEntityID);
        ImGui::End();

        IMGUIM.imgui_game_objects_list();
        IMGUIM.display_loading_options();
        IMGUIM.imgui_game_objects_edit();

    }


    void IMGUI_Manager::imgui_game_objects_list() {

        ImGui::Begin("Hierarchy Object List");

        int current_object_index = 0;
        const auto& entities = ecs.get_entities();


        if (selected_object_index >= entities.size()) {
            selected_object_index = -1;
        }

        for (int i = 0; i < entities.size(); ++i) {

            if (entities[i] != nullptr) {
                std::string obj_name = entities[i]->get_name();

                //selectable for clicking; second param for highlighting
                if (ImGui::Selectable(obj_name.c_str(), selected_object_index == current_object_index) && !mouse_clicked_or_dragged) {

                    //selected; casuing seceond param state to change
                    selected_object_index = current_object_index;
                }

          


            }

            ++current_object_index;
        }

        if (ImGui::Button("Edit Game Object")) {
            show_window = !show_window;
        }

        if (ImGui::Button("Remove Game Object")) {
            remove_game_obj = !remove_game_obj;
        }

        ImGui::Text("\n\n%s", "Create Game Object From Prefab");
        if (ImGui::Button("Create Game Object From Prefab")) {

            create_game_obj = !create_game_obj;
        }

        if (ImGui::Button("Save Changes")) {
            const std::string SCENES = "Scenes";
            std::string scene_path = ASM.get_full_path(SCENES, "scene1.scn");
            if (SM.save_game_state(scene_path.c_str())) {
                //LM.write_log("IMGUI_Manager::update(): Successfully initated game state to %s", Path_Helper::get_scene_path().c_str());
                LM.write_log("IMGUI_Manager::update(): Successfully initated game state to %s");
            }
            else {
                //LM.write_log("IMGUI_Manager::update(): Failed to initate save game state to %s", Path_Helper::get_scene_path().c_str());
                LM.write_log("IMGUI_Manager::update(): Failed initated game state to %s");
            }

        }

        ImGui::End();

        if (selected_object_index != -1 && remove_game_obj && selected_object_index < entities.size()) {
            remove_game_objects(selected_object_index);
            selected_object_index = -1;
        }

        if (create_game_obj) {
            add_game_objects();
        }

    }

    static bool is_static_on = false;
    static bool is_moveable_on = true;
    static bool is_grounded_on = false;

    static bool is_active_on = true;
    static bool is_reverse_on = false;
    static bool is_rotate_on = true;;


    void IMGUI_Manager::imgui_game_objects_edit() {

        static int last_selected_object_index = -1; // Track the previous selected object index

        ImGui::Begin("Edit Object Properties");


        if (!show_window) {
            ImGui::Text("Select a game object to edit it.");
        }
        else {
            const auto& entities = ecs.get_entities();

            //for animation dropdown
            // if the selected object has changed, reset filled and clear assigned names for new object
            if (selected_object_index != last_selected_object_index) {
                last_selected_object_index = selected_object_index;
                assigned_names.clear();
                filled = false; // Reset filled to allow repopulating `assigned_names` for the new object
            }

            if (selected_object_index == -1) {
                ImGui::Text("Select a game object to edit it.");
            }
            else {

                std::string Name = entities[selected_object_index]->get_name();
                std::string condition_name_model = "Name of Entity";

                char Buffer[128]; //add to constant.h
                strncpy_s(Buffer, Name.c_str(), sizeof(Buffer));//s is safer
                Buffer[sizeof(Buffer) - 1] = '\0';

                ImGui::BeginDisabled();
                if (ImGui::InputText(condition_name_model.c_str(), Buffer, sizeof(Buffer))) {
                    std::string name = std::string(Buffer);
                    entities[selected_object_index]->set_name(name);
                }
                ImGui::EndDisabled();


                //Transform2D Component
                if (entities[selected_object_index]->has_component(ecs.get_component_id<Transform2D>())) {
                    Transform2D& transform = ecs.get_component<Transform2D>(entities[selected_object_index].get()->get_id());
                    if (ImGui::CollapsingHeader("Transformation")) {

                        //modifying values
                        auto& position = transform.position;
                        ImGui::InputFloat2("Position", &position.x);

                        auto& prev_position = transform.prev_position;
                        prev_position = position;
                        ImGui::BeginDisabled();
                        ImGui::InputFloat2("Previous Position", &prev_position.x); // Disabled input box (no editing)
                        ImGui::EndDisabled();


                        auto& orientation = transform.orientation;
                        ImGui::InputFloat2("Orientation", &orientation.x);

                        auto& scale = transform.scale;
                        ImGui::InputFloat2("Scale", &scale.x);
                    }
                }

                //Velocity Component
                if (entities[selected_object_index]->has_component(ecs.get_component_id<Velocity_Component>())) {
                    Velocity_Component& velocity_comp = ecs.get_component<Velocity_Component>(entities[selected_object_index].get()->get_id());
                    if (ImGui::CollapsingHeader("Velocity")) {

                        auto& velocity = velocity_comp.velocity;
                        ImGui::InputFloat2("Velocity", &velocity.x);

                    }
                }

                //Physics Component
                if (entities[selected_object_index]->has_component(ecs.get_component_id<Physics_Component>())) {
                    Physics_Component& physics = ecs.get_component<Physics_Component>(entities[selected_object_index].get()->get_id());
                    if (ImGui::CollapsingHeader("Physics")) {

                        auto& gravity = physics.get_gravity();
                        ImGui::InputFloat2("Gravity", &gravity.x);

                        auto& damping_factor = physics.get_damping_factor();
                        ImGui::InputFloat("Damping Factor", &damping_factor);

                        auto& max_velocity = physics.get_max_velocity();
                        ImGui::InputFloat("Maximun Velocity", &max_velocity);

                        auto& accumulated_force = physics.get_accumulated_force();
                        ImGui::InputFloat2("Accumulated Force", &accumulated_force.x);

                        auto& mass = physics.get_mass();
                        ImGui::InputFloat("Mass", &mass);

                        auto& is_static = physics.get_is_static();
                        std::string s_label = "is_static: " + std::string(is_static_on ? "On" : "Off");
                        if (button_toggle(s_label, &is_static_on)) {
                            is_static = !is_static;
                        }

                        auto& is_grounded = physics.get_is_grounded();
                        std::string g_label = "is_grounded: " + std::string(is_grounded_on ? "On" : "Off");
                        if (button_toggle(g_label, &is_grounded_on)) {
                            is_grounded = !is_grounded;
                        }

                        auto& jump = physics.get_jump_force();
                        ImGui::InputFloat("Jump Force", &jump);
                    }
                }

                //Grpahics Component
                if (entities[selected_object_index]->has_component(ecs.get_component_id<Graphics_Component>())) {
                    Graphics_Component& graphics = ecs.get_component<Graphics_Component>(entities[selected_object_index].get()->get_id());
                    if (ImGui::CollapsingHeader("Graphics")) {

                        auto& model_name = graphics.model_name;
                        std::string condition_name_m = "model_name";
                        text_input(model_name, condition_name_m);

                        auto& color = graphics.color;
                        ImGui::InputFloat3("Color", &color.x);

                        auto& texture_name = graphics.texture_name;
                        std::string condition_name_texture = "texture_name";
                        text_input(texture_name, condition_name_texture);

                        auto& shd_ref = graphics.shd_ref;
                        ImGui::BeginDisabled();
                        ImGui::InputInt("shd_ref", reinterpret_cast<int*>(&shd_ref));
                        ImGui::EndDisabled();

                    }
                }

                //Collision Component
                if (entities[selected_object_index]->has_component(ecs.get_component_id<Collision_Component>())) {
                    Collision_Component& collision = ecs.get_component<Collision_Component>(entities[selected_object_index].get()->get_id());
                    if (ImGui::CollapsingHeader("Collision")) {

                        auto& width = collision.width;
                        ImGui::InputFloat("Width", &width);

                        auto& height = collision.height;
                        ImGui::InputFloat("Height", &height);
                    }
                }

                //Animation Component
                if (entities[selected_object_index]->has_component(ecs.get_component_id<Animation_Component>())) {
                    Animation_Component& animation = ecs.get_component<Animation_Component>(entities[selected_object_index].get()->get_id());

                    if (!filled) {

                        auto& animation_list = animation.animations;
                        for (const auto& it : animation_list) {
                            assigned_names.push_back(it.second);
                        }

                        filled = true;

                    }

                    if (ImGui::CollapsingHeader("Animation") && filled) {

                        std::vector<const char*> animation_names_c_str;
                        for (const auto& name : assigned_names) {
                            animation_names_c_str.push_back(name.c_str());
                        }

                        auto& animation_list = animation.animations;
                        static std::vector<int> selected_items(animation_list.size(), -1);
                        int index = 0;
                        for (auto it = animation_list.begin(); it != animation_list.end(); ++it, ++index) {
                            if (selected_items.size() != animation_list.size()) {
                                selected_items.resize(animation_list.size(), -1);
                            }

                            ImGui::Text("Selected Animation for %i: %s", index, it->second.c_str());
                            std::string label = "Choose Animation for " + std::to_string(index);
                            if (ImGui::Combo(label.c_str(), &selected_items[index], animation_names_c_str.data(), static_cast<int>(assigned_names.size()))) {

                                // Update the specific animation in the list
                                if (selected_items[index] >= 0 && selected_items[index] < assigned_names.size()) {
                                    it->second = assigned_names[selected_items[index]];
                                }
                            }
                        }


                        auto& curr = animation.curr_animation_idx;

                        ImGui::Text("Current Animation Index: %i", curr);
                        ImGui::Text("Note: The animation index depends on movement.\n\nWhile moving, only indexes 3 and 4 can play;\nWhile stationary, only indexes 0 and 1 are allowed.\n\nIn the Level Editor, objects are stationary by default,\nso only animations 0 and 1 are available.\nIf an out - of - range index is entered, \nit snaps to 0 for even values and 1 for odd values.");
                        int temp_value = static_cast<int>(curr);
                        if (ImGui::DragInt("Current Animation Index", &temp_value, 0.1f, 0, static_cast<int>(animation_list.size()) - 1)) {

                            if (temp_value < 0) {
                                temp_value = 0;
                            }
                            else if (temp_value >= static_cast<int>(animation_list.size())) {
                                temp_value = static_cast<int>(animation_list.size()) - 1;
                            }

                            // Only update `curr_animation_idx` if temp_value is within valid bounds
                            if (temp_value >= 0 && temp_value < static_cast<int>(animation_list.size())) {
                                curr = static_cast<unsigned int>(temp_value);
                            }
                        }

                    }
                }

                //Logic Component
                if (entities[selected_object_index]->has_component(ecs.get_component_id<Logic_Component>())) {
                    Logic_Component& logic = ecs.get_component<Logic_Component>(entities[selected_object_index].get()->get_id());
                    if (ImGui::CollapsingHeader("Logic")) {

                        auto& is_active = logic.is_active;
                        std::string s_label = "is_active: " + std::string(is_active_on ? "On" : "Off");
                        if (button_toggle(s_label, &is_active_on)) {
                            is_active = !is_active;
                        }

                        auto& movement_speed = logic.movement_speed;
                        ImGui::InputFloat("Movement Speed", &movement_speed);

                        auto& movement_range = logic.movement_range;
                        ImGui::InputFloat("Movement Range", &movement_range);

                        auto& reverse_direction = logic.reverse_direction;
                        std::string reverse_dir = "reverse_direction: " + std::string(is_reverse_on ? "On" : "Off");
                        if (button_toggle(reverse_dir, &is_reverse_on)) {
                            reverse_direction = !reverse_direction;
                        }

                        auto& is_rotate = logic.rotate_with_motion;
                        std::string rotate_w_motion = "rotate_with_motion: " + std::string(is_rotate_on ? "On" : "Off");
                        if (button_toggle(rotate_w_motion, &is_rotate_on)) {
                            is_rotate = !is_rotate;
                        }

                        auto& original_position = logic.origin_pos;
                        ImGui::InputFloat2("Original Position", &original_position.x);
                    }
                }
            }
        }

        ImGui::End();
    }

    bool IMGUI_Manager::button_toggle(const std::string& boolean_name, bool* state) {


        bool clicked = ImGui::Button(boolean_name.c_str());
        if (clicked) {
            *state = !(*state);
        }

        return clicked;
    }

    void IMGUI_Manager::add_game_objects() {

        ImGui::Begin("Add Game Object", &create_game_obj);

        std::vector <const char*> prefab_names_c_str{};

        for (const std::string& name : prefab_names) {
            prefab_names_c_str.push_back(name.c_str());
        }

        const char** prefab_options = prefab_names_c_str.data();

        static int selected_item = -1;

        if (ImGui::Combo("Clone from Prefab Options", &selected_item, prefab_options, static_cast<int>(prefab_names_c_str.size()))) {

            //Simon's code
            EntityID new_entity = ECSM.clone_entity_from_prefab(prefab_options[selected_item]);
            if (new_entity != INVALID_ENTITY_ID) {
                // Generate random position
                static std::default_random_engine generator;
                static std::uniform_real_distribution<float> distribution(-2500.0f, 2500.0f);

                float random_x = distribution(generator);
                float random_y = distribution(generator);

                // Get the Transform2D component and set its position
                if (ECSM.has_component<Transform2D>(new_entity)) {
                    Transform2D& transform = ECSM.get_component<Transform2D>(new_entity);
                    transform.position.x = random_x;
                    transform.position.y = random_y;

                    LM.write_log("Game_Manager::update:Cloned entity %u at random position (%f, %f)", new_entity, random_x, random_y);
                }
                else {
                    LM.write_log("Game_Manager::update:Cloned entity %u does not have a Transform2D component.", new_entity);
                }
            }
            else {
                LM.write_log("Game_Manager::update:Failed to clone entity from prefab 'dummy_object'.");
            }
        }

        /*static const char* prefab_options[]{"player", "dummy_object", "gui_container", "gui_progress_bar", "gui_image"};
        static int selected_item = -1;
        if (ImGui::Combo("Clone from Prefab Options", &selected_item, prefab_options, IM_ARRAYSIZE(prefab_options))){
            //Simon's code
            EntityID new_entity = ECSM.clone_entity_from_prefab(prefab_options[selected_item]);
            if (new_entity != INVALID_ENTITY_ID) {
                // Generate random position
                static std::default_random_engine generator;
                static std::uniform_real_distribution<float> distribution(-2500.0f, 2500.0f);

                float random_x = distribution(generator);
                float random_y = distribution(generator);

                // Get the Transform2D component and set its position
                if (ECSM.has_component<Transform2D>(new_entity)) {
                    Transform2D& transform = ECSM.get_component<Transform2D>(new_entity);
                    transform.position.x = random_x;
                    transform.position.y = random_y;

                    LM.write_log("Game_Manager::update:Cloned entity %u at random position (%f, %f)", new_entity, random_x, random_y);
                }
                else {
                    LM.write_log("Game_Manager::update:Cloned entity %u does not have a Transform2D component.", new_entity);
                }
            }
            else {
                LM.write_log("Game_Manager::update:Failed to clone entity from prefab 'dummy_object'.");
            }
        }*/


        ImGui::End();
    }

    void IMGUI_Manager::remove_game_objects(int index) {

        const auto& entities = ecs.get_entities();
        EntityID eid = entities[index]->get_id();

        if (eid != INVALID_ENTITY_ID) {
            ECSM.destroy_entity(eid);
            remove_game_obj = !remove_game_obj;
            return;
        }


    }

    void IMGUI_Manager::text_input(std::string& data_name, std::string& codition_name) {

        char Buffer[128]; //add to constant.h
        strncpy_s(Buffer, data_name.c_str(), sizeof(Buffer));//s is safer
        Buffer[sizeof(Buffer) - 1] = '\0';

        if (ImGui::InputText(codition_name.c_str(), Buffer, sizeof(Buffer))) {
            data_name = std::string(Buffer);
        }
    }

    void IMGUI_Manager::fill_prefab_names(const char* prefab_name) {
        prefab_names.push_back(prefab_name);
    }

    void IMGUI_Manager::render() {
        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void IMGUI_Manager::shut_down() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        LM.write_log("IMGUI_Manager::shut_down(): IMGUI_Manager shut down successfully.");
    }

} // namespace lof
