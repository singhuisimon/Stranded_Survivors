
// Include header file
#include "IMGUI_Manager.h"

// Include other managers
#include "Log_Manager.h"
#include "ECS_Manager.h"

#include <iostream>
#include <random>
#include <chrono>
#include <filesystem>
#include <vector>

// Include utility function
#include "../Utility/Constant.h"
#include "../Utility/Path_Helper.h"

namespace lof {

    int selected_file_index = -1;
    int selected_object_index = -1;
    bool load_selected = false;
    bool show_window = false;
    bool remove_game_obj = false;
    bool create_game_obj = false;

    IMGUI_Manager::IMGUI_Manager() : ecs(ECSM) {}

    IMGUI_Manager::IMGUI_Manager(ECS_Manager& ecs_manager) : ecs(ecs_manager) {
        set_type("IMGUI_Manager");
    }

    IMGUI_Manager& IMGUI_Manager::get_instance() {
        static IMGUI_Manager instance;
        return instance;
    }

    //need to
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

    void IMGUI_Manager::display_loading_options(const std::string& directory) {
        //std::vector<std::string> files;
        //try {
        //    for (const auto& file : std::filesystem::directory_iterator(directory)) {
        //        if (file.is_regular_file()) {
        //            files.push_back(file.path().filename().string()); // Store filename
        //        }
        //    }
        //}
        //catch (const std::filesystem::filesystem_error& e) {
        //    std::cout << "Error: " << e.what() << std::endl; // Print error message
        //}
        int current_object_index = 0;

        if (ImGui::Begin("File List")) {

            // Display the ListBox without allowing selection
            //for (const auto& filename : files) {
            //    //selectable for clicking; second param for highlighting
            //    if (ImGui::Selectable(filename.c_str(), selected_file_index == current_object_index)) {
            //        //selected; casuing seceond param state to change
            //        selected_file_index = current_object_index;
            //    }
            //    ++current_object_index;
            //    
            //    // Create a non-selectable item in the ListBox
            //    //ImGui::Text("%s", filename.c_str());
            //}

            selected_file_index = current_object_index;
            if (ImGui::Button("Load Scene")) {
                load_selected = !load_selected;
            }
            ImGui::End(); // End the ImGui window
        }

       
        //Pressing the button
        if (load_selected && (selected_file_index != -1)) {

            //std::cout << Path_Helper::get_save_file_path(files[selected_file_index].c_str()) << std::endl;
            //load new file, entirety of ecs cleared
            //load ecs again and load up new file 
            //ECSM.shut_down();
            //ECSM.start_up();
            //if (SM.load_scene((Path_Helper::get_save_file_path(files[selected_file_index].c_str())).c_str())){
            if (SM.load_scene((Path_Helper::get_scene_path().c_str()))) {
                std::cout << "\n\n\nworks! \n\n\n" << Path_Helper::get_scene_path() << std::endl;
                //std::cout << "\n\n\nworks! \n\n\n" << files[selected_file_index] << std::endl;
                load_selected = !load_selected;
            }
            else {
                std::cout << "\n\n\nfailed to load \n\n\n" << Path_Helper::get_scene_path() << std::endl;
                load_selected = !load_selected;
            }
        }
    }

    

    void IMGUI_Manager::start_frame() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }


    //function with code taken from IMGUI's GITHUB. To test out IMGUI and their demo.cpp; TO BE DELETED LATER
    void IMGUI_Manager::example_demo(bool& show_demo_window, bool& show_another_window, ImVec4& clear_color, ImGuiIO& io) {

        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");
        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
        // Render windows if the corresponding checkbox is checked
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);  // This renders the ImGui demo window
        if (show_another_window) {
            ImGui::Begin("Another Window", &show_another_window);  // Open another window
            ImGui::Text("Hello from another window!");  // Add content to the window
            ImGui::End();  // Close the window
        }
    }

    /*void IMGUI_Manager::show_performance_viewer() {
        ImGui::Begin("Performance Viewer");
        ImGui::End();
    }*/

    void IMGUI_Manager::imgui_game_objects_list() {

        ImGui::Begin("Hierarchy Object List");

        int current_object_index = 0;
        const auto& entities = ecs.get_entities();

        //std::cout << entities.size() << std::endl;

        if (selected_object_index >= entities.size()) {
            selected_object_index = -1;
        }

        for (int i = 0; i < entities.size(); ++i) { //Still getting deleted entity ??
            
            //std::cout << "num: " << i+1 << "  ";
            if (entities[i] != nullptr) {
                std::string obj_name = entities[i]->get_name();
                //std::cout << obj_name << std::endl;
                
                //selectable for clicking; second param for highlighting
                if (ImGui::Selectable(obj_name.c_str(), selected_object_index == current_object_index)) {

                    //selected; casuing seceond param state to change
                    selected_object_index = current_object_index;
                }
                
                
            }

            ++current_object_index;
        }

        // discuss editing name
        if (ImGui::Button("Edit Game Object")) {
            show_window = !show_window;
        }

        if (ImGui::Button("Remove Game Object")) {
            remove_game_obj = !remove_game_obj;
        }

        //add other options
        ImGui::Text("\n\n%s", "Create Game Object From Prefab");
        if (ImGui::Button("Create Game Object From Prefab")) {

            create_game_obj = !create_game_obj;
        }

        ImGui::End();

        if (selected_object_index != -1 && show_window) {
            imgui_game_objects_edit();
        }

        if (selected_object_index != -1 && remove_game_obj) {
            remove_game_objects(selected_object_index);
        }

        if (create_game_obj) {
            add_game_objects();
        }

    }

    static bool is_static_on = false;
    static bool is_moveable_on = true;
    static bool is_grounded_on = false;
    static bool is_jumping_on = false;
    static bool is_audio_on = false;


    void IMGUI_Manager::imgui_game_objects_edit() {

        ImGui::Begin("Edit Object Properties", &show_window);

        const auto& entities = ecs.get_entities();

        std::string Name = entities[selected_object_index]->get_name();
        std::string condition_name_model = "Name of Entity";
    
        char Buffer[128]; //add to constant.h
        strncpy_s(Buffer, Name.c_str(), sizeof(Buffer));//s is safer
        Buffer[sizeof(Buffer) - 1] = '\0';

        if (ImGui::InputText(condition_name_model.c_str(), Buffer, sizeof(Buffer))) {
             std::string name= std::string(Buffer);
             entities[selected_object_index]->set_name(name);
        }
        

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

                auto& gravity = physics.gravity;
                ImGui::InputFloat2("Gravity", &gravity.x);

                auto& damping_factor = physics.damping_factor;
                ImGui::InputFloat("Damping Factor", &damping_factor);

                auto& max_velocity = physics.max_velocity;
                ImGui::InputFloat("Maximun Velocity", &max_velocity);

                auto& accumulated_force = physics.accumulated_force;
                ImGui::InputFloat2("Accumulated Force", &accumulated_force.x);

                auto& mass = physics.mass;
                ImGui::InputFloat("Mass", &mass);

                auto& is_static = physics.is_static;
                std::string s_label = "is_static: " + std::string(is_static_on ? "On" : "Off");
                if (button_toggle(s_label, &is_static_on)) {
                    is_static = !is_static;
                }

                auto& is_grounded = physics.is_grounded;
                std::string g_label = "is_grounded: " + std::string(is_grounded_on ? "On" : "Off");
                if (button_toggle(g_label, &is_grounded_on)) {
                    is_grounded = !is_grounded;
                }

                auto& jump = physics.jump_force;
                ImGui::InputFloat("Jump Force", &jump);
            }
        }

        //Grpahics Component
        if (entities[selected_object_index]->has_component(ecs.get_component_id<Graphics_Component>())) {
            Graphics_Component& graphics = ecs.get_component<Graphics_Component>(entities[selected_object_index].get()->get_id());
            if (ImGui::CollapsingHeader("Graphics")) {

                auto& model_name = graphics.model_name;
                std::string condition_name_model = "model_name";
                text_input(model_name, condition_name_model);

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
            if (ImGui::CollapsingHeader("Animation")) {

                //need to fix! - first still buggy!!!!
                auto& animation_list = animation.animations;
                for (auto it = animation_list.begin(); it != animation_list.end(); ++it) {
                    
                    char Buffer[128];
                    strncpy_s(Buffer, it->second.c_str(), sizeof(Buffer));//s is safer
                    Buffer[sizeof(Buffer) - 1] = '\0';
                    
                    //needs to be consistent
                    if (ImGui::InputText(it->first.c_str(), Buffer, sizeof(Buffer))) {
                        it->second = Buffer;
                    }
          
                }

                //find a way for this not to be less than 1
                auto& current_animation_index = animation.curr_animation_idx;

                /*if (current_animation_index <= 0) {
                    current_animation_index = 0;
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
                }*/
                
                if (ImGui::InputInt("start animation index", reinterpret_cast<int*>(&current_animation_index))) {
                    if (current_animation_index < 0) {
                        current_animation_index = 0;
                    }
                }
                //if (current_animation_index == 0) {
                //    ImGui::PopItemFlag(); // Re-enable the widget
                //}

                //somehow ok w less than 1
                auto& start_animation_index = animation.start_animation_idx;
                ImGui::InputInt("start animation index", reinterpret_cast<int*>(&start_animation_index));

            }
        }

        if (ImGui::Button("Save Changes")) {

            std::string scene_path = Path_Helper::get_scene_path();
            if (SM.save_game_state(scene_path.c_str())) {
                LM.write_log("IMGUI_Manager::update(): Successfully saved game state to %s", Path_Helper::get_scene_path().c_str());
                std::cout << "\n\n\n\nGame saved successfully to: " << Path_Helper::get_scene_path() << "\n\n\n" << std::endl;
            }
            else {
                LM.write_log("IMGUI_Manager::update(): Failed to save game state to %s", Path_Helper::get_scene_path().c_str());
                std::cout << "\n\n\n\nFailed to save game!\n\n\n" << std::endl;
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

    //simon's code
    void IMGUI_Manager::add_game_objects() {

        ImGui::Begin("Edit Object Properties", &create_game_obj);
        //add more options

        static const char* prefab_options[]{ "player", "dummy_object", "gui_container", "gui_progress_bar", "gui_image" };

        static int selected_item = -1;

        if (ImGui::Combo("Clone from Prefab Options", &selected_item, prefab_options, IM_ARRAYSIZE(prefab_options))){
            
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
            

        ImGui::End();
    }

    //removing game object - has error
    void IMGUI_Manager::remove_game_objects(int index) {

        const auto& entities = ecs.get_entities();
        EntityID eid = entities[index]->get_id();

        if (eid != INVALID_ENTITY_ID) {
            ECSM.destroy_entity(eid);

            std::cout << "end of calling remove\n";
            remove_game_obj = !remove_game_obj;
            return;
        }

        
    }

    /*try {
            const auto& entities = ecs.get_entities();

            if (selected_object_index >= entities.size() || !entities[selected_object_index]) {
                std::cout << "\n\n\n\nError: selected_object_index is out of bounds.\n\n\n\n" << std::endl;
                return;
            }

            auto& selectedEntity = entities[selected_object_index];
            if (!selectedEntity) {
                std::cout << "\n\n\n\nError: Entity at selected_object_index is already null.\n\n\n\n" << std::endl;
                return;
            }

            EntityID entityId = entities[selected_object_index]->get_id();
            if (entityId >= entities.size() || !entities[entityId]) {
                std::cout << "\n\n\n\nError: Entity ID is invalid or entity does not exist.\n\n\n\n" << std::endl;
                return;
            }

            ECSM.destroy_entity(entityId);
            std::cout << entities.size() << std::endl;
        }
        catch (const std::exception& e) {
            std::cout << "\n\n\n\nUnexpected error: " << e.what() << "\n\n\n\n" << std::endl;
        }*/

    void IMGUI_Manager::text_input(std::string& data_name, std::string& codition_name) {

        char Buffer[128]; //add to constant.h
        strncpy_s(Buffer, data_name.c_str(), sizeof(Buffer));//s is safer
        Buffer[sizeof(Buffer) - 1] = '\0';

        if (ImGui::InputText(codition_name.c_str(), Buffer, sizeof(Buffer))) {
            data_name = std::string(Buffer);
        }
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
