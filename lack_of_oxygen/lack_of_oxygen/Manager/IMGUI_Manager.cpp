/**
 * @file IMGUI_Manager.cpp
 * @brief Declaration of the IMGUI_Manager class for running the IMGUI level editor.
 * @author Liliana Hanawardani (83%), Saw Hui Shan (13%), Amanda Leow Boon Suan (3%), Simon Chan (1%),
 * @date November 8, 2024
 * Copyright (C) 2025 DigiPen Institute of Technology.
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

    //Variables to note down indexes of files or objects
    int selected_file_index = -1;
    int prev_file_selected = -1;
    int selected_object_index = -1;

    //Booleans to open IMGUI windows or execute functionalities
    bool show_window = false;
    bool remove_game_obj = false;
    bool create_game_obj = false;

    //Boolean for mouse dragging
    static bool mouse_clicked_or_dragged = false;

    //Mouse position in terms of game world
    ImVec2 mouse_texture_coord_world{};

    //Vectors for Animation names
    std::vector<std::string> loaded_animations;

    //Stores all animation names in storage in c-string
    std::vector<const char*> c_str_animation_storage;

    //Booleans to note down mouse behaviour
    static bool mouse_was_down = false;
    static ImVec2 mouse_pos_before_press;

    //Helper function to make every character lower-case (for texture identification in storage)
    char to_lower(char c) {
        return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }

    //Locally stored vector to store all the component checks (add and remove component functions)
    std::vector<std::tuple<const char*, ComponentID, std::function<void()>, std::function<void()>>> component_checks;

    //Locally stored function to fill component check (Logic component commented out)
    void fill_component_checks() {
        auto& entities = ECSM.get_entities();
        component_checks = {
            {"Transform Component", static_cast<ComponentID>(ECSM.get_component_id<Transform2D>()),
                [&]() { ECSM.add_component<Transform2D>(entities[selected_object_index]->get_id(), Transform2D()); },
                [&]() { ECSM.remove_component<Transform2D>(entities[selected_object_index]->get_id()); }},

            {"Velocity Component", static_cast<ComponentID>(ECSM.get_component_id<Velocity_Component>()),
                [&]() { ECSM.add_component<Velocity_Component>(entities[selected_object_index]->get_id(), Velocity_Component()); },
                [&]() { ECSM.remove_component<Velocity_Component>(entities[selected_object_index]->get_id()); }},

            {"Physics Component", static_cast<ComponentID>(ECSM.get_component_id<Physics_Component>()),
                [&]() { ECSM.add_component<Physics_Component>(entities[selected_object_index]->get_id(), Physics_Component()); },
                [&]() { ECSM.remove_component<Physics_Component>(entities[selected_object_index]->get_id()); }},

            {"Graphics Component", static_cast<ComponentID>(ECSM.get_component_id<Graphics_Component>()),
                [&]() { ECSM.add_component<Graphics_Component>(entities[selected_object_index]->get_id(), Graphics_Component()); },
                [&]() { ECSM.remove_component<Graphics_Component>(entities[selected_object_index]->get_id()); }},

            {"Collision Component", static_cast<ComponentID>(ECSM.get_component_id<Collision_Component>()),
                [&]() { ECSM.add_component<Collision_Component>(entities[selected_object_index]->get_id(), Collision_Component()); },
                [&]() { ECSM.remove_component<Collision_Component>(entities[selected_object_index]->get_id()); }},

            {"Animation Component", static_cast<ComponentID>(ECSM.get_component_id<Animation_Component>()),
                [&]() { ECSM.add_component<Animation_Component>(entities[selected_object_index]->get_id(), Animation_Component()); },
                [&]() { ECSM.remove_component<Animation_Component>(entities[selected_object_index]->get_id()); }},

            {"Logic Component", static_cast<ComponentID>(ECSM.get_component_id<Logic_Component>()),
                [&]() { ECSM.add_component<Logic_Component>(entities[selected_object_index]->get_id(), Logic_Component()); },
                [&]() { ECSM.remove_component<Logic_Component>(entities[selected_object_index]->get_id()); }},

            {"Audio Component", static_cast<ComponentID>(ECSM.get_component_id<Audio_Component>()),
                [&]() { ECSM.add_component<Audio_Component>(entities[selected_object_index]->get_id(), Audio_Component()); },
                [&]() { ECSM.remove_component<Audio_Component>(entities[selected_object_index]->get_id()); }},

            {"Text Component", static_cast<ComponentID>(ECSM.get_component_id<Text_Component>()),
                [&]() { ECSM.add_component<Text_Component>(entities[selected_object_index]->get_id(), Text_Component()); },
                [&]() { ECSM.remove_component<Text_Component>(entities[selected_object_index]->get_id()); }},
        };
    }

    IMGUI_Manager::IMGUI_Manager() : ecs(ECSM) {}

    IMGUI_Manager::IMGUI_Manager(ECS_Manager& ecs_manager) : ecs(ecs_manager) {
        set_type("IMGUI_Manager");
    }

    IMGUI_Manager& IMGUI_Manager::get_instance() {
        static IMGUI_Manager instance;
        return instance;
    }

    //Inherited start_up function, not using
    int IMGUI_Manager::start_up() {
        throw std::runtime_error("No-parameter start_up() is disabled in IMGUI_Manager. start_up() now has a parameter GLFWwindow*& window");
    }

    void IMGUI_Manager::fill_up_animation_storage() {

        auto& animation_storage = ASM.get_animation_storage();
        for (auto& animation : animation_storage) {
            c_str_animation_storage.push_back(animation.first.c_str());
        }
    }

    //Start up function
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
        fill_component_checks();
        fill_up_animation_storage();

        return 0;
    }

    //Function to fill up sound types
    void IMGUI_Manager::fill_up_sound_names() {

        //Fill in audio_types vector 
        audio_types.push_back(std::make_pair("BGM", (AudioType)0));
        audio_types.push_back(std::make_pair("SFX", (AudioType)1));
        audio_types.push_back(std::make_pair("UI", (AudioType)2));
        audio_types.push_back(std::make_pair("NIL", (AudioType)3));
    }

    //Function to get scene files from the scene folder
    std::vector<std::string> IMGUI_Manager::get_scene_files() {
        
        std::vector<std::string> file_names;

        //Gets filepath to scene file with Asset Manager
        std::string level_path = ASM.get_full_path("Scenes", "");

        for (const auto& entry : std::filesystem::directory_iterator(level_path)) {
            if (entry.is_regular_file()) {

                //Push back file names
                file_names.push_back(entry.path().filename().string());
            }
        }
        return file_names;
    }

    //Function to initiate loading a new scene
    void IMGUI_Manager::load_scene(const std::string& file_name) {

        //Stop all the sounds before 
        ADM.stop_mastergroup();
        ADM.set_new_scene(true);

        const std::string scenes = "Scenes";

        //Loads the specific scene via the Serialization Manager and set it with the Game Manager
        if (SM.load_scene(ASM.get_full_path(scenes, file_name).c_str())) {

            auto it = std::find_if(file_name.begin(), file_name.end(), ::isdigit);

            if (it != file_name.end()) {
                GM.set_current_scene(std::stoi(std::string(it, file_name.end())));
            }
            else if (file_name == "credit.scn") {
                GM.set_current_scene(3);
            }
            else if (file_name == "main_menu.scn") {
                GM.set_current_scene(0);
            }
            else if (file_name == "win_screen.scn") {
                GM.set_current_scene(4);
            }
            else if (file_name == "tutorial.scn") {
                GM.set_current_scene(5);
            }

            //Reset int used for file list
            selected_object_index = -1;

            //Reset camera position after loading scene
            auto& camera = GFXM.get_camera();
            camera.pos_x = DEFAULT_CAMERA_POS_X;
            camera.pos_y = DEFAULT_CAMERA_POS_Y;

            //Restart the oxygen levels after loading a new scenes
            if (GM.get_current_oxygen_level() < 100) {
                GM.set_current_oxygen_level(100);
            }
 
            //Note down current file
            set_current_file_shown(file_name);

            //Reset audio names
            audio_file_names.clear();
            audio_types.clear();
            fill_up_sound_names();

        }
    }

    //Function to display the file list
    void IMGUI_Manager::display_loading_options() {

        ImGui::Begin("File List");
        
        std::vector<std::string> file_names = get_scene_files();

        static std::string selected_file{};

        //Iterate through file names
        for (int i = 0; i < file_names.size(); ++i) {

            if (!file_names[i].empty()) {

                //Selectable for clicking; Second parameter for highlighting
                if (ImGui::Selectable(file_names[i].c_str(), selected_file_index == i)) {

                    //Selected; Caused second parameter to change
                    selected_file_index = i;
                    selected_file = file_names[i];
                }

            }
        }

        //Load scene button
        if (ImGui::Button("Load Scene") && (selected_file_index != -1) && !selected_file.empty()) {
            load_scene(selected_file);
        }

        ImGui::Separator();

        //Button for dragged scenes from asset browser to load
        ImGui::Button("Drop Scenes Here To Load");
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENES_ITEM")) {
                const char* droppedFilePath = (const char*)payload->Data; 
                std::string file_name = droppedFilePath;
                file_name.erase(0, ASM.get_full_path("Scenes", "").length());
                load_scene(file_name);
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::End();
    }

    //Starts IMGUI frame
    void IMGUI_Manager::start_frame() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    //Function to get mouse position in terms of game world coordinates
    ImVec2 IMGUI_Manager::get_imgui_mouse_pos(ImVec2 texture_pos, ImVec2 mouse_pos, unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT) {

        //Checks if mouse is within texture
        if (mouse_pos.x >= texture_pos.x && mouse_pos.x <= (texture_pos.x + SCR_WIDTH / 2) &&
            mouse_pos.y >= texture_pos.y && mouse_pos.y <= (texture_pos.y + SCR_HEIGHT / 2)) {

            //Gets position of mouse in terms of texture (viewport) coordinates
            ImVec2 mouse_texture_coord_screen{};
            mouse_texture_coord_screen.x = (mouse_pos.x - texture_pos.x);
            mouse_texture_coord_screen.y = (mouse_pos.y - texture_pos.y);

            //Get camera's position in game world
            auto& camera = GFXM.get_camera();

            //Calculate scale factors to normalise coordinates for window resolution changes
            float normalise_x = static_cast<float>(SCR_WIDTH) / SM.get_scr_width();
            float normalise_y = static_cast<float>(SCR_HEIGHT) / SM.get_scr_height();

            //Gets x position of mouse in terms of which quadrant it lands in the game world
            mouse_texture_coord_world.x = mouse_texture_coord_screen.x;
            if (mouse_texture_coord_screen.x < (SCR_WIDTH / 4)) {
                mouse_texture_coord_world.x = -((SCR_WIDTH / 4) - mouse_texture_coord_screen.x);
            }
            else {
                mouse_texture_coord_world.x = mouse_texture_coord_screen.x - (SCR_WIDTH / 4);
            }

            //Scale to account for original game world size (window size)
            mouse_texture_coord_world.x *= 2;

            //Normalising x position to account for window resolution changes; Add the camera's position
            mouse_texture_coord_world.x = (mouse_texture_coord_world.x / normalise_x);
            mouse_texture_coord_world.x += camera.pos_x;

            //Do the same for y
            mouse_texture_coord_world.y = mouse_texture_coord_screen.y;
            if (mouse_texture_coord_screen.y <= (SCR_HEIGHT / 4)) {
                mouse_texture_coord_world.y = (SCR_HEIGHT / 4) - mouse_texture_coord_screen.y;
            }
            else {
                mouse_texture_coord_world.y = -(mouse_texture_coord_screen.y - (SCR_HEIGHT / 4));
            }

            mouse_texture_coord_world.y *= 2;
            mouse_texture_coord_world.y = (mouse_texture_coord_world.y / normalise_y);
            mouse_texture_coord_world.y += camera.pos_y;
        }

        //Return mouse in terms of game world
        return mouse_texture_coord_world;
    }

    //Getter to return mouse position
    ImVec2 IMGUI_Manager::imgui_mouse_pos() {
        return mouse_pos_game;
    }

    bool select_entity = false; //to ensure mouse click selected
    EntityID selectedEntityID = static_cast<EntityID>(-1);

#if 1

    //Static bool to keep track if the mouse is in the viewport
    static bool mouse_in_window = false;

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

        static int currentMode = 0; //Default to "Drag"
        const char* modes[] = { "None","Drag", "Scale", "Rotate" }; //Options for picking

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

            ImGui::SameLine();

            //If game is playing (Not paused)
            if (game_playing) {

                //Show text in Green
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0, 1.0, 0.0, 1.0));
                ImGui::Text("GAME IS PLAYING");

                //Show button text in red
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0));
                if (ImGui::Button("PAUSE")) {
                    game_playing = false;
                }

                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
            }
            else {

                //Show text in red
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0));
                ImGui::Text("GAME IS PAUSED");

                //Showed button text in green
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0, 1.0, 0.0, 1.0));
                if (ImGui::Button("PLAY")) {
                    game_playing = true;
                }

                ImGui::PopStyleColor();
                ImGui::PopStyleColor();

            }

            ImGui::SameLine();

            //Stop button
            if (ImGui::Button("STOP")) {

                load_scene(get_current_file_shown());
                game_playing = false;
            }

            ImGui::NewLine();


            //Viewport texture
            auto texture = GFXM.get_framebuffer_texture();
            ImVec2 texture_pos = ImGui::GetCursorScreenPos();

            if (texture) {
                ImGui::Image((ImTextureID)(intptr_t)GFXM.get_framebuffer_texture(),
                    ImVec2(static_cast<float>(SCR_WIDTH) / 2, static_cast<float>(SCR_HEIGHT) / 2),
                    ImVec2(0, 1), ImVec2(1, 0));
            }

            //Set up viewport as drag drop target for scenes
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENES_ITEM")) {
                    const char* droppedFilePath = (const char*)payload->Data;
                    std::string file_name = droppedFilePath;
                    file_name.erase(0, ASM.get_full_path("Scenes", "").length());

                    load_scene(file_name);

                }
                ImGui::EndDragDropTarget();
            }

            //Ensure that the mouse is in the viewport or using the mouse pop-up
            if (ImGui::IsPopupOpen("OperationContextMenu") || ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem)) {
                mouse_in_window = true;
            }
            else {
                mouse_in_window = false;
            }

            //Get the mouse in game world position
            ImVec2 mouse_pos = ImGui::GetIO().MousePos;
            mouse_pos_game = get_imgui_mouse_pos(texture_pos, mouse_pos, SCR_WIDTH, SCR_HEIGHT);
            if (mouse_in_window) {
                ImGui::Text("Mouse in terms of world at: (%.2f, %.2f)", mouse_pos_game.x, mouse_pos_game.y);
            }

            //Check for collision of mouse with entity
            ESS.Check_Selected_Entity();
            EntityInfo& selectedEntityInfo = ESS.get_selected_entity_info();

            //Only executes when mouse is in the viewport or using the mouse pop-up
            if (mouse_in_window) {

                //Check if mouse is within texture bounds
                if (mouse_pos.x >= texture_pos.x && mouse_pos.x <= (texture_pos.x + SCR_WIDTH / 2) &&
                    mouse_pos.y >= texture_pos.y && mouse_pos.y <= (texture_pos.y + SCR_HEIGHT / 2)) {

                    //Handle entity selection with left click
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

                    //Show context menu on right click when an entity is selected
                    if (selectedEntityID != INVALID_ENTITY_ID && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                        ImGui::OpenPopup("OperationContextMenu");
                    }

                    //Position the context menu at the mouse position
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

                    //Handle mouse operations
                    static ImVec2 selected_entity_start_pos;
                    auto& entities = ecs.get_entities();

                    if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && selectedEntityID != INVALID_ENTITY_ID) {
                        if (!mouse_was_down) {
                            mouse_pos_before_press = get_imgui_mouse_pos(texture_pos, mouse_pos, SCR_WIDTH, SCR_HEIGHT);
                            if (selectedEntityInfo.isSelected) {
                                if (entities[selectedEntityID]->has_component(ecs.get_component_id<Transform2D>())) {
                                    Transform2D& transform = ecs.get_component<Transform2D>(entities[selectedEntityID].get()->get_id());
                                    //Store initial values based on operation type
                                    switch (currentMode) {
                                    case 0: //None - do nothing
                                        break;
                                    case 1: //Drag
                                        selected_entity_start_pos.x = transform.position.x;
                                        selected_entity_start_pos.y = transform.position.y;
                                        break;
                                    case 2: //Scale
                                        selected_entity_start_pos.x = transform.scale.x;
                                        selected_entity_start_pos.y = transform.scale.y;
                                        break;
                                    case 3: //Rotate
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
                        case 1: //Drag
                        {
                            ImVec2 dragged_offset;
                            dragged_offset.x = mouse_pos_game.x - mouse_pos_before_press.x;
                            dragged_offset.y = mouse_pos_game.y - mouse_pos_before_press.y;

                            if (selectedEntityInfo.isSelected && selectedEntityID != INVALID_ENTITY_ID) {
                                if (entities[selectedEntityID]->has_component(ecs.get_component_id<Transform2D>())) {
                                    Transform2D& transform = ecs.get_component<Transform2D>(entities[selectedEntityID].get()->get_id());
                                    transform.position.x = selected_entity_start_pos.x + dragged_offset.x;
                                    transform.position.y = selected_entity_start_pos.y + dragged_offset.y;
                                    transform.prev_position = transform.position;
                                }
                            }
                            break;
                        }
                        case 2: //Scale
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
            }

            //Showing EntityID at the bottom
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

    //Function to ensure GUI is disabled
    void IMGUI_Manager::disable_GUI() {

        for (auto& system : ECSM.get_systems()) {
            if (auto* gui_system = dynamic_cast<GUI_System*>(system.get())) {

                gui_system->hide_mineral_tank_gui();
                gui_system->hide_oxygen_tank_gui();
                gui_system->hide_wormhole_gui();

            }
        }
    }

    //Function to handle hierarchy object list
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

                //Selectable for clicking; Second parameter for highlighting
                if (ImGui::Selectable(obj_name.c_str(), selected_object_index == current_object_index) && !mouse_clicked_or_dragged) {

                    //Selected; Caused second parameter state to change
                    selected_object_index = current_object_index;
                }
            }
            ++current_object_index;
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

    //Static map to store buffers for each sound by index or key
    static std::unordered_map<int, std::string> buffer_map;

    //String to hold the requested animation
    static std::string requested_animation = "";

    //Function to handle edit object properties window
    void IMGUI_Manager::imgui_game_objects_edit() {

        static int last_selected_object_index = -1; //Track the previous selected object index

        ImGui::Begin("Edit Object Properties");

        const auto& entities = ecs.get_entities();

        //Edit button
        ImGui::Text("Selected Game Object Functionalities:\n");
        if (ImGui::Button("Edit Game Object")) {
            show_window = !show_window;
        }

        //Remove button; Disabled for when the Player entity is selected
        ImGui::BeginDisabled(selected_object_index != -1 && entities[selected_object_index].get()->get_name() == "player1");
        if (ImGui::Button("Remove Game Object")) {
            remove_game_obj = !remove_game_obj;
        }
        ImGui::EndDisabled();

        //Create Game Object button
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

        //Push names of animations in storage to the loaded_animations vector
        auto& animation_storage = ASM.get_animation_storage();
        for (auto& animation : animation_storage) {
            loaded_animations.push_back(animation.first);
        }
        
        if (!show_window) {
            
        }
        else {
            

            //If the selected object has changed, reset filled and clear assigned names for new object
            if (selected_object_index != last_selected_object_index) {
                last_selected_object_index = selected_object_index;              
                
                buffer_map.clear();

            }

            if (selected_object_index == -1) {
                //ImGui::Text("Select a game object to edit it.");
            }
            else {

                ImGui::NewLine();
                ImGui::Separator();
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

                        auto& max_velocity_sq = physics.get_max_velocity_sq();
                        ImGui::Text("Max Velocity Square: %i", max_velocity_sq);

                        auto& accumulated_force = physics.get_accumulated_force();
                        ImGui::InputFloat2("Accumulated Force", &accumulated_force.x);

                        auto& acceleration = physics.get_acceleration();
                        ImGui::InputFloat2("Acceleration", &acceleration.x);

                        auto& mass = physics.get_mass();
                        ImGui::InputFloat("Mass", &mass);

                        ImGui::BeginDisabled();
                        auto& inv_mass = physics.get_inv_mass();
                        ImGui::InputFloat("inv_mass", &inv_mass);
                        ImGui::EndDisabled();

                        auto& is_static = physics.get_is_static();
                        std::string s_label = "is_static: " + std::string(is_static_on ? "On" : "Off");
                        if (button_toggle(s_label, &is_static_on)) {
                            is_static = !is_static;
                        }

                        ImGui::BeginDisabled();
                        auto& is_has_jumped = physics.get_has_jumped();
                        std::string j_label = "is_has_jumped: " + std::string(is_has_jumped ? "On" : "Off");
                        if (button_toggle(j_label, &is_has_jumped)) {
                            is_has_jumped = !is_has_jumped;
                        }

                        auto& is_jump_requested = physics.get_jump_requested();
                        std::string jr_label = "is_jump_requested: " + std::string(is_jump_requested ? "On" : "Off");
                        if (button_toggle(jr_label, &is_jump_requested)) {
                            is_jump_requested = !is_jump_requested;
                        }
                        ImGui::EndDisabled();

                        auto& jump = physics.get_jump_force();
                        ImGui::InputFloat("Jump Force", &jump);

                        /*auto& is_grounded = physics.get_is_grounded();
                        std::string g_label = "is_grounded: " + std::string(is_grounded_on ? "On" : "Off");
                        if (button_toggle(g_label, &is_grounded_on)) {
                            is_grounded = !is_grounded;
                        }*/
                        
                    }
                }

                //Graphics Component
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

                        //Identify if Entity is a button
                        std::string is_button = "_button";
                        size_t found = Name.find(is_button);

                        //If it is a button
                        if (found != std::string::npos) {
                                
                            ImGui::NewLine();
                            ImGui::Text("Note: The textures for buttons are in assigned in batches.\nPlease assign textures with these in them:");
                            for (auto& batch : batches) {
                                ImGui::Text(batch.c_str());
                            }
                            ImGui::NewLine();
                            ImGui::Separator();

                            size_t pos = texture_name.find_last_of('_');  // Find first underscore
                            std::string batch_name = texture_name.substr(0, pos);
                            ImGui::Text("Current Batch: %s", batch_name.c_str());

                            char buffer_graphics[128];
                            strncpy_s(buffer_graphics, texture_name.c_str(), sizeof(buffer_graphics));
                            buffer_graphics[sizeof(buffer_graphics) - 1] = '\0';
                            if (ImGui::InputText(condition_name_texture.c_str(), buffer_graphics, sizeof(buffer_graphics))) {
                                std::string buffer_string = std::string(buffer_graphics);

                                texture_name = buffer_string;

                                //Extracts the base texture and updates it being assigned to the corresponding button in the vector
                                for (std::string name : batches) {

                                    size_t name_found = texture_name.find(name);

                                    if (name_found != std::string::npos) {
                                        for (auto& paired : batch_and_button) {

                                            if (Name == paired.first) {
                                                paired.second = name;
                                            }
                                        }
                                    }
                                }
                                
                            }

                            //For the drag drop target of the texture for button entity
                            if (ImGui::BeginDragDropTarget()) {

                                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_ITEM")) {
                                    const char* droppedFilePath = (const char*)payload->Data;
                                    std::string file_name = droppedFilePath;
                                    file_name.erase(0, ASM.get_full_path("Textures", "").length());
                                    file_name = file_name.substr(0, file_name.size() - 4);

                                    texture_name = file_name;

                                    //Do the same
                                    for (std::string name : batches) {

                                        size_t batch_found = texture_name.find(name);

                                        if (batch_found != std::string::npos) {

                                            for (auto& paired : batch_and_button) {

                                                if (Name == paired.first) {
                                                    paired.second = name;
                                                }
                                            }
                                        }
                                    }
                                }

                                ImGui::EndDragDropTarget();
                            }
                            
                        }
                        else { //If entity is not a button

                            //text_input function is not used due to using to_lower
                            char buffer_graphics[128];
                            strncpy_s(buffer_graphics, texture_name.c_str(), sizeof(buffer_graphics));
                            buffer_graphics[sizeof(buffer_graphics) - 1] = '\0';
                            if (ImGui::InputText(condition_name_texture.c_str(), buffer_graphics, sizeof(buffer_graphics))) {
                                std::string buffer_string = std::string(buffer_graphics);
                                std::transform(buffer_string.begin(), buffer_string.end(), buffer_string.begin(), to_lower);
                                texture_name = buffer_string;
                            }

                            //For drag drop target of texture
                            if (ImGui::BeginDragDropTarget()) {

                                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_ITEM")) {
                                    const char* droppedFilePath = (const char*)payload->Data;
                                    std::string file_name = droppedFilePath;
                                    file_name.erase(0, ASM.get_full_path("Textures", "").length());
                                    file_name = file_name.substr(0, file_name.size() - 4);

                                    std::transform(file_name.begin(), file_name.end(), file_name.begin(), to_lower);
                                    texture_name = file_name;
                                }

                                ImGui::EndDragDropTarget();
                            }
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

                    if (ImGui::CollapsingHeader("Animation")) {

                        //Displays index playing at that time
                        auto& curr = animation.curr_animation_idx;
                        ImGui::Text("Current Animation Index Playing %i", curr);
                        ImGui::NewLine();

                        ImGui::Separator();

                        //Getting animation indexes of entity
                        auto& animation_list = animation.animations;

                        ImGui::Text("Add Animation Index:");
                        ImGui::Text("Select an animation and click add to add it as another animation index");
                        static int selected_animation_idx = -1;

                        //Dropdown for selecting an animation
                        std::string combo_label = "Select Animation##combo";
                        if (ImGui::Combo(combo_label.c_str(), &selected_animation_idx, c_str_animation_storage.data(), static_cast<int>(c_str_animation_storage.size()))) {
                            
                            //Stores selected animation name in requested_animation string for processing
                            if (selected_animation_idx >= 0 && selected_animation_idx < c_str_animation_storage.size()) {
                                requested_animation = c_str_animation_storage[selected_animation_idx];
                            }
                        }

                        if (ImGui::Button("Add Animation and Index")) {

                            //Getting a new index
                            //Start with the size of the current animation_list
                            int missing_index = static_cast<int>(animation_list.size());
                            int prev_index_check = -1;

                            //Iterate through indexes to find largest missing index
                            for (auto& animation_indexes : animation_list) {
                                int index_animation = std::stoi(animation_indexes.first);
                                if (index_animation - 1 != prev_index_check) {
                                    missing_index = prev_index_check + 1;
                                    break;
                                }
                                prev_index_check = index_animation;
                            }

                            //Prevent indexes from reaching 10 and above
                            if (missing_index >= 10) {
                                missing_index = 0;
                            }

                            //Match requested animation to animation anme in storage
                            for (auto& animation_info : animation_storage) {

                                if (requested_animation == animation_info.first) {
                                    animation_list.insert({ std::to_string(missing_index), requested_animation });
                                    break;
                                }
                            }

                        }

                        //Open the popup when the button is clicked
                        ImGui::Text("Remove Animation Index:");
                        if (ImGui::Button("Remove Index")) {
                            ImGui::OpenPopup("Animation Index Removal"); 
                        }

                        //Position the context menu at the mouse position
                        if (ImGui::BeginPopup("Animation Index Removal")) {
                            ImGui::Text("Select Animation Index to Remove");
                            ImGui::Separator();

                            int removal_index = 0;
                            for (const auto& remove_animation : animation_list) {
                                if (ImGui::Selectable(remove_animation.first.c_str())) {

                                    //Remove selected animation except the first
                                    if (removal_index != 0) {
                                        animation_list.erase(remove_animation.first); 
                                    }
                                    break; 
                                }
                                removal_index++;
                            }

                            ImGui::EndPopup();
                        }

                        ImGui::NewLine();

                        //Vector to keep track of selection of all indexes in animation
                        static std::vector<int> selected_items(animation_list.size(), -1);

                        //Index of the action to select animation
                        int index = 0;
                        for (auto it = animation_list.begin(); it != animation_list.end(); ++it, ++index) {
                            
                            //Ensures the selected_items vector is the same size as the animation_list
                            if (selected_items.size() != animation_list.size()) {
                                selected_items.resize(animation_list.size(), -1);
                            }
                            ImGui::Text("Selected Animation for Index %s: %s", it->first.c_str(), it->second.c_str());
                            std::string label = "Index " + std::to_string(index);
                            
                            //Dropdown for the animation
                            if (ImGui::Combo(label.c_str(), &selected_items[index], c_str_animation_storage.data(), static_cast<int>(c_str_animation_storage.size()))) {
                                //If valid animation is selected, update the animation.
                                if (selected_items[index] >= 0 && selected_items[index] < c_str_animation_storage.size()) {
                                    //Update the animation's name
                                    it->second = c_str_animation_storage[selected_items[index]];
                                }
                            }
                        }

                        ImGui::Separator();
                        ImGui::NewLine();          
         
                    }
                }

                //Vector to store all script names from logic manager
                std::vector<std::string> lgm_scripts = LGM.get_script_names();

                //Logic Component
                if (entities[selected_object_index]->has_component(ecs.get_component_id<Logic_Component>())) {
                    Logic_Component& logic = ecs.get_component<Logic_Component>(entities[selected_object_index].get()->get_id());

                    //Get all logic associated with entity
                    auto& scripts = logic.get_logic_datas();

                    //All script names in entities
                    std::unordered_set<std::string> scripts_in_entity;

                    //Tracking used and unused scripts
                    std::vector<std::string> used_scripts;
                    std::vector<std::string> unused_scripts;

                    //Insert all names
                    for (const auto& script : scripts) {          
                        scripts_in_entity.insert(script->script_name);
                    }

                    for (const auto& lgm_script : lgm_scripts) {

                        //Only add scripts that are not already used
                        if (scripts_in_entity.find(lgm_script) == scripts_in_entity.end()) {
                            unused_scripts.push_back(lgm_script);
                        }
                        else {

                            //Else add them to used_scripts
                            used_scripts.push_back(lgm_script);
                        }
                    }

                    if (ImGui::CollapsingHeader("Logic")) {

                        if (ImGui::Button("Add New Script")) {

                            //Add script labelled "HI"
							const std::string DEFAULT_SCRIPT_HEADER = DEFAULT_SCRIPT + std::to_string(logic.get_logic_datas().size());
                            logic.add_script(DEFAULT_SCRIPT);
                        }
                     
                        if (ImGui::Button("Remove Script")) {
                            ImGui::OpenPopup("RemoveScript");
                        }

                        //Position the context menu at the mouse position
                        if (ImGui::BeginPopup("RemoveScript")) {
                            ImGui::Text("Select Script");
                            ImGui::Separator();

                            //Remove script from logic component
                            int index = 0;
                            for (const auto& script_name_in_entity : scripts_in_entity) {
                                if (ImGui::Selectable(script_name_in_entity.c_str())) {
                                    logic.remove_script(script_name_in_entity);
                                    break;
                                }
                                index++;
                            }

                            ImGui::EndPopup();
                        }

                        //Headers for each script in component
                        int i = 0;
                        for (auto& script : scripts) {

                            std::string& script_name = script->script_name;
							std::string script_header = "Script: " + std::to_string(i);
							if (ImGui::CollapsingHeader(script_header.c_str())) {


                                ImGui::BeginDisabled();

                                ImGui::Text("Script Name: %s", script->script_name.c_str());
                                ImGui::EndDisabled();

                                std::string button_label = "Choose Different Script For " + std::to_string(i);
                                std::string dropdown_label = "ChoosingDiffScriptFor" + std::to_string(i);
                                if (ImGui::Button(button_label.c_str())) {
                                    ImGui::OpenPopup(dropdown_label.c_str());
                                }

                                //Position the context menu at the mouse position
                                if (ImGui::BeginPopup(dropdown_label.c_str())) {
                                    ImGui::Text("Select Script");
                                    ImGui::Separator();

                                    int index = 0;
                                    for (const auto& script_name_in_list : unused_scripts) {
                                        if (ImGui::Selectable(script_name_in_list.c_str())) {
                                            script->script_name = script_name_in_list;
                                            break; 
                                        }
                                        index++;
                                    }

                                    ImGui::EndPopup();
                                }

                                auto& is_active = script->is_active;
                                std::string s_label = "Active for " + std::to_string(i);

                                //Display and toggle the active state
                                if (ImGui::Checkbox(s_label.c_str(), &is_active)) {
                                    logic.set_active(script_name, is_active);
                                }

                                //Display Execution State (Read-Only)
                                const char* execution_state_str = "";
                                switch (script->state) {
                                    case ExecutionState::Uninitialized: execution_state_str = "Uninitialized"; break;
                                    case ExecutionState::Running: execution_state_str = "Running"; break;
                                    case ExecutionState::Paused: execution_state_str = "Paused"; break;
                                    case ExecutionState::Terminated: execution_state_str = "Terminated"; break;
                                    default: execution_state_str = "Unknown"; break;
                                }
                                ImGui::Text("Execution State: %s", execution_state_str);

							}

                            ++i;
                        }

                        ImGui::NewLine();

                    }
                }

                //Audio Component
                if (entities[selected_object_index]->has_component(ecs.get_component_id<Audio_Component>())) {
                    Audio_Component& audio = ecs.get_component<Audio_Component>(entities[selected_object_index].get()->get_id());
                    if (ImGui::CollapsingHeader("Audio")) {

                        //Add new audio button that adds new sound to component
                        if (ImGui::Button("Add New Audio")) {
                            const std::string DEFAULT_KEY = DEFAULT_AUDIO_KEY + std::to_string(audio.get_sounds().size());
                            audio.add_sound(DEFAULT_KEY, DEFAULT_AUDIO_FILEPATH, DEFAULT_AUDIO_TYPE, MIN_SIMULTANEOUS, DEFAULT_AUDIO_FLOAT,
                                DEFAULT_AUDIO_FLOAT, DEFAULT_LOOP, DEFAULT_ACTIVE, DEFAULT_IS_3D);
                        }

                        //Gets a collection of sounds from audio component (not sound map)
                        auto& sounds = audio.get_sounds();

                        static size_t selected_audio_index = static_cast<size_t>(-1);

                        //Remove audio pop-up and button
                        std::string remove_audio = "Remove Audio";
                        const char* remove_audio_button = remove_audio.c_str();
                        if (ImGui::Button(remove_audio_button)) {
                            ImGui::OpenPopup("Remove Audio");
                        }

                        if (ImGui::BeginPopup("Remove Audio")) {
                            ImGui::Text("Remove Audio");
                            ImGui::Separator();

                            for (size_t i = 0; i < sounds.size(); i++) {
                                if (ImGui::Selectable(sounds[i].key.c_str())) {
                                    selected_audio_index = i;
                                }
                            }
                            ImGui::EndPopup();
                        }

                        //Removing selected sound from component
                        if (selected_audio_index != -1) {
                            if (selected_audio_index >= 0 && selected_audio_index < sounds.size()) {  // Bounds check
                                const auto& audio_key = sounds[selected_audio_index].key;
                                audio.remove_sound(audio_key);
                            }

                            selected_audio_index = static_cast<size_t>(-1);
                        }

                        //Static vector to keep track of selected sounds(actions that need sounds)
                        static std::vector<int> selected_sounds;

                        //Clears vector to make sure its empty
                        selected_sounds.clear();

                        //resizes it to match sounds/actions (initializing everything to -1 to indicate no selection)
                        //sound size should be 3
                        selected_sounds.resize(sounds.size(), -1);

                        //For Audio Type
                        static std::vector<int> selected_sounds_type;
                        selected_sounds_type.clear();
                        selected_sounds_type.resize(sounds.size(), -1);

                        int type_index = 0;
                        for (int i = 0; i < sounds.size(); ++i, ++type_index) {

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

                                    if (sounds[i].audio_type == BGM) {
                                        for (auto& system : ECSM.get_systems()) {
                                            if (system->get_type() == "Audio_System") {
                                                auto* audio_system = static_cast<Audio_System*>(system.get());

                                                audio_system->stop_sound(sounds[i].key);
                                            }
                                        }
                                    }
                                    
                                    //Update the sound's file path based on the selected string from the sound map
                                    audio.set_filepath(sounds[i].key, sound_map_filenames[selected_index]);

                                    if (sounds[i].audio_type == BGM) {
                                        ADM.set_new_scene(true);
                                    }

                                    LM.write_log("IMGUIM:: Sound being changed to %s for %s", sound_map_filenames[selected_index].c_str(), sounds[i].key.c_str());
                                }
                            }

                            //For drag drop target of audio
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

                                        if (sounds[i].audio_type == BGM) {
                                            for (auto& system : ECSM.get_systems()) {
                                                if (system->get_type() == "Audio_System") {
                                                    auto* audio_system = static_cast<Audio_System*>(system.get());

                                                    audio_system->stop_sound(sounds[i].key);
                                                }
                                            }
                                        }

                                        ADM.load_sound(file_name, audio.get_audio_type(sounds[i].key));
                                        audio.set_filepath(sounds[i].key, file_name);
                                        ADM.set_new_scene(true);
                                    }

                                }
                                ImGui::EndDragDropTarget();
                            }

                            //Textbox for sound's key
                            std::string old_key_name = sounds[i].key;
                            std::string condition_name_key = "key for " + std::to_string(i);

                            if (buffer_map.find(i) == buffer_map.end()) {
                                buffer_map[i] = old_key_name;
                            }

                            text_input(buffer_map[i], condition_name_key);

                            //Save button for sound's key
                            std::string save = "save " + condition_name_key;
                            const char* button_name = save.c_str();
                            if (ImGui::Button(button_name)) {
                                std::string new_key_name = buffer_map[i];
                                audio.set_key(old_key_name, new_key_name);

                            }

                            //Display audio's type
                            auto audio_type = audio.get_audio_type(sounds[i].key);
                            std::vector<const char*> audio_type_cstr;

                            for (const auto& fill_audio_type_pair : audio_types) {
                                audio_type_cstr.push_back(fill_audio_type_pair.first.c_str());
                            }

                            //Find matching representative string for the file path
                            auto type_avail = std::find_if(audio_types.begin(), audio_types.end(),
                                [&audio_type](const std::pair<std::string, AudioType>& p) {
                                    return p.second == audio_type;
                                });


                            //Show the current sound's representative string in a text label
                            if (type_avail != audio_types.end()) {
                                ImGui::Text("Audio Type for %s: %s", sounds[i].key.c_str(), type_avail->first.c_str());
                            }
                            else {
                                ImGui::Text("Audio Type for %s: Not Found", sounds[i].key.c_str());
                            }

                            //Combo box to choose the new representative string for this sound
                            std::string type_label = "Choose Audio Type for " + std::to_string(type_index);
                            if (ImGui::Combo(type_label.c_str(), &selected_sounds_type[type_index], audio_type_cstr.data(), static_cast<int>(audio_type_cstr.size()))) {

                                //Get the selected representative string type_index
                                int selected_index = selected_sounds_type[type_index];
                                if (selected_index >= 0 && selected_index < audio_type_cstr.size()) {

                                    // Update the sound's file path based on the selected rep string
                                    audio.set_audio_type(sounds[i].key, audio_types[selected_index].second);

                                }
                            }

                            ImGui::Separator();
                            ImGui::NewLine();
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

                static int selected = 0;
                static std::vector<const char*> missing_components;

                //Identifying which components that the entity does not have and storing them in missing_components              
                missing_components.clear();
                missing_components.push_back("None");
                for (const auto& [name, id, add_func, remove_func] : component_checks) {
                    if (!entities[selected_object_index]->has_component(id)) {
                        missing_components.push_back(name);
                    }
                }

                //Add component functionality
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

                            //Iterate though the missing_components as options
                            const auto& component_name = missing_components[selected];
                            for (const auto& [name, id, add_func, remove_func] : component_checks) {
                                if (std::string(component_name) == name) {

                                        //Execute its add function
                                        add_func();
                                        LM.write_log("IMGUI_Manager::imgui_game_objects_list(): Added %s to %s", name, entities[selected_object_index]->get_name().c_str());

                                    break;
                                }
                            }

                        }
                        break;
                    }

                    //Reset selected after handling pop-up
                    selected = -1;  
                }

                //Do the same for removing components
                static size_t selected_to_remove = 0;
                static std::vector<const char*> present_components;

                present_components.clear();
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

                if (selected_to_remove != -1) {  //Check if a valid selection was made

                    switch (selected_to_remove) {
                    case 0:
                        break;
                    default:
                        if (selected_to_remove >= 0 && selected_to_remove < present_components.size()) {  // Bounds check

                            const auto& component_name = present_components[selected_to_remove];
                            for (const auto& [name, id, add_func, remove_func] : component_checks) {
                                if (std::string(component_name) == name) {

                                        //Execute its remove function
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

    //Function that handles remove button
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

                        //Remove it from the filesystem
                        std::filesystem::remove(temp);

                        auto& texture_storage = ASM.get_texture_storage();                        

                        //Prepare the texture name for removal
                        temp.erase(0, ASM.get_full_path("Textures", "").length());
                        temp = temp.substr(0, temp.find_last_of('.'));

                        auto it = texture_storage.find(temp);
                        if (it == texture_storage.end()) {
                            std::transform(temp.begin(), temp.end(), temp.begin(), to_lower);
                        }

                        //Call the Assets_Manager's delete_texture function to delete the texture and remove associated components
                        ASM.delete_texture(temp);

                        is_file_selected = false;

                    } //Deleting Fonts from Associated Entities
                    else if ((start_pos_of_folder_filepath = temp.find(ASM.get_full_path("Fonts", ""))) != std::string::npos) {

                        //Remove it from filesystem
                        std::filesystem::remove(temp);

                        //Prepare the font name for removal
                        temp.erase(0, ASM.get_full_path("Fonts", "").length());
                        temp = temp.substr(0, temp.find_last_of('.'));

                        auto& font_storage = ASM.get_font_storage();

                        auto font = font_storage.begin();
                        while (font != font_storage.end()) {
                            //std::cout << font->first << std::endl;
                            if (font->first == temp) {

                                //Erase from font storage
                                font = font_storage.erase(font);
                            }
                            else {
                                ++font;
                            }
                        }
                        is_file_selected = false;

                    } //Deleting Audios from Associated Entities
                    else if ((start_pos_of_folder_filepath = temp.find(ASM.get_full_path("Audio", ""))) != std::string::npos)
                    {
                        //Get the file path
                        std::string filepath = temp;

                        //Get the filename
                        temp.erase(0, ASM.get_full_path("Audio", "").length());
                        temp = temp.substr(0, temp.find_last_of('.'));
                        std::string filename = temp;

                        //Try to find and remove audio
                        ASM.find_and_remove_audio(filename);

                        //Close audio file if it exists
                        if (std::filesystem::exists(filepath))
                        {
                            std::ifstream file(filepath);
                            if (file.is_open())
                            {
                                file.close();
                            }
                        }

                        //Removing from filesystem
                        if (std::filesystem::remove(filepath))
                        {
                            ;
                        }
                        else {
                            ;
                        }

                    }
                    else {

                        //Remove other file types
                        std::filesystem::remove(temp);
                    }

                } //Catching execptions
                catch (std::filesystem::filesystem_error& e) {
                    const char* error_msg = e.what();
                    if (error_msg) {

                        //std::cout << "Error in Deletion: " << error_msg << std::endl;
                    }
                    else {
                        ImGui::Text("Error in Deletion");
                    }
                }
            }
            ImGui::PopStyleColor();
        }
    }

    static bool show_msg = true;

    //Function to handle asset browser
    void IMGUI_Manager::asset_browser() {

        ImGui::Begin("Asset Browser");

        static std::string current_directory = "";

        //Get asset folder directory
        const std::string ASSETS = "";
        std::string assets_path = ASM.get_full_path(ASSETS, "");

        ImGui::Columns(7, 0, false);

        //If asset folder cannot be found
        if (assets_path.empty() || !std::filesystem::exists(assets_path)) {
            ImGui::Text("Error: Invalid assets path.");
        }
        else { //Otherwise if it can be found

            //If the current directory is empty
            if (current_directory.empty()) {

                ImGui::Separator(); //Adds a separator between the columns and other widgets

                try {

                    //Accessing each file in assets directory
                    for (const auto& entry : std::filesystem::directory_iterator(assets_path)) {

                        //If directory is selected
                        if (entry.is_directory()) {

                            //Make button options for folders
                            if (ImGui::Button(entry.path().filename().string().c_str(), { 128, 128 })) {
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
            else { //If current directory isn't empty, it's in the folder's directory

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
                        }

                        //Accessing each file in the current directory
                        for (const auto& folder_entry : std::filesystem::directory_iterator(current_directory)) {

                            if (folder_entry.is_regular_file()) {

                                //Make button options for files
                                if (ImGui::Button(folder_entry.path().filename().string().c_str(), { 128, 128 })) {
                                    selected_filepath = folder_entry.path().string();
                                    is_file_selected = true;
                                }

                                //Selection logic for removal
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

                                //Calls functions to set up the drag drop source for dragging assets via filenames for specified types
                                set_drag_drop_source(current_directory, "Textures", folder_entry.path().string(), "TEXTURE_ITEM");
                                set_drag_drop_source(current_directory, "Audio", folder_entry.path().string(), "AUDIO_ITEM");
                                set_drag_drop_source(current_directory, "Scenes", folder_entry.path().string(), "SCENES_ITEM");

                                //Fonts do not call function and Manually done to account for filepath being dragged, not filename
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
                
                //Generate random position
                static std::default_random_engine generator;
                static std::uniform_real_distribution<float> distribution(-2500.0f, 2500.0f);

                float random_x = distribution(generator);
                float random_y = distribution(generator);

                //Get the Transform2D component and set its position
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
    void IMGUI_Manager::text_input(std::string& data_name, std::string& condition_name) {

        char Buffer[128];
        //strncpy_s is safer
        strncpy_s(Buffer, data_name.c_str(), sizeof(Buffer));
        Buffer[sizeof(Buffer) - 1] = '\0';

        if (ImGui::InputText(condition_name.c_str(), Buffer, sizeof(Buffer))) {

            //replaces the data with the input
            data_name = std::string(Buffer);
        }
    }

    //Function to fill prefab names
    void IMGUI_Manager::fill_prefab_names(const char* prefab_name) {
        prefab_names.push_back(prefab_name);
    }

    //Function to set the scene loaded
    void IMGUI_Manager::set_current_file_shown(std::string current_file) {
        current_file_shown = current_file;
    }

    //Function to set the drag drop source and payload (filename)
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

    //Function to get filename from filepath
    std::string IMGUI_Manager::get_filename_from_filepath(std::string file_path, std::string folder_name) {
        file_path.erase(0, ASM.get_full_path(folder_name, "").length());
        std::string file_name = file_path.substr(0, file_path.find_last_of('.'));
        return file_name;
    }

    //Function to get the scene loaded
    std::string IMGUI_Manager::get_current_file_shown() {
        return current_file_shown;
    }

    //Function to show pop-up warnings
    void IMGUI_Manager::asset_browser_pop_up(bool& show_popup, const char* popup_name, const char* message) {
        if (show_popup) {
            ImGui::OpenPopup(popup_name);

            ImVec2 popup_position = ImVec2(750, 750);
            ImGui::SetNextWindowPos(popup_position);

            if (ImGui::BeginPopup(popup_name)) {

                ImVec4 text_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Text, text_color);

                ImGui::NewLine();
                ImGui::Text(message);
                ImGui::NewLine();

                ImGui::PopStyleColor();

                if (ImGui::Button("Close Message")) {
                    show_msg = false;
                }

                ImGui::EndPopup();
            }
        }
    }

#if 1
    //Function to render
    void IMGUI_Manager::render() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
#endif

    //Function to shut down
    void IMGUI_Manager::shut_down() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        LM.write_log("IMGUI_Manager::shut_down(): IMGUI_Manager shut down successfully.");
    }

    std::vector<std::string> assetFiles;

    //Function to handle files dragged and dropped from outside the engine
    void IMGUI_Manager::Handle_Dropped_File(const std::string filePath)
    {
        std::string extension = filePath.substr(filePath.find_last_of(".") + 1); // find the type of file 

        if (extension == "png")
        {
            assetFiles.push_back(filePath);
            //std::cout << "Added Asset: " << filePath << "\n";
        }

    }

    //Function to return buttons_and_batches vector
    std::vector<std::pair<std::string, std::string>>& IMGUI_Manager::return_buttons_and_batches() {
        return batch_and_button;
    }

    //Function to first initialise buttons_and_batches vector
    void IMGUI_Manager::init_buttons_and_batches() {

        batch_and_button = { {"play_button", "Main_Menu_Play_Batch_14"},
        { "credit_button", "Main_Menu_Credits_Batch_14" },
        { "quit_button", "Main_Menu_Quit_Batch_14" },
        { "back_button", "Back_Batch_14" },
        { "restart_button", "Restart_Batch_14" },
        { "main_menu_button", "Main_Menu_Batch_14" } };

    }

    //Function to update contents of buttons_and_batches vector
    void IMGUI_Manager::update_buttons_and_batches() {

        const auto& entities = ECSM.get_entities();
        std::string is_button = "_button";
        for (auto& entity : entities) {
            size_t found = entity.get()->get_name().find(is_button);
            if (found != std::string::npos) {

                 //Updates texture that corresponds to the button in the vector with the base texture found in the Graphics_Component
                if (entity->has_component(ecs.get_component_id<Graphics_Component>())) {
                    Graphics_Component& graphics = ecs.get_component<Graphics_Component>(entity.get()->get_id());
                    std::string base_texture_name = graphics.texture_name;

                    size_t pos = base_texture_name.find_last_of('_');

                    if (pos != std::string::npos) {
                        base_texture_name.erase(pos);

                        for (auto& pair : batch_and_button) {
                            if (pair.first == entity.get()->get_name()) {

                                pair.second = base_texture_name;  
                                break;  
                            }
                        }
                    }
                }

            }
        }


    }


} // namespace lof
