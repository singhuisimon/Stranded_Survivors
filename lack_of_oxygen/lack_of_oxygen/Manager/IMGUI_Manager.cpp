
 // Include header file
#include "IMGUI_Manager.h"

// Include other managers
#include "Log_Manager.h"
#include "ECS_Manager.h"
#include "Level_Manager.h"

// Include utility function
#include "../Utility/Constant.h"
#include "../Utility/Path_Helper.h"

namespace lof {

    int selected_object_index = -1;
    bool show_window = false;

    //NOT SUPPOSED TO LOAD!!! -> CONSTANTLY LOADING IN UPDATE
    rapidjson::Document load_scn(const std::string& path) {

        std::ifstream file_str(path);
        if (!file_str.is_open()) {
            throw std::runtime_error("Error in opening scn file");
        }

        std::string scn_content{};
        std::string line{};

        while (std::getline(file_str, line)) {
            scn_content += line;
        }

        rapidjson::Document scn_document;
        scn_document.Parse(scn_content.c_str());
        return scn_document;
    }

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
    
    void IMGUI_Manager::imgui_game_objects_list() {

        //loop entities here!!!
        //make a function gets the names via json and just call here

        // please get name of entity
        //might neeed to get function that gets entity's name vfrom json file


        ImGui::Begin("Hierarchy Object List");

        //remove json readings - need to write anotehr function to get the name
        const std::string scene1_scn = Path_Helper::get_scene_path();
        const std::string prefabs_json = Path_Helper::get_prefabs_path();

        rapidjson::Document scene1_data = load_scn(scene1_scn);
        if (scene1_data.HasMember("objects") && scene1_data["objects"].IsArray()) {
            const auto& objects = scene1_data["objects"].GetArray();

            int current_object_index = 0;
            for (const auto& object : objects) {
                if (object.HasMember("name") && object["name"].IsString()) {
                    std::string obj_name = object["name"].GetString();

                    //selectable for clicking; second param for highlighting
                    if (ImGui::Selectable(obj_name.c_str(), selected_object_index == current_object_index)) {

                        //selected; casuing seceond param state to change
                        selected_object_index = current_object_index;
                    }

                }

                ++current_object_index;
            }
        }
        

        if (ImGui::Button("Edit Selected")) {
            show_window = !show_window;
        }

        ImGui::End();

        if (selected_object_index != -1 && show_window) {
            
            imgui_game_objects_edit();
        }
        
    }

    static bool is_static_on = false;
    static bool is_moveable_on = true;
    static bool is_grounded_on = false;
    static bool is_jumping_on = false;

    void IMGUI_Manager::imgui_game_objects_edit() {
        
        ImGui::Begin("Edit Object Properties", &show_window);

        const auto& entities = ecs.get_entities();

        //Transform2D Component
        if (entities[selected_object_index]->has_component(ecs.get_component_id<Transform2D>())) {
            Transform2D& transform = ecs.get_component<Transform2D>(entities[selected_object_index].get()->get_id());
            if (ImGui::CollapsingHeader("Transformation")) {

                //modifying values
                auto& position = transform.position;
                ImGui::InputFloat2("Position", &position.x);

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

                //buggy!!!
                auto& shd_ref = graphics.shd_ref;
                ImGui::InputInt("shd_ref", reinterpret_cast<int*>(&shd_ref));
                
            }
        }

        //Collision Component
        if (entities[selected_object_index]->has_component(ecs.get_component_id<Collision_Component>())) {
            Collision_Component& collision = ecs.get_component<Collision_Component>(entities[selected_object_index].get()->get_id());
            if (ImGui::CollapsingHeader("Collision")) {

                auto& width = collision.width;
                ImGui::InputFloat("Width", &width);

                auto& height = collision.height;
                ImGui::InputFloat("Width", &height);
            }
        }

        if (ImGui::Button("Save Changes")) {

            //button to save scene goes here
 
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
