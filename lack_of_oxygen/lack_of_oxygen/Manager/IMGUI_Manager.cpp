
 // Include header file
#include "IMGUI_Manager.h"

// Include other managers
#include "Log_Manager.h"
#include "ECS_Manager.h"

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

    //currently has bug, needs to save in json file!!!
    void IMGUI_Manager::imgui_game_objects_edit() {
        
        ImGui::Begin("Edit Object Properties", &show_window);

        const auto& entities = ecs.get_entities();

        if (entities[selected_object_index]->has_component(ecs.get_component_id<Transform2D>())) {
            Transform2D& transform = ecs.get_component<Transform2D>(entities[selected_object_index].get()->get_id());
            if (ImGui::CollapsingHeader("Transformation")) {

                auto& position = transform.position;
                ImGui::InputFloat2("Position", &position.x);

                auto& orientation = transform.orientation;
                ImGui::InputFloat2("Orientation", &orientation.x);

                auto& scale = transform.scale;
                ImGui::InputFloat2("Scale", &scale.x);
            }

            if (ImGui::Button("Save Changes")) {
                //save_transform_to_json(entities[selected_object_index].get()->get_id(), transform);
            }
        }
       
        if (entities[selected_object_index]->has_component(ecs.get_component_id<Velocity_Component>())) {
            Velocity_Component& velocity_comp = ecs.get_component<Velocity_Component>(entities[selected_object_index].get()->get_id());
            if (ImGui::CollapsingHeader("Velocity")) {

                auto& velocity = velocity_comp.velocity;
                ImGui::InputFloat2("Velocity", &velocity.x);

            }
        }


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

                //booleans make into buttons???

                auto& jump = physics.jump_force;
                ImGui::InputFloat("Jump Force", &jump);
            }
        }

        if (entities[selected_object_index]->has_component(ecs.get_component_id<Graphics_Component>())) {
            Graphics_Component& graphics = ecs.get_component<Graphics_Component>(entities[selected_object_index].get()->get_id());
            if (ImGui::CollapsingHeader("Graphics")) {

                //strings. asset and model manager???? shd_ref, mdl_to_ndc_xform
              
                auto& color = graphics.color;
                ImGui::InputFloat3("Color", &color.x);
            }
        }

        if (entities[selected_object_index]->has_component(ecs.get_component_id<Collision_Component>())) {
            Collision_Component& collision = ecs.get_component<Collision_Component>(entities[selected_object_index].get()->get_id());
            if (ImGui::CollapsingHeader("Collision")) {

                // error w collision handling
                auto& width = collision.width;
                ImGui::InputFloat("Width", &width);

                auto& height = collision.height;
                ImGui::InputFloat("Width", &height);
            }
        }
            
        /*void IMGUI_Manager::imgui_game_objects_edit(rapidjson::Document & scene1_data) {
        ImGui::Begin("Edit Object Properties", &show_window);

        if (scene1_data.HasMember("objects") && scene1_data["objects"].IsArray()) {
            const auto& objects = scene1_data["objects"].GetArray();
            auto& selected_object = objects[selected_object_index];

            if (selected_object.HasMember("name") && selected_object["name"].IsString()) {
                std::string obj_name = selected_object["name"].GetString();
                ImGui::Text("Object Name: %s", obj_name.c_str());
            }

            ImGui::Text("\nList of components:");
            if (selected_object.HasMember("components")) {
                auto& component = selected_object["components"];

                if (component.HasMember("Transform2D")) {

                    if (ImGui::CollapsingHeader("Transformation")) {
                        if (component["Transform2D"].HasMember("position") && component["Transform2D"]["position"].IsArray()) {
                            auto& transform = component["Transform2D"];
                            auto& position = transform["position"];

                            float x = position[0].GetFloat();
                            float y = position[1].GetFloat();

                            if (ImGui::InputFloat2("Position", &position)) {
                                position[0].SetFloat(x);
                                position[1].SetFloat(y);
                            }
                        }

                        if (component["Transform2D"].HasMember("scale") && component["Transform2D"]["scale"].IsArray()) {
                            auto& transform = component["Transform2D"];
                            auto& scale = transform["scale"];

                            float x = scale[0].GetFloat();
                            float y = scale[1].GetFloat();

                            if (ImGui::InputFloat2("Scale", &x)) {
                                scale[0].SetFloat(x);
                                scale[1].SetFloat(y);
                            }
                        }

                        if (component["Transform2D"].HasMember("orientation") && component["Transform2D"]["orientation"].IsArray()) {
                            auto& transform = component["Transform2D"];
                            auto& orientation = transform["orientation"];

                            float x = orientation[0].GetFloat();
                            float y = orientation[1].GetFloat();

                            if (ImGui::InputFloat2("Orientation", &x)) {
                                orientation[0].SetFloat(x);
                                orientation[1].SetFloat(y);
                            }
                        }
                    }
                }
                
                if (component.HasMember("Collision_Component")) {
                    if (ImGui::CollapsingHeader("Collision")) {
                        if (component["Collision_Component"].HasMember("width") && component["Collision_Component"]["width"].IsFloat()) {
                            auto& transform = component["Collision_Component"];
                            auto& width = transform["width"];

                            float x = width.GetFloat();

                            if (ImGui::InputFloat("Collision Width", &x)) {
                                width.SetFloat(x);
                            }
                        }

                        if (component["Collision_Component"].HasMember("height") && component["Collision_Component"]["height"].IsFloat()) {
                            auto& transform = component["Collision_Component"];
                            auto& height = transform["height"];

                            float x = height.GetFloat();

                            if (ImGui::InputFloat("Collision Height", &x)) {
                                height.SetFloat(x);
                            }
                        }
                    }

                }

            }
        }

        */


        ImGui::End();
    }


    /*void IMGUI_Manager::save_transform_to_json(EntityID entity, const Transform2D& transformed_data) {
        ;
    }*/

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
