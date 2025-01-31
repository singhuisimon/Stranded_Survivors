/**
 * @file IMGUI_Manager.cpp
 * @brief Declaration of the IMGUI_Manager class for running the IMGUI level editor.
 * @author Liliana Hanawardani (80%), Simon Chan (13%), Saw Hui Shan (7%)
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
#include "Assets_Manager.h"
#include "Audio_Manager.h"

// Include utility functions
#include "../Utility/Constant.h"
#include "../Utility/Entity_Selector_Helper.h"
#include "../Utility/Win_Control.h"

//Include other file headers
#include "../Component/Component.h"
#include "../System/GUI_System.h"
#include "../System/Audio_System.h" //For Audio System access

//Include standard headers
#include <iostream>
#include <random>
#include <chrono>
#include <filesystem>
#include <vector>
#include <functional>


namespace lof {

    //variables to note down indexes of files or objects
    int selected_file_index = -1;
    int prev_file_selected = -1;
    int selected_object_index = -1;

    //booleans to open IMGUI windows or execute functionalities
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

    //booleans to note down mouse behaviour
    static bool mouse_was_down = false;
    static ImVec2 mouse_pos_before_press;
    
    char to_lower(char c) {
        return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }


    IMGUI_Manager::IMGUI_Manager() : ecs(ECSM) {}

    IMGUI_Manager::IMGUI_Manager(ECS_Manager& ecs_manager) : ecs(ecs_manager){
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

    //start up
    int IMGUI_Manager::start_up(GLFWwindow*& glfwindow) {
        if (is_started()) {
            LM.write_log("IMGUI_Manager::start_up(): Already started.");
            return 0; // Already started
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(glfwindow, true);
        ImGui_ImplOpenGL3_Init();

        LM.write_log("IMGUI_Manager::start_up(): IMGUI_Manager started successfully.");
        fill_up_sound_names();

        return 0;
    }

    //fill up sound names
    void IMGUI_Manager::fill_up_sound_names() {

        //Fill in audio_types vector 
        audio_types.push_back(std::make_pair("BGM", (AudioType)0));
        audio_types.push_back(std::make_pair("SFX", (AudioType)1));
        audio_types.push_back(std::make_pair("NIL", (AudioType)2));

    }

    //file list
    void IMGUI_Manager::display_loading_options() {

        ImGui::Begin("File List");

        const std::string SCENES = "Scenes";
        std::string level_path = ASM.get_full_path(SCENES, "");
        std::vector<std::string> file_names;

        //Iterate through the directory and collect file names
        for (const auto& entry : std::filesystem::directory_iterator(level_path)) {
            
            //If the file is found, add to the list of file names
            if (entry.is_regular_file()) {  
                
                file_names.push_back(entry.path().filename().string());
            }

        }

        //Indexes for files
        int current_file_index = 0;
        int shown_file_index = -1;
        std::string selected_file{};

        //Iterate through file names
        for (int i = 0; i < file_names.size(); ++i) {

            if (!file_names[i].empty()) {

                //selectable for clicking; second param for highlighting
                if (ImGui::Selectable(file_names[i].c_str(), selected_file_index == current_file_index)) {

                    //selected; casuing seceond param state to change
                    selected_file_index = current_file_index;
                }

                if (file_names[i] == get_current_file_shown()) {
                    shown_file_index = i;
                }

            }

            ++current_file_index;
        }

        //If file is selecetd
        if (selected_file_index != -1) {
            selected_file = file_names[selected_file_index];
        }
        
        //load scene button
        if (ImGui::Button("Load Scene")) {
            load_selected = true;
            
        }

        ImGui::Separator();

        ImGui::Button("Drop Scenes Here");
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENES_ITEM")) {
                    const char* droppedFilePath = (const char*)payload->Data;

                    std::string file_name = droppedFilePath;
                    file_name.erase(0, ASM.get_full_path("Scenes", "").length());

                    if (std::find(file_names.begin(), file_names.end(), file_name) == file_names.end()) {
                        file_names.push_back(file_name);
                        std::cout << "Added: " << file_name << std::endl;
                    }
                    else {
                        std::cout << "Already exists: " << file_name << std::endl;
                    }
                }
                ImGui::EndDragDropTarget();
            }
        
   
        ImGui::End();

        //If file name is clicked and button is pressed
        if (load_selected && (selected_file_index != -1) && !selected_file.empty()) {

            //Gets file according to index load
            const std::string scenes = "Scenes";
            if (SM.load_scene(ASM.get_full_path(scenes, selected_file).c_str())) {
   
                GM.set_current_scene(selected_file_index + 1);
                selected_object_index = -1;

                // Reset camera position
                auto& camera = GFXM.get_camera();
                camera.pos_x = DEFAULT_CAMERA_POS_X;
                camera.pos_y = DEFAULT_CAMERA_POS_Y;

                // Update top UI overlay position to follow player
                EntityID ui_overlay_id = ECSM.find_entity_by_name("top_ui_overlay");
                EntityID oxygen_meter_id = ECSM.find_entity_by_name("top_ui_oxygen_meter");
                EntityID panic_meter_id = ECSM.find_entity_by_name("top_ui_panik_meter");
                EntityID mineral_texture_id = ECSM.find_entity_by_name("top_ui_mineral_texture");

                EntityID oxygen_text_id = ECSM.find_entity_by_name("top_ui_oxygen_text");
                EntityID panic_text_id = ECSM.find_entity_by_name("top_ui_panic_text");
                EntityID mineral_count_text_id = ECSM.find_entity_by_name("top_ui_mineral_count_text");

                // Reset player position if exists
                EntityID player_id = ECSM.find_entity_by_name(DEFAULT_PLAYER_NAME);
                if (player_id != INVALID_ENTITY_ID && GM.get_current_scene() == 2) {
                    if (ECSM.has_component<Transform2D>(player_id)) {
                        auto& transform = ECSM.get_component<Transform2D>(player_id);
                        transform.position = Vec2D(0.0f, 0.0f);
                        transform.prev_position = transform.position;
                    }
                    if (ECSM.has_component<Velocity_Component>(player_id)) {
                        auto& velocity = ECSM.get_component<Velocity_Component>(player_id);
                        velocity.velocity = Vec2D(0.0f, 0.0f);
                    }
                }

                if (ui_overlay_id != INVALID_ENTITY_ID) {
                    auto& player_transform = ECSM.get_component<Transform2D>(player_id);
                    auto& ui_transform = ECSM.get_component<Transform2D>(ui_overlay_id);

                    // Define layout constants for vertical stacking
                    constexpr float VERTICAL_OFFSET = 500.0f;        // Distance above player
                    constexpr float METER_SPACING = 50.0f;           // Vertical space between meters
                    constexpr float METER_WIDTH = 400.0f;            // Width of the meters
                    constexpr float METER_HEIGHT = 40.0f;            // Height of each meter bar
                    //constexpr float TEXT_OFFSET_X = 300.0f;           // Horizontal offset from the UI element
                    constexpr float TEXT_OFFSET_Y = 10.0f;            // Vertical offset from the UI element

                    // Calculate base position for UI elements
                    Vec2D base_position{
                        0.0f,
                        player_transform.position.y + VERTICAL_OFFSET
                    };

                    // Update main UI overlay position
                    ui_transform.position = base_position;
                    ui_transform.prev_position = ui_transform.position;

                    // Position oxygen meter (top meter)
                    if (oxygen_meter_id != INVALID_ENTITY_ID &&
                        ECSM.has_component<Transform2D>(oxygen_meter_id)) {
                        auto& oxygen_transform = ECSM.get_component<Transform2D>(oxygen_meter_id);

                        // Set position and scale for oxygen meter
                        oxygen_transform.position = {
                            base_position.x - METER_WIDTH,  // Center horizontally
                            base_position.y                 // Top position
                        };
                        oxygen_transform.scale = Vec2D(METER_WIDTH, METER_HEIGHT);
                        oxygen_transform.prev_position = oxygen_transform.position;
                    }

                    // Position oxygen text
                    if (oxygen_text_id != INVALID_ENTITY_ID &&
                        ECSM.has_component<Transform2D>(oxygen_text_id)) {
                        auto& oxygen_text_transform = ECSM.get_component<Transform2D>(oxygen_text_id);
                        //auto& oxygen_text = ECSM.get_component<Text_Component>(oxygen_text_id); 
                        auto& oxygen_transform = ECSM.get_component<Transform2D>(oxygen_meter_id);

                        // Position text to the left of the oxygen meter
                        oxygen_text_transform.position = {
                            (oxygen_transform.position.x - (oxygen_transform.scale.x / 2.0f) - (oxygen_text_transform.scale.x / 2.0f)), // Left of meter
                            oxygen_transform.position.y // Vertically centered with oxygen meter
                        };
                        oxygen_text_transform.prev_position = oxygen_text_transform.position;
                    }

                    // Position panic meter (bottom meter)
                    if (panic_meter_id != INVALID_ENTITY_ID &&
                        ECSM.has_component<Transform2D>(panic_meter_id)) {
                        auto& panic_transform = ECSM.get_component<Transform2D>(panic_meter_id);

                        // Set position and scale for panic meter
                        panic_transform.position = {
                            base_position.x - METER_WIDTH,          // Center horizontally
                            base_position.y - METER_SPACING         // Below oxygen meter
                        };
                        panic_transform.scale = Vec2D(METER_WIDTH, METER_HEIGHT);
                        panic_transform.prev_position = panic_transform.position;
                    }

                    // Position panic text
                    if (panic_text_id != INVALID_ENTITY_ID &&
                        ECSM.has_component<Transform2D>(panic_text_id)) {
                        auto& panic_text_transform = ECSM.get_component<Transform2D>(panic_text_id);
                        auto& panic_transform = ECSM.get_component<Transform2D>(panic_meter_id);

                        // Position text to the left of the panic meter
                        panic_text_transform.position = {
                            (panic_transform.position.x - (panic_transform.scale.x / 2.0f) - (panic_text_transform.scale.x / 2.0f)),  // Left of meter
                            panic_transform.position.y  // Vertically centered with panic meter
                        };
                        panic_text_transform.prev_position = panic_text_transform.position;
                    }

                    // Position mineral texture on the right side
                    if (mineral_texture_id != INVALID_ENTITY_ID &&
                        ECSM.has_component<Transform2D>(mineral_texture_id)) {
                        auto& mineral_transform = ECSM.get_component<Transform2D>(mineral_texture_id);

                        mineral_transform.position = {
                            base_position.x,                         // Center position
                            base_position.y - METER_SPACING / 2.0f   // Vertically centered between meters
                        };
                        mineral_transform.prev_position = mineral_transform.position;
                    }

                    // Position mineral count text
                    if (mineral_count_text_id != INVALID_ENTITY_ID &&
                        ECSM.has_component<Transform2D>(mineral_count_text_id) &&
                        ECSM.has_component<Transform2D>(mineral_texture_id)) {
                        auto& mineral_count_text_transform = ECSM.get_component<Transform2D>(mineral_count_text_id);
                        auto& mineral_transform = ECSM.get_component<Transform2D>(mineral_texture_id);

                        // Position text to the right of the mineral texture
                        mineral_count_text_transform.position = {
                            (mineral_transform.position.x + (mineral_transform.scale.x)) ,  // Right of icon
                            mineral_transform.position.y - TEXT_OFFSET_Y  // Vertically centered with mineral icon
                        };
                        mineral_count_text_transform.prev_position = mineral_count_text_transform.position;
                    }
                }

                //Note down current file
                set_current_file_shown(selected_file);

                //Reset audio names
                audio_file_names.clear();
                audio_types.clear();
                fill_up_sound_names();

            }

            load_selected = false;
        }     
    }

    //starts frame
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
            ImGui::Separator();
            ImGui::Text("Mouse in Game World at: (%.2f, %.2f)", mouse_texture_coord_world.x, mouse_texture_coord_world.y);
            ImGui::Text("Mouse in screen at: (%.2f, %.2f)", mouse_pos.x, mouse_pos.y);
            ImGui::Separator();
            ImGui::Text("Camera at: (%.2f, %.2f)", camera.pos_x, camera.pos_y);
        }
        else {

            //Display debug information
            ImGui::Text("Mouse outside texture at: (%.2f, %.2f)", mouse_pos.x, mouse_pos.y);
        }

        //Return mouse in terms of game world
        return mouse_texture_coord_world;
    }

    ImVec2 IMGUI_Manager::imgui_mouse_pos() {
        return mouse_pos_game;
    }

    bool select_entity = false; //to ensure mouse click selected
    EntityID selectedEntityID = static_cast<EntityID>(-1);

#if 1
    //Rendering overall UI and asset browser
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
        
        static int currentMode = 0; // Default to "Drag"
        const char* modes[] = { "None","Drag", "Scale", "Rotate" };

        ImGui::Begin("Level Editor Mode", nullptr, window_flags);
       
        //Set up Dockspace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("DockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), docking_flags);
        }

        //Set up Menu
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Menu"))
            {          
                if (ImGui::MenuItem("Stop Resizing ImGui Window", "", (docking_flags & ImGuiDockNodeFlags_NoResize) != 0)) {
                    docking_flags ^= ImGuiDockNodeFlags_NoResize;
                }

                ImGui::Separator();
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        
        ImGui::PopStyleVar(2);
        ImGui::End();

        //In the game world viewport
        if (GFXM.get_editor_mode() == 1) {
            ImGui::Begin("Game Viewport", nullptr);

            auto texture = GFXM.get_framebuffer_texture();
            ImVec2 texture_pos = ImGui::GetCursorScreenPos();

            if (texture) {
                ImGui::Image((ImTextureID)(intptr_t)GFXM.get_framebuffer_texture(),
                    ImVec2(static_cast<float>(SCR_WIDTH) / 2, static_cast<float>(SCR_HEIGHT) / 2),
                    ImVec2(0, 1), ImVec2(1, 0));
            }

            ImVec2 mouse_pos = ImGui::GetIO().MousePos;
            mouse_pos_game = get_imgui_mouse_pos(texture_pos, mouse_pos, SCR_WIDTH, SCR_HEIGHT);
            ESS.Check_Selected_Entity();
            EntityInfo& selectedEntityInfo = ESS.get_selected_entity_info();

            // Check if mouse is within texture bounds
            if (mouse_pos.x >= texture_pos.x && mouse_pos.x <= (texture_pos.x + SCR_WIDTH / 2) &&
                mouse_pos.y >= texture_pos.y && mouse_pos.y <= (texture_pos.y + SCR_HEIGHT / 2)) {

                // Handle entity selection with left click
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    mouse_clicked_or_dragged = true;
                    if (selectedEntityInfo.isSelected) {
                        select_entity = true;
                        selectedEntityID = selectedEntityInfo.selectedEntity;
                        LM.write_log("IMGUI_Manager::render_ui(): Selected Entity ID system: %d", selectedEntityInfo.selectedEntity);
                    }
                    else {
                        select_entity = false;
                        selectedEntityID = selectedEntityInfo.selectedEntity;
                    }
                }
                else {
                    mouse_clicked_or_dragged = false;
                    select_entity = false;
                }

                // Show context menu on right click when an entity is selected
                if (selectedEntityID != INVALID_ENTITY_ID && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                    ImGui::OpenPopup("OperationContextMenu");
                }

                // Position the context menu at the mouse position
                if (ImGui::BeginPopup("OperationContextMenu")) {
                    ImGui::Text("Select Operation");
                    ImGui::Separator();

                    for (int i = 0; i < IM_ARRAYSIZE(modes); i++) {
                        if (ImGui::Selectable(modes[i])) {
                            currentMode = i;
                        }
                    }

                    ImGui::EndPopup();
                }

                if (select_entity && selectedEntityID != INVALID_ENTITY_ID) {
                    selected_object_index = selectedEntityID;
                }

                // Handle mouse operations
                static ImVec2 selected_entity_start_pos;
                auto& entities = ecs.get_entities();

                if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && selectedEntityID != INVALID_ENTITY_ID) {
                    if (!mouse_was_down) {
                        mouse_pos_before_press = get_imgui_mouse_pos(texture_pos, mouse_pos, SCR_WIDTH, SCR_HEIGHT);
                        if (selectedEntityInfo.isSelected) {
                            if (entities[selectedEntityID]->has_component(ecs.get_component_id<Transform2D>())) {
                                Transform2D& transform = ecs.get_component<Transform2D>(entities[selectedEntityID].get()->get_id());
                                // Store initial values based on operation type
                                switch (currentMode) {
                                case 0: // None - do nothing
                                    break;
                                case 1: // Drag
                                    selected_entity_start_pos.x = transform.position.x;
                                    selected_entity_start_pos.y = transform.position.y;
                                    break;
                                case 2: // Scale
                                    selected_entity_start_pos.x = transform.scale.x;
                                    selected_entity_start_pos.y = transform.scale.y;
                                    break;
                                case 3: // Rotate
                                    selected_entity_start_pos.x = transform.orientation.x;
                                    selected_entity_start_pos.y = transform.orientation.y;
                                    break;
                                }
                            }
                        }
                        mouse_was_down = true;
                    }

                    switch (currentMode) {
                    case 0: 
                        break;
                    case 1: // Drag
                    {
                        ImVec2 dragged_offset;
                        unsigned int game_scale_width = SM.get_scr_width();
                        unsigned int game_scale_height = SM.get_scr_height();
                        unsigned int window_width = WC.get_win_width();
                        unsigned int window_height = WC.get_win_height();
                        float ratio_width = static_cast<float>(game_scale_width) / window_width;
                        float ratio_height = static_cast<float>(game_scale_height) / window_height;

                        if (is_full_screen) {
                            dragged_offset.x = (mouse_pos_game.x - mouse_pos_before_press.x) * ratio_width;
                            dragged_offset.y = (mouse_pos_game.y - mouse_pos_before_press.y) * ratio_height;
                        }
                        else {
                            dragged_offset.x = mouse_pos_game.x - mouse_pos_before_press.x;
                            dragged_offset.y = mouse_pos_game.y - mouse_pos_before_press.y;
                        }

                        if (selectedEntityInfo.isSelected && selectedEntityID != INVALID_ENTITY_ID) {
                            if (entities[selectedEntityID]->has_component(ecs.get_component_id<Transform2D>())) {
                                Transform2D& transform = ecs.get_component<Transform2D>(entities[selectedEntityID].get()->get_id());
                                transform.position.x = selected_entity_start_pos.x + dragged_offset.x;
                                transform.position.y = selected_entity_start_pos.y + dragged_offset.y;
                                transform.prev_position = transform.position;
                            }
                            if (entities[selectedEntityID]->has_component(ecs.get_component_id<Logic_Component>())) {
                                Logic_Component& logic = ecs.get_component<Logic_Component>(entities[selectedEntityID].get()->get_id());
                                logic.origin_pos.x = selected_entity_start_pos.x + dragged_offset.x;
                                logic.origin_pos.y = selected_entity_start_pos.y + dragged_offset.y;
                            }
                        }
                        break;
                    }
                    case 2: // Scale
                    {
                        ImVec2 scale_offset;
                        constexpr float SCALE_SENSITIVITY = 0.05f;
                        scale_offset.x = (mouse_pos_game.x - mouse_pos_before_press.x) * SCALE_SENSITIVITY;
                        scale_offset.y = (mouse_pos_game.y - mouse_pos_before_press.y) * SCALE_SENSITIVITY;

                        if (selectedEntityInfo.isSelected && selectedEntityID != INVALID_ENTITY_ID) {
                            if (entities[selectedEntityID]->has_component(ecs.get_component_id<Transform2D>())) {
                                Transform2D& transform = ecs.get_component<Transform2D>(entities[selectedEntityID].get()->get_id());

                                float scale_factor = 1.0f + scale_offset.x;
                                float new_scale_x = selected_entity_start_pos.x * scale_factor;
                                transform.scale.x = std::max(0.1f, new_scale_x);

                                scale_factor = 1.0f + scale_offset.y;
                                float new_scale_y = selected_entity_start_pos.y * scale_factor;
                                transform.scale.y = std::max(0.1f, new_scale_y);

                                if (entities[selectedEntityID]->has_component(ecs.get_component_id<Collision_Component>())) {
                                    Collision_Component& collision = ecs.get_component<Collision_Component>(entities[selectedEntityID].get()->get_id());
                                    collision.width = transform.scale.x;
                                    collision.height = transform.scale.y;
                                }
                            }
                        }
                        break;
                    }
                    case 3: //rotate
                    {
                        float rotation_offset = (mouse_pos_game.x - mouse_pos_before_press.x) * 0.5f;

                        if (selectedEntityInfo.isSelected && selectedEntityID != INVALID_ENTITY_ID) {
                            if (entities[selectedEntityID]->has_component(ecs.get_component_id<Transform2D>())) {
                                Transform2D& transform = ecs.get_component<Transform2D>(entities[selectedEntityID].get()->get_id());

                                // Update rotation
                                transform.orientation.x = selected_entity_start_pos.x + rotation_offset;

                                // Normalize rotation angle to keep it between 0 and 360 degrees
                                while (transform.orientation.x >= 360.0f) transform.orientation.x -= 360.0f;
                                while (transform.orientation.x < 0.0f) transform.orientation.x += 360.0f;
                            }
                        }
                        break;
                        }
                   
                    }
       
                }
                else {
                    mouse_was_down = false;
                }
            }

            ImGui::Separator();
            if (selectedEntityID == -1) {
                ImGui::Text("Selected Entity: None");
            }
            else {
                ImGui::Text("Selected Entity: %d", selectedEntityID);
            }

            ImGui::End();
        }

        //Calls other window's functions
        IMGUIM.asset_browser();
        IMGUIM.imgui_game_objects_list();
        IMGUIM.display_loading_options();
        IMGUIM.imgui_game_objects_edit();

    }
#endif

    //to disable the GUI
    void IMGUI_Manager::disable_GUI() {

        for (auto& system : ECSM.get_systems()) {
            if (auto* gui_system = dynamic_cast<GUI_System*>(system.get())) {
          
                gui_system->hide_mineral_tank_gui();
                gui_system->hide_oxygen_tank_gui();
     
            }
        }
    }

    //game object list
    void IMGUI_Manager::imgui_game_objects_list() {

        ImGui::Begin("Hierarchy Object List");

        int current_object_index = 0;
        const auto& entities = ecs.get_entities();

        //Resets selected_object_index to -1 to notify not selecting anything
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

        //Edit button
        ImGui::Text("\n");
        if (ImGui::Button("Edit Game Object")) {
            show_window = !show_window;
        }

        //Remove button; Disabled for when the Player entity is selecetd
        ImGui::BeginDisabled(selected_object_index != -1 && entities[selected_object_index].get()->get_name() == "player1");
        if (ImGui::Button("Remove Game Object")) {
            remove_game_obj = !remove_game_obj;
        }
        ImGui::EndDisabled();

        //Create Game Object button
        ImGui::Text("\n");
        if (ImGui::Button("Create Game Object From Prefab")) {

            create_game_obj = !create_game_obj;
        }

        //Save Changes button
        if (ImGui::Button("Save Changes")) {
            const std::string SCENES = "Scenes";

            std::string scene_path = ASM.get_full_path(SCENES, get_current_file_shown());
            if (SM.save_game_state(scene_path.c_str())) {   
                LM.write_log("IMGUI_Manager::imgui_game_objects_list(): Successfully saved game state");
            }
            else {
                LM.write_log("IMGUI_Manager::imgui_game_objects_list(): Failed saved game state");
            }
        }

        ImGui::End();

        //Remove game object; Ensures selected_object_index is safe 
        if (selected_object_index != -1 && remove_game_obj && selected_object_index < entities.size()) {
            remove_game_objects(selected_object_index);
            selected_object_index = -1;
        }

        //Create game object window
        if (create_game_obj) {
            add_game_objects();
        }

    }

    //Booleans to note down state of component boolean
    static bool is_static_on = false;
    static bool is_moveable_on = true;
    static bool is_grounded_on = false;

    static bool is_active_on = true;
    static bool is_reverse_on = false;
    static bool is_rotate_on = true;;

    // Use a static map to store buffers for each sound by index or key
    static std::unordered_map<int, std::string> buffer_map;

    //edit game object property list
    void IMGUI_Manager::imgui_game_objects_edit() {

        static int last_selected_object_index = -1; //Track the previous selected object index

        ImGui::Begin("Edit Object Properties");

        if (!show_window) {
            ImGui::Text("Select a game object to edit it.");
        }
        else {
            const auto& entities = ecs.get_entities();

            //For animation dropdown
            //If the selected object has changed, reset filled and clear assigned names for new object
            if (selected_object_index != last_selected_object_index) {
                last_selected_object_index = selected_object_index;
                assigned_names.clear();
                buffer_map.clear();
                filled = false; 
            }

            if (selected_object_index == -1) {
                ImGui::Text("Select a game object to edit it.");
            }
            else {

                std::string Name = entities[selected_object_index]->get_name();
                std::string condition_name_model = "Name of Entity";

                //Not using text_input function due to disabling textbox
                char buffer_disabled[128];
                strncpy_s(buffer_disabled, Name.c_str(), sizeof(buffer_disabled));
                buffer_disabled[sizeof(buffer_disabled) - 1] = '\0';
                ImGui::BeginDisabled();
                if (ImGui::InputText(condition_name_model.c_str(), buffer_disabled, sizeof(buffer_disabled))) {
                    std::string name = std::string(buffer_disabled);
                    entities[selected_object_index]->set_name(name);
                }
                ImGui::EndDisabled();


                //Transform2D Component
                if (entities[selected_object_index]->has_component(ecs.get_component_id<Transform2D>())) {
                    Transform2D& transform = ecs.get_component<Transform2D>(entities[selected_object_index].get()->get_id());
                    if (ImGui::CollapsingHeader("Transformation")) {

                        auto& position = transform.position;
                        ImGui::InputFloat2("Position", &position.x);

                        auto& prev_position = transform.prev_position;
                        prev_position = position;
                        //Disable input box
                        ImGui::BeginDisabled();
                        ImGui::InputFloat2("Previous Position", &prev_position.x);
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
                        ImGui::InputFloat("Maximum Velocity", &max_velocity);

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

                        //not using text_input due to to_lower
                        char buffer_graphics[128];
                        strncpy_s(buffer_graphics, texture_name.c_str(), sizeof(buffer_graphics));
                        buffer_graphics[sizeof(buffer_graphics) - 1] = '\0';
                        if (ImGui::InputText(condition_name_texture.c_str(), buffer_graphics, sizeof(buffer_graphics))) {
                            std::string buffer_string = std::string(buffer_graphics);
                            std::transform(buffer_string.begin(), buffer_string.end(), buffer_string.begin(), to_lower);
                            texture_name = buffer_string;
                        }

                        if (ImGui::BeginDragDropTarget()) {

                            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_ITEM")) {
                                const char* droppedFilePath = (const char*)payload->Data;
                                std::string file_name = droppedFilePath;
                                file_name.erase(0, ASM.get_full_path("Textures", "").length());
                                file_name = file_name.substr(0, file_name.size()-4);

                                std::transform(file_name.begin(), file_name.end(), file_name.begin(), to_lower);
                                texture_name = file_name;
                            }

                            ImGui::EndDragDropTarget();
                        }

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

                    //If the animation names have not been filled
                    if (!filled) {

                        auto& animation_list = animation.animations;
                        for (const auto& it : animation_list) {
                            assigned_names.push_back(it.second);
                        }

                        filled = true;

                    }

                    if (ImGui::CollapsingHeader("Animation") && filled) {

                        //Converts all of the names to c-string
                        std::vector<const char*> animation_names_c_str;
                        for (const auto& name : assigned_names) {
                            animation_names_c_str.push_back(name.c_str());
                        }

                        auto& animation_list = animation.animations;

                        //vector to keep track of selected items for each animation
                        //initialized with -1 for each item, no selection
                        static std::vector<int> selected_items(animation_list.size(), -1);

                        //index of the action to select animation for
                        int index = 0;

                        for (auto it = animation_list.begin(); it != animation_list.end(); ++it, ++index) {

                            //ensures the selected_items vector is the same size as the animation_list
                            if (selected_items.size() != animation_list.size()) {
                                selected_items.resize(animation_list.size(), -1);
                            }

                            ImGui::Text("Selected Animation for %i: %s", index, it->second.c_str());
                            std::string label = "Choose Animation for " + std::to_string(index);

                            //Dropdown for the animation
                            if (ImGui::Combo(label.c_str(), &selected_items[index], animation_names_c_str.data(), static_cast<int>(assigned_names.size()))) {

                                //If valid animation is selected, update the animation.
                                if (selected_items[index] >= 0 && selected_items[index] < assigned_names.size()) {

                                    //Update the animation's name
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

                            //Only update curr_animation_idx if temp_value is within valid bounds
                            if (temp_value >= 0 && temp_value < static_cast<int>(animation_list.size())) {
                                curr = static_cast<unsigned int>(temp_value);
                            }
                        }

                    }
                }

                const char* logic_behaviour[] = { "Horizontal", "Circular"};

                //Logic Component
                if (entities[selected_object_index]->has_component(ecs.get_component_id<Logic_Component>())) {
                    Logic_Component& logic = ecs.get_component<Logic_Component>(entities[selected_object_index].get()->get_id());
                    if (ImGui::CollapsingHeader("Logic")) {

                        auto& is_active = logic.is_active;
                        std::string s_label = "is_active: " + std::string(is_active_on ? "On" : "Off");
                        if (button_toggle(s_label, &is_active_on)) {
                            is_active = !is_active;
                        }

                        auto& movement_pattern = logic.movement_pattern;
                        //Update logic behavior, corresponds the string to the movement_pattern value
                        int current_behaviour = (movement_pattern == Logic_Component::MovementPattern::LINEAR) ? 0 : 1;
                        ImGui::Text("Choose Logic Behaviour");
                        if (ImGui::Combo(entities[selected_object_index].get()->get_name().c_str(), &current_behaviour, logic_behaviour, IM_ARRAYSIZE(logic_behaviour))) {
                           
                            movement_pattern = (current_behaviour == 0) ? Logic_Component::MovementPattern::LINEAR : Logic_Component::MovementPattern::CIRCULAR;
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

                //Audio Component
                if (entities[selected_object_index]->has_component(ecs.get_component_id<Audio_Component>())) {
                    Audio_Component& audio = ecs.get_component<Audio_Component>(entities[selected_object_index].get()->get_id());
                    if (ImGui::CollapsingHeader("Audio")) {

                        //gets a collection of sounds from audio component (not sound map)
                        auto& sounds = audio.get_sounds();

                        //Static vector to keep track of selected sounds(actions that need sounds)
                        static std::vector<int> selected_sounds;

                        //Clears vector to make sure its empty
                        selected_sounds.clear();

                        //resizes it to match sounds/actions (initializing everything to -1 to indicate no selection)
                        //sound size should be 3
                        selected_sounds.resize(sounds.size(), -1);

                        //Iterate through each sound/action
                        for (int i = 0; i < sounds.size(); ++i) {

                            //Get corresponding filepath for the current sound's key
                            auto sound_filepath = audio.get_filepath(sounds[i].key);
                            std::string saved_keyID = sound_filepath + std::to_string(entities[selected_object_index]->get_id()) + sounds[i].key;

                            //Populate file_name_cstr from the sound map in Audio_System
                            std::vector<const char*> file_name_cstr;
                            std::vector<std::string> sound_map_filenames;

                            //Retrieve the sound map filenames from Audio_System
                            sound_map_filenames = ADM.get_sound_map_filename();
                            for (const auto& name : sound_map_filenames) {
                                file_name_cstr.push_back(name.c_str());
                            }

                            //Find the current sound's representative string in the sound map
                            auto its = std::find(sound_map_filenames.begin(), sound_map_filenames.end(), sound_filepath);

                            //Show the current sound's representative string in a text label
                            if (its != sound_map_filenames.end()) {
                                ImGui::Text("Selected Sound for %s: %s", sounds[i].key.c_str(), its->c_str());
                            }
                            else {
                                ImGui::Text("Selected Sound for %s: Not Found", sounds[i].key.c_str());
                            }

                            //Create a combo box to choose the new representative string for the sound
                            std::string label = "Choose Sound for " + std::to_string(i); // Label for the dropdown
                            if (ImGui::Combo(label.c_str(), &selected_sounds[i], file_name_cstr.data(), static_cast<int>(file_name_cstr.size()))) {

                                int selected_index = selected_sounds[i];
                                if (selected_index >= 0 && selected_index < file_name_cstr.size()) {

                                    //Update the sound's file path based on the selected string from the sound map
                                    audio.set_filepath(sounds[i].key, sound_map_filenames[selected_index]);
                                    LM.write_log("IMGUIM:: Sound being changed to %s for %s", sound_map_filenames[selected_index].c_str(), sounds[i].key.c_str());
                                }
                            }

                            //Drag and drop functionality
                            if (ImGui::BeginDragDropTarget()) {

                                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AUDIO_ITEM")) {
                                    const char* droppedFilePath = (const char*)payload->Data;

                                    std::string file_path = droppedFilePath;
                                    std::string file_name = droppedFilePath;
                                    file_name.erase(0, ASM.get_full_path("Audio", "").length());
                                    file_name = file_name.substr(0, file_name.find_last_of('.'));

                                    bool file_in_dropdown = false;
                                    
                                    for (int j = 0; j < file_name_cstr.size(); ++j) {
                                        if (j < sound_map_filenames.size()) {
                                            if (file_name == sound_map_filenames[j]) {
                                                audio.set_filepath(sounds[i].key, sound_map_filenames[j]);
                                                file_in_dropdown = true;
                                                break;
                                            }
                                        }
                                    }

                                    if (!file_in_dropdown) {

                                        ADM.load_sound(file_name, audio.get_audio_type(sounds[i].key));
                                        audio.set_filepath(sounds[i].key, file_name);
                                    }
                                   
                                }
                                ImGui::EndDragDropTarget();
                            }
                        }

                        //Updating Audio Key - Has Bug
                        for (int i = 0; i < sounds.size(); ++i) {
                      
                            std::string old_key_name = sounds[i].key;
                            std::string condition_name_key = "key for " + std::to_string(i);

                            if (buffer_map.find(i) == buffer_map.end()) {
                                buffer_map[i] = old_key_name;
                            }

                            text_input(buffer_map[i], condition_name_key);

                            //Save button
                            std::string save = "save " + condition_name_key;
                            const char* button_name = save.c_str();
                            if (ImGui::Button(button_name)) {
                                std::string new_key_name = buffer_map[i];
                                audio.set_key(old_key_name, new_key_name);

                            }
                            

                        }

                        //For Audio Type
                        static std::vector<int> selected_sounds_type;
                        selected_sounds_type.clear();
                        selected_sounds_type.resize(sounds.size(), -1);
                        
                        int type_index = 0;
                        for (int i = 0; i < sounds.size(); ++i, ++type_index) {

                            auto audio_type = audio.get_audio_type(sounds[i].key);
                            std::vector<const char*> audio_type_cstr;

                            for (const auto& fill_audio_type_pair : audio_types) {
                                audio_type_cstr.push_back(fill_audio_type_pair.first.c_str());
                            }

                            //Find matching representative string for the file path
                            auto its = std::find_if(audio_types.begin(), audio_types.end(),
                                [&audio_type](const std::pair<std::string, AudioType>& p) {
                                    return p.second == audio_type;
                                });


                            //Show the current sound's representative string in a text label
                            if (its != audio_types.end()) {
                                ImGui::Text("Audio Type for %s: %s", sounds[i].key.c_str(), its->first.c_str());
                            }
                            else {
                                ImGui::Text("Audio Type for %s: Not Found", sounds[i].key.c_str());
                            }

                            //Create a combo box to choose the new representative string for this sound
                            std::string label = "Choose Audio Type for " + std::to_string(type_index); 
                            if (ImGui::Combo(label.c_str(), &selected_sounds_type[type_index], audio_type_cstr.data(), static_cast<int>(audio_type_cstr.size()))) {

                                //Get the selected representative string type_index
                                int selected_index = selected_sounds_type[type_index];
                                if (selected_index >= 0 && selected_index < audio_type_cstr.size()) {

                                    // Update the sound's file path based on the selected rep string
                                    audio.set_audio_type(sounds[i].key, audio_types[selected_index].second);
                                    
                                }
                            }
                        }
                    }
                }

                //Text Component
                if (entities[selected_object_index]->has_component(ecs.get_component_id<Text_Component>())) {
                    Text_Component& text_component = ecs.get_component<Text_Component>(entities[selected_object_index].get()->get_id());
                    if (ImGui::CollapsingHeader("Text")) {

                        auto& font_name = text_component.font_name;
                        std::string text_condition = "Font Name";
                        text_input(font_name, text_condition);

                        if (ImGui::BeginDragDropTarget()) {

                            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FONT_ITEM")) {
                                const char* droppedFilePath = (const char*)payload->Data;
          
                                std::string file_name = droppedFilePath;
                                file_name.erase(0, ASM.get_full_path("Fonts", "").length());
                                file_name = file_name.substr(0, file_name.size() - 4);
                                font_name = file_name;
                            }

                            ImGui::EndDragDropTarget();
                        }

                        auto& text_name = text_component.text;
                        std::string name_condition = "Text Name";
                        text_input(text_name, name_condition);

                    }
                }

                //GUI Component
                /*if (entities[selected_object_index]->has_component(ecs.get_component_id<GUI_Component>())) {
                    GUI_Component& gui = ecs.get_component<GUI_Component>(entities[selected_object_index].get()->get_id());
                    if (ImGui::CollapsingHeader("GUI Component")) {
                        auto& progress = gui.progress;
                        ImGui::InputFloat("Movement Range", &progress);
                        auto& progress_bar = gui.is_progress_bar;
                        std::string is_progress_bar = "is_progress_bar: " + std::string(progress_bar ? "On" : "Off");
                        if (button_toggle(is_progress_bar, &progress_bar)) {
                            progress_bar = !progress_bar;
                        }
                        auto& container = gui.is_container;
                        std::string is_container = "is_container: " + std::string(container ? "On" : "Off");
                        if (button_toggle(is_container, &container)) {
                            container = !container;
                        }
                    }
                }*/

                static int selected = 0;
                static std::vector<const char*> missing_components;

                //populate the vector (done before rendering ImGui)
                missing_components.clear(); //clear previous data
                missing_components.push_back("None");

                std::vector<std::tuple<const char*, ComponentID, std::function<void()>, std::function<void()>>> component_checks = {
                    {"Transform Component", static_cast<ComponentID>(ecs.get_component_id<Transform2D>()),
                        [&]() { ecs.add_component<Transform2D>(entities[selected_object_index]->get_id(), Transform2D()); },
                        [&]() { ecs.remove_component<Transform2D>(entities[selected_object_index]->get_id()); }},

                    {"Velocity Component", static_cast<ComponentID>(ecs.get_component_id<Velocity_Component>()),
                        [&]() { ecs.add_component<Velocity_Component>(entities[selected_object_index]->get_id(), Velocity_Component()); },
                        [&]() { ecs.remove_component<Velocity_Component>(entities[selected_object_index]->get_id()); }},

                    {"Physics Component", static_cast<ComponentID>(ecs.get_component_id<Physics_Component>()),
                        [&]() { ecs.add_component<Physics_Component>(entities[selected_object_index]->get_id(), Physics_Component()); },
                        [&]() { ecs.remove_component<Physics_Component>(entities[selected_object_index]->get_id()); }},

                    {"Graphics Component", static_cast<ComponentID>(ecs.get_component_id<Graphics_Component>()),
                        [&]() { ecs.add_component<Graphics_Component>(entities[selected_object_index]->get_id(), Graphics_Component()); },
                        [&]() { ecs.remove_component<Graphics_Component>(entities[selected_object_index]->get_id()); }},

                    {"Collision Component", static_cast<ComponentID>(ecs.get_component_id<Collision_Component>()),
                        [&]() { ecs.add_component<Collision_Component>(entities[selected_object_index]->get_id(), Collision_Component()); },
                        [&]() { ecs.remove_component<Collision_Component>(entities[selected_object_index]->get_id()); }},

                    {"Animation Component", static_cast<ComponentID>(ecs.get_component_id<Animation_Component>()),
                        [&]() { ecs.add_component<Animation_Component>(entities[selected_object_index]->get_id(), Animation_Component()); },
                        [&]() { ecs.remove_component<Animation_Component>(entities[selected_object_index]->get_id()); }},

                    {"Logic Component", static_cast<ComponentID>(ecs.get_component_id<Logic_Component>()),
                        [&]() { ecs.add_component<Logic_Component>(entities[selected_object_index]->get_id(), Logic_Component()); },
                        [&]() { ecs.remove_component<Logic_Component>(entities[selected_object_index]->get_id()); }},

                    {"Audio Component", static_cast<ComponentID>(ecs.get_component_id<Audio_Component>()),
                        [&]() { ecs.add_component<Audio_Component>(entities[selected_object_index]->get_id(), Audio_Component()); },
                        [&]() { ecs.remove_component<Audio_Component>(entities[selected_object_index]->get_id()); }},

                    {"Text Component", static_cast<ComponentID>(ecs.get_component_id<Text_Component>()),
                        [&]() { ecs.add_component<Text_Component>(entities[selected_object_index]->get_id(), Text_Component()); },
                        [&]() { ecs.remove_component<Text_Component>(entities[selected_object_index]->get_id()); }},
                };

                for (const auto& [name, id, add_func, remove_func] : component_checks) {
                    if (!entities[selected_object_index]->has_component(id)) {
                        missing_components.push_back(name);
                    }
                }

                std::string add_cmpt = "Add Component";
                const char* add_button_name = add_cmpt.c_str();
                if (ImGui::Button(add_button_name)) {
                    ImGui::OpenPopup("Add Components");
                }

                if (ImGui::BeginPopup("Add Components")) {
                    ImGui::Text("Add Components");
                    ImGui::Separator();

                    for (size_t i = 0; i < missing_components.size(); i++) {
                        if (ImGui::Selectable(missing_components[i])) {  // Ensure it's a C-string
                            selected = static_cast<int>(i);  // Store the selected index
                        }
                    }
                    ImGui::EndPopup();
                }

                if (selected != -1) {

                    switch (selected) {
                    case 0:
                        break;
                    default:
                        if (selected >= 0 && static_cast<size_t>(selected) < missing_components.size()) {

                            const auto& component_name = missing_components[selected];
                            for (const auto& [name, id, add_func, remove_func] : component_checks) {
                                if (std::string(component_name) == name) {
                                    add_func();
                                    LM.write_log("IMGUI_Manager::imgui_game_objects_list(): Added %s to %s", name, entities[selected_object_index]->get_name().c_str());
                                    break;
                                }
                            }

                        }
                        break;
                    }
                    selected = -1;  // Reset `selected` after handling
                }

                static size_t selected_to_remove = 0;
                static std::vector<const char*> present_components;

                //populate the vector (done before rendering ImGui)
                present_components.clear(); //clear previous data
                present_components.push_back("None");

                for (const auto& [name, id, add_func, remove_func] : component_checks) {
                    if (entities[selected_object_index]->has_component(id)) {
                        present_components.push_back(name);
                    }
                }

                std::string rmv_cmpt = "Remove Component";
                const char* remove_button_name = rmv_cmpt.c_str();
                if (ImGui::Button(remove_button_name)) {
                    ImGui::OpenPopup("Remove Components");
                }

                if (ImGui::BeginPopup("Remove Components")) {
                    ImGui::Text("Remove Components");
                    ImGui::Separator();

                    for (size_t i = 0; i < present_components.size(); i++) {
                        if (ImGui::Selectable(present_components[i])) {
                            selected_to_remove = i;
                        }
                    }
                    ImGui::EndPopup();
                }

                if (selected_to_remove != -1) {  // Check if a valid selection was made

                    switch (selected_to_remove) {
                    case 0:
                        break;
                    default: 
                        if (selected_to_remove >= 0 && selected_to_remove < present_components.size()) {  // Bounds check

                            const auto& component_name = present_components[selected_to_remove];
                            for (const auto& [name, id, add_func, remove_func] : component_checks) {
                                if (std::string(component_name) == name) {
                                    remove_func();
                                    LM.write_log("IMGUI_Manager::imgui_game_objects_list(): Removed %s to %s", name, entities[selected_object_index]->get_name().c_str());
                                    break;
                                }
                            }
                        }
                        break;
                    }
                    selected_to_remove = static_cast<size_t>(-1);
                }

            }
        }

        ImGui::End();
    }

    static std::string selected_filepath = "";
    static bool is_file_selected = false;

#if 0
    void remove_button() {

        if (is_file_selected == true) {

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0));
            if (ImGui::Button("DELETE FILE")) {

                try {

                    std::string temp = selected_filepath;
                    selected_filepath.clear();
                    std::filesystem::remove(temp);
                   
                    size_t start_pos_of_folder_filepath;

                    //Deleting Textures from Associated Entities
                    if ((start_pos_of_folder_filepath = temp.find(ASM.get_full_path("Textures", ""))) != std::string::npos) {
                        
                        temp.erase(0, ASM.get_full_path("Textures", "").length());
                        temp = temp.substr(0, temp.find_last_of('.'));
                        std::transform(temp.begin(), temp.end(), temp.begin(), to_lower);

                        auto& texture_storage = ASM.get_texture_storage();
                        auto texture = texture_storage.begin();

                        while (texture != texture_storage.end()) {
                            std::cout << texture->first << std::endl;
                            if (texture->first == temp) {
                                std::cout << temp << " is deleted. please delete from storage" << std::endl;
                                texture = texture_storage.erase(texture);
                            }
                            else {
                                ++texture;
                            }
                        }

                        is_file_selected = false;
                    }
                    
                    else if ((start_pos_of_folder_filepath = temp.find(ASM.get_full_path("Fonts", ""))) != std::string::npos) {

                        temp.erase(0, ASM.get_full_path("Fonts", "").length());
                        temp = temp.substr(0, temp.find_last_of('.'));

                        auto& font_storage = ASM.get_font_storage();
                        auto font = font_storage.begin();
                        while (font != font_storage.end()) {
                            std::cout << font->first << std::endl;
                        }

                        is_file_selected = false;
                    }
                    else 
                    {
                        ImGui::Text("Deletion Not Available For Asset Type");
                    }
                }
                catch (std::filesystem::filesystem_error& e) {

                    const char* error_msg = e.what();
                    if (error_msg) {
                        ImGui::Text("Error in Deletion: %s", error_msg);
                    }
                    else {
                        ImGui::Text("Error in Deletion");
                    }
                }
            }
            ImGui::PopStyleColor();
        }
    }
#endif

    void remove_button() {
        if (is_file_selected == true) {

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0));
            if (ImGui::Button("DELETE FILE")) {

                try {
               
                    std::string temp = selected_filepath;
                    selected_filepath.clear();
                    size_t start_pos_of_folder_filepath;

                    //Deleting Textures from Associated Entities
                    if ((start_pos_of_folder_filepath = temp.find(ASM.get_full_path("Textures", ""))) != std::string::npos) {

                        std::filesystem::remove(temp);
                        
                        auto& texture_storage = ASM.get_texture_storage();

                        //Prepare the texture name for removal
                        temp.erase(0, ASM.get_full_path("Textures", "").length());
                        temp = temp.substr(0, temp.find_last_of('.'));
                        std::transform(temp.begin(), temp.end(), temp.begin(), to_lower);

                        // Step 3: Call the Assets_Manager's delete_texture function to delete the texture and remove associated components
                        ASM.delete_texture(temp);

                        is_file_selected = false;
                    }
                    else if ((start_pos_of_folder_filepath = temp.find(ASM.get_full_path("Fonts", ""))) != std::string::npos) {
                        
                        std::filesystem::remove(temp);

                        temp.erase(0, ASM.get_full_path("Fonts", "").length());
                        temp = temp.substr(0, temp.find_last_of('.'));

                        auto& font_storage = ASM.get_font_storage();

                        //To Change to Asset Manager
                        auto font = font_storage.begin();

                        while (font != font_storage.end()) {
                            std::cout << font->first << std::endl;
                            if (font->first == temp) {
                                font = font_storage.erase(font);
                            }
                            else {
                                ++font;
                            }
                        }
                        is_file_selected = false;
                    }
                    else if ((start_pos_of_folder_filepath = temp.find(ASM.get_full_path("Audio", ""))) != std::string::npos) {

                        //AUDIO - Channel Map Not Working (Repetition Issue); Sound map Working
                        /*std::cout << "------------------------------------" << std::endl;
                        std::cout << "BEFORE DELETION" << std::endl;
                        for (auto& system : ECSM.get_systems()) {
                            if (system->get_type() == "Audio_System") {
                                auto* audio_system = static_cast<Audio_System*>(system.get());

                                std::cout << "CHECKING CHANNEL MAP" << std::endl;
                                for (auto& channel : audio_system->get_channel_map()) {
                                    std::cout << channel.first << std::endl;
                                }
                            }
                        }
                        std::cout << "CHECKING SOUND MAP" << std::endl;
                        auto& sound_map = ADM.get_sound_map();
                        for (auto it = sound_map.begin(); it != sound_map.end(); ++it) {
                            std::cout << it->first << std::endl;
                        }
                        std::cout << "------------------------------------" << std::endl;*/

                        //Get the filepath
                        std::string filepath = temp;

                        //Get the filename
                        temp.erase(0, ASM.get_full_path("Audio", "").length());
                        temp = temp.substr(0, temp.find_last_of('.'));
                        std::string filename = temp;

                        //TODO: Identify entity with selected sound
                        EntityID entity_with_sound = 5;

                        //Get sound key from Audio Component
                        Audio_Component& audio = ECSM.get_component<Audio_Component>(entity_with_sound);
                        auto& sounds = audio.get_sounds();
                        std::string sound_key;
                        std::cout << "sound_key:" << std::endl;
                        for (auto& sound : sounds) {
                            std::cout << sound.filepath << std::endl;
                            if (sound.filepath == filename) {
                                sound_key = sound.key;
                            }
                        }

                        std::cout << "sound_key: " << sound_key << std::endl;
                        //If sound key is found
                        if (!sound_key.empty()) {

                            //audio.set_audio_state(sound_key, PlayState::NONE);

                            //Combine to get key for channel map
                            std::string key_for_channel_map = filename + std::to_string(entity_with_sound) + sound_key;

                            //Stop sound, erase from channel map
                            for (auto& system : ECSM.get_systems()) {
                                if (system->get_type() == "Audio_System") {
                                    auto* audio_system = static_cast<Audio_System*>(system.get());

                                    //In audio System
                                    if (audio_system) {
                                        audio_system->stop_sound(key_for_channel_map);

                                        //Plausible Fix
                                        //audio_system->get_channel_map().erase(key_for_channel_map);

                                        std::cout << "deleted "<< key_for_channel_map << " from channel map\n";
                                    }

                                    auto& sound_map = ADM.get_sound_map();

                                    /*for (auto it = sound_map.begin(); it != sound_map.end(); ) {
                                        if (it->first == filename) {
                                            it = sound_map.erase(it);
                                            std::cout << "Deleted from sound map" << std::endl;
                                        }
                                        else {
                                            ++it;
                                        }
                                    }*/

                                    if (sound_map.find(filename) != sound_map.end()) {
                                        sound_map.erase(filename);  //Remove from the map
                                        std::cout << "Successfully removed from sound_map." << std::endl;
                                    }
                                    else {
                                        std::cout << "Sound was not found in sound_map." << std::endl;
                                    }

                                }
                            }

                            //Small delay to ensure system releases file
                            std::this_thread::sleep_for(std::chrono::milliseconds(100));

                        }

                        std::cout << "Attempting to remove: " << filename << std::endl;
                        if (std::filesystem::exists(filepath)) {
                            std::cout << "File exists before removal." << std::endl;

                            //Attempt to close any open streams (precautionary)
                            std::ifstream file(filepath);
                            if (file.is_open()) {
                                file.close();
                                std::cout << "Closed any open file stream.\n";
                            }

                            // Attempt deletion
                            if (std::filesystem::remove(filepath)) {
                                std::cout << "File successfully removed.\n";
                            }
                            else {
                                std::cerr << "Failed to remove file: " << filename << std::endl;
                            }
                        }
                        else {
                            std::cerr << "File does not exist.\n";
                        }

                        /*std::cout << "------------------------------------" << std::endl;
                        std::cout << "AFTER DELETION" << std::endl;
                        for (auto& system : ECSM.get_systems()) {
                            if (system->get_type() == "Audio_System") {
                                auto* audio_system = static_cast<Audio_System*>(system.get());

                                std::cout << "CHECKING CHANNEL MAP" << std::endl;
                                for (auto& channel : audio_system->get_channel_map()) {
                                    std::cout << channel.first << std::endl;
                                }
                            }
                        }
                        std::cout << "CHECKING SOUND MAP" << std::endl;
                        for (auto it = sound_map.begin(); it != sound_map.end(); ++it) {
                            std::cout << it->first << std::endl;
                        }
                        std::cout << "------------------------------------" << std::endl;*/

                    }
                    /*else if ((start_pos_of_folder_filepath = temp.find(ASM.get_full_path("Scenes", ""))) != std::string::npos) {
                        std::filesystem::remove(temp);
                    }*/
                    else {

                        std::filesystem::remove(temp);

                        /*ImGui::OpenPopup("Warning Message");
                        if (ImGui::BeginPopup("Warning Message")) {
                            ImVec4 text_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
                            ImGui::PushStyleColor(ImGuiCol_Text, text_color);
                            ImGui::Text("Note: If file is deleted, game will not work when reloaded!");
                            if (ImGui::Button("Delete Anyway")) {
                                std::filesystem::remove(temp);
                            }
                            ImGui::PopStyleColor();
                            ImGui::EndPopup();
                        }*/
                    }

                }
                catch (std::filesystem::filesystem_error& e) {
                    const char* error_msg = e.what();
                    if (error_msg) {
                        std::cout << "Error in Deletion: " << error_msg << std::endl;
                    }
                    else {
                        ImGui::Text("Error in Deletion");
                    }
                }
            }
            ImGui::PopStyleColor();
        }
    }

#if 0
    void IMGUI_Manager::render_asset_browser() {
        
        // Directory to load files from
        std::string tex_directory = ASM.get_full_path("Textures", "");

        if (!std::filesystem::exists(tex_directory)) {
            return;
        }

        if (ImGui::Begin("Asset Browser")) {
            // Button to open the file browser
            if (ImGui::Button("Add Asset")) {
                // You can call your custom file selection logic here
                std::string selected_file = open_file_explorer();  // Your file explorer function

                if (!selected_file.empty()) {
                    // Register the selected asset
                    ASM.register_assets_from_file(selected_file);
                }
            }

            auto& all_assets = ASM.get_all_assets();
            // Display all registered assets
            for (const auto& asset_name : all_assets) {
                if (ImGui::Selectable(asset_name.c_str())) {
                    // Handle asset selection (e.g., load the asset or preview it)
                }
            }

            ImGui::End();
        }
    }

    std::string IMGUI_Manager::open_file_explorer() {
        // Display the files in the directory for selection
        std::string tex_directory = ASM.get_full_path("Textures", "");

        // Here we simulate opening the file explorer
        // Loop through the files and list them in the ImGui UI (the file selection itself happens in the render loop)
        for (const auto& entry : std::filesystem::directory_iterator(tex_directory)) {
            if (entry.is_regular_file()) {
                std::string file_name = entry.path().stem().string(); // Get the file name without extension

                // Show the file name as a selectable item in ImGui
                if (ImGui::Selectable(file_name.c_str())) {
                    // Return the selected file path for registration
                    return entry.path().string();  // Return full path of the selected file
                }
            }
        }

        return "";  // Return an empty string if no file is selected
    }


#endif 

    static bool show_msg = true;

    //asset browser
    void IMGUI_Manager::asset_browser() {
        
        ImGui::Begin("Asset Browser");

        //current directory
        static std::string current_directory = "";

        //get asset folder directory
        const std::string ASSETS = "";
        std::string assets_path = ASM.get_full_path(ASSETS, "");

        ImGui::Columns(7, 0, false);

        //if asset folder cannot be found
        if (assets_path.empty() || !std::filesystem::exists(assets_path)) {
            ImGui::Text("Error: Invalid assets path.");
        }
        else { //otherwise if it can be found

            //if the current directory is empty
            if (current_directory.empty()) {

                ImGui::Separator(); // Adds a separator between the columns and other widgets

                try { 

                    //accessing each file in assets directory
                    for (const auto& entry : std::filesystem::directory_iterator(assets_path)) {

                        //if directory is selected
                        if (entry.is_directory()) {
                            if (ImGui::Button(entry.path().filename().string().c_str(), {128, 128})) {
                                current_directory = ASM.get_full_path(entry.path().filename().string(), "").c_str();
                            }
                            ImGui::Text(entry.path().filename().string().c_str());
                        }
                        ImGui::NextColumn();
                    }
                }
                catch (const std::filesystem::filesystem_error& e) {
                    ImGui::Text("Error accessing assets directory: %s", e.what());
                }
            }
            else { //if current directory isn't empty, its in a new directory

                //Back button that goes back to the assets directory (when current_directory is empty  
                /*if (ImGui::Button("Add File")) {
                    // Logic to add folder
                }

                ImGui::NextColumn();

                if (ImGui::Button("Delete File")) {
                    ;
                }*/
                
                ImGui::Columns(8, 0, false);

                if (ImGui::Button("<- Back")) {
                    current_directory.clear();
                }

                remove_button();

                ImGui::Separator();

                if (!current_directory.empty()) {
                    try {

                        //Block for Models and Shaders
                        if (current_directory == ASM.get_full_path("Models", "") || current_directory == ASM.get_full_path("Shaders", "") || 
                            current_directory == ASM.get_full_path("Config", "") || current_directory == ASM.get_full_path("Prefab", "") || 
                            current_directory == ASM.get_full_path("Level_Design", "")) {

                            asset_browser_pop_up(show_msg, "N/A Message", "  Dragging Not Available For Asset Type  ");

                            //if (show_msg) {
                            //    ImGui::OpenPopup("N/A Message");
                            //    ImVec2 popup_position = ImVec2(750, 750);  // Choose a fixed position
                            //    ImGui::SetNextWindowPos(popup_position);
                            //    if (ImGui::BeginPopup("N/A Message")) {
                            //        // Customizing the text size and color
                            //        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]); // Use a custom or default font with larger size (if desired)
                            //        ImVec4 text_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);  // Red color for the text
                            //        ImGui::PushStyleColor(ImGuiCol_Text, text_color);
                            //        ImGui::NewLine();
                            //        // Show the message
                            //        ImGui::Text("  Dragging Not Available For Asset Type  ");
                            //        ImGui::NewLine();
                            //        // Reset styles to default
                            //        ImGui::PopStyleColor();
                            //        ImGui::PopFont();
                            //        // Close button
                            //        if (ImGui::Button("Close Message")) {
                            //            show_msg = false;  // Close the message popup
                            //        }
                            //        ImGui::EndPopup();
                            //    }
                            //}
                        }

                        //accessing each file in the current directory
                        for (const auto& folder_entry : std::filesystem::directory_iterator(current_directory)) {

                            if (folder_entry.is_regular_file()) {

                                if (ImGui::Button(folder_entry.path().filename().string().c_str(), { 128, 128 })) {
                                    selected_filepath = folder_entry.path().string();
                                    is_file_selected = true;
                                }

                                if (selected_filepath == folder_entry.path().string()) {
                                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0, 1.0, 0.0, 1.0));
                                    ImGui::Text("SELECTED");
                                    ImGui::PopStyleColor();

                                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0));
                                    if (ImGui::Button("Remove Selection")) {
                                        selected_filepath.clear();
                                        is_file_selected = false;
                                    }
                                    ImGui::PopStyleColor();
                                }

                                set_drag_drop_source(current_directory, "Textures", folder_entry.path().string(), "TEXTURE_ITEM");
                                set_drag_drop_source(current_directory, "Audio", folder_entry.path().string(), "AUDIO_ITEM");
                                set_drag_drop_source(current_directory, "Scenes", folder_entry.path().string(), "SCENES_ITEM");

                                //Manually done to account for File
                                if (current_directory == ASM.get_full_path("Fonts", "")) { 
                                    std::string file_path = folder_entry.path().string();
                                    if (file_path != ASM.get_full_path("Fonts", "Fonts.txt")) {
                                        if (ImGui::BeginDragDropSource()) {
                                            ImGui::SetDragDropPayload("FONT_ITEM", file_path.c_str(), file_path.length() + 1);
                                            ImGui::Text("Dragging: %s", folder_entry.path().filename().string().c_str());
                                            ImGui::EndDragDropSource();
                                        }
                                    }
                                    else {
                                        ImGui::Text("Dragging Not Available");
                                    }
                                }
                                ImGui::Text(folder_entry.path().filename().string().c_str());
                            }
                            ImGui::NextColumn();
                        }

                    }
                    catch (const std::filesystem::filesystem_error& e) {
                        ImGui::Text("Error accessing current directory: %s", e.what());

                    }
                }
                else {
                    if (!show_msg) {
                        show_msg = true;
                    }
                }   
            }
        }
        ImGui::End();
    }

    // ------------------------------------- Functions for Functionality and Optimisation -------------------------------------//
    //Function that toggles the button
    bool IMGUI_Manager::button_toggle(const std::string& boolean_name, bool* state) {

        bool clicked = ImGui::Button(boolean_name.c_str());
        if (clicked) {
            *state = !(*state);
        }

        return clicked;
    }

    //Function to add game objects
    void IMGUI_Manager::add_game_objects() {

        ImGui::Begin("Add Game Object", &create_game_obj);

        std::vector <const char*> prefab_names_c_str{};

        //Converts to C-String
        for (const std::string& name : prefab_names) {
            prefab_names_c_str.push_back(name.c_str());
        }

        const char** prefab_options = prefab_names_c_str.data();

        static int selected_item = -1;

        if (ImGui::Combo("Clone from Prefab Options", &selected_item, prefab_options, static_cast<int>(prefab_names_c_str.size()))) {

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
                }
                
                LM.write_log("IMGUI_Manager::add_game_objects(): New entity added: %i", new_entity);
            }
            else {
                LM.write_log("IMGUI_Manager::add_game_objects(): No new entity added");
            }
        }

        ImGui::End();
    }

    //Function to remove game objects
    void IMGUI_Manager::remove_game_objects(int index) {

        const auto& entities = ecs.get_entities();
        EntityID eid = entities[index]->get_id();

        //If entity exists
        if (eid != INVALID_ENTITY_ID) {
            ECSM.destroy_entity(eid);
            LM.write_log("IMGUI_Manager::remove_game_objects(): Removed entity: %i", eid);
            remove_game_obj = !remove_game_obj;
            return;
        }
    }

    //For all text input (includes buffer)
    void IMGUI_Manager::text_input(std::string& data_name, std::string& codition_name) {

        char Buffer[128];
        //strncpy_s is safer
        strncpy_s(Buffer, data_name.c_str(), sizeof(Buffer));
        Buffer[sizeof(Buffer) - 1] = '\0';

        if (ImGui::InputText(codition_name.c_str(), Buffer, sizeof(Buffer))) {
            
            //replaces the data with the input
            data_name = std::string(Buffer);
        }
    }

    //to fill prefab names
    void IMGUI_Manager::fill_prefab_names(const char* prefab_name) {
        prefab_names.push_back(prefab_name);
    }

    //set scene loaded
    void IMGUI_Manager::set_current_file_shown(std::string current_file) {
        current_file_shown = current_file;
    }


    //To set the payload (Update once extract filename)
    void IMGUI_Manager::set_drag_drop_source(std::string current_directory, std::string folder_name, std::string file_path, std::string payload_name) {
        
        if (current_directory == ASM.get_full_path(folder_name, "")) {
            if (ImGui::BeginDragDropSource()) {
                ImGui::SetDragDropPayload(payload_name.c_str(), file_path.c_str(), file_path.length() + 1);
                std::string file_name = get_filename_from_filepath(file_path, folder_name);
                ImGui::Text("Dragging: %s", file_name.c_str());
                ImGui::EndDragDropSource();
            }
        }
    }

    std::string IMGUI_Manager::get_filename_from_filepath(std::string file_path, std::string folder_name) {
        file_path.erase(0, ASM.get_full_path(folder_name, "").length());
        std::string file_name = file_path.substr(0, file_path.find_last_of('.'));
        return file_name;
    }

    //gets the scene loaded
    std::string IMGUI_Manager::get_current_file_shown() {
        return current_file_shown;
    }

    void IMGUI_Manager::asset_browser_pop_up(bool& show_msg, const char* popup_name, const char* message) {
        if (show_msg) {
            ImGui::OpenPopup(popup_name);

            ImVec2 popup_position = ImVec2(750, 750);
            ImGui::SetNextWindowPos(popup_position);

            if (ImGui::BeginPopup(popup_name)) {

                //ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
                ImVec4 text_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Text, text_color);

                ImGui::NewLine();
                ImGui::Text(message);
                ImGui::NewLine();

                ImGui::PopStyleColor();
                //ImGui::PopFont();

                if (ImGui::Button("Close Message")) {
                    show_msg = false;
                }

                ImGui::EndPopup();
            }
        }
    }

#if 1
    //render
    void IMGUI_Manager::render() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
#endif

    //shut down
    void IMGUI_Manager::shut_down() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        LM.write_log("IMGUI_Manager::shut_down(): IMGUI_Manager shut down successfully.");
    }

} // namespace lof
