
#ifndef LOF_IMGUI_MANAGER_H
#define LOF_IMGUI_MANAGER_H

// Macros for accessing manager singleton instances
#define IMGUIM lof::IMGUI_Manager::get_instance()

 // Include header file
#include "Manager.h"

// Include other necessary headers
// Include IMGUI headers
#include "../IMGUI/imgui.h"
#include "../IMGUI/imgui_impl_glfw.h"
#include "../IMGUI/imgui_impl_opengl3.h"
// Include RapidJSON headers
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include <rapidjson/istreamwrapper.h>

// Include standard headers
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>

namespace lof {

    rapidjson::Document load_scn(const std::string& path);
    class IMGUI_Manager : public Manager {
    private:
        IMGUI_Manager();
        class ECS_Manager& ecs;
    public:

        IMGUI_Manager(ECS_Manager& ecs_manager);
        IMGUI_Manager(const IMGUI_Manager&) = delete;
        IMGUI_Manager& operator=(const IMGUI_Manager&) = delete;
        static IMGUI_Manager& get_instance();

        //note: inherited start_up function must be dealt with

        int start_up(GLFWwindow*& window);
        void start_frame();
        void example_demo(bool& show_demo_window, bool& show_another_window, ImVec4& clear_color, ImGuiIO& io);
        void display_loading_options(const std::string& directory);
        void imgui_game_objects_list();
        void imgui_game_objects_edit();
        void add_game_objects();
        void remove_game_objects();
        void clone_game_objects();
        bool button_toggle(const std::string& condition_name, bool* state);
        void text_input(std::string& data_name, std::string& codition_name);
        void render();
        void shut_down() override;
        //remember to delete update and start up
    };

} // namespace lof

#endif // LOF_IMGUI_MANAGER_H
