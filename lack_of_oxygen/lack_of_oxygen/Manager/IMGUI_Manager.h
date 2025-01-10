/**
 * @file IMGUI_Manager.h
 * @brief Declaration of the IMGUI_Manager class for running the IMGUI level editor.
 * @author Liliana Hanawardani (100%)
 * @date November 8, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#ifndef LOF_IMGUI_MANAGER_H
#define LOF_IMGUI_MANAGER_H

 // Macro for accessing manager singleton instances
#define IMGUIM lof::IMGUI_Manager::get_instance()

 // Include header file
#include "Manager.h"

// Include IMGUI headers
#include "../IMGUI/imgui.h"
#include "../IMGUI/imgui_impl_glfw.h"
#include "../IMGUI/imgui_impl_opengl3.h"


// Include standard headers
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>

namespace lof {

    //boolean to notify if an entity has been selected
    extern bool select_entity;

    /**
     * @class IMGUI_Manager
     * @brief Manages the starting up, shutting down, the displaying and editing
     * information, and the internal logic in level editor.
     *
     * This class follows the Singleton pattern to ensure a single instance throughout
     * the application's lifecycle.
     */
    class IMGUI_Manager : public Manager {
    private:

        /**
         * @brief Private constructor for singleton pattern
         */
        IMGUI_Manager();

        //Reference to the existing ECS Manager to accses entities
        class ECS_Manager& ecs;

        //Vector to get and hold the prefab names for display
        std::vector<std::string> prefab_names{};

        //Vector to hold audio file names for display
        std::vector<std::pair<std::string, std::string>> audio_file_names{};
        
        //Vector to hold audio types for display
        std::vector<std::pair<std::string, AudioType>> audio_types{};

        //Mouse position in the game world
        ImVec2 mouse_pos_game;

        //String holding the name of the current file loaded
        std::string current_file_shown;

    public:

        /**
         * @brief Single argument constructor that takes in and initalises a reference to the existing ECS Manager.
         * @param ecs_manager Reference to the existing ECS Manager to initalise the private member ecs with.
         */
        IMGUI_Manager(ECS_Manager& ecs_manager);

        //Deleted copy constructor and copy assignmemt to enforce singleton pattern
        IMGUI_Manager(const IMGUI_Manager&) = delete;
        IMGUI_Manager& operator=(const IMGUI_Manager&) = delete;

        /**
         * @brief Get singleton instance of IMGUI_Manager.
         * @return Reference to IMGUI_Manager instance.
         */
        static IMGUI_Manager& get_instance();

        /**
         * @brief start_up function inherited from Manager. Not using and will throw a runtime error when used.
         * @return Will not return anything due to error being thrown when used.
         */
        int start_up() override;

        /**
         * @brief Redefined start_up function (not inherited from Manager). Calls upon IMGUI's functions for initalization.
         * @param window A reference to a pointer to the GLFWwindow window created with the GLFW library in main.
         * @return Will return 0 after sucessful initalistion.
         */
        int start_up(GLFWwindow*& glfwindow);

        /**
         * @brief Calls functions from IMGUI that set up a new frame for rendering with ImGui in an OpenGL and GLFW context
         */
        void start_frame();

        void render_ui(unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT);

        /**
         * @brief Displays the file list in the menu. Handles the logic for calling the load scene function for the "Load File" option.
         */
        void display_loading_options();

        /**
         * @brief Displays the Object Hierarchy List and its selectable options. Handles the logic of calling of respective functions when the buttons are pressed after selecting.
         */
        void imgui_game_objects_list();

        /**
         * @brief Displays the window for the Property Editor. Handles the displaying and the logic of editing of entities' component's values.
         */
        void imgui_game_objects_edit();

        /**
         * @brief Displays the window with a dropdown displaying what type of game objects can be created and calls the function to create the game object based on the chosen type.
         */
        void add_game_objects();

        /**
         * @brief Gets the entity's ID and calls upon destroy_entity function to remove it.
         * @param index The position of the game object in the the entities vector to be removed.
         */
        void remove_game_objects(int index);

        /**
         * @brief Function to create and display a button to toggle between boolean values.
         * @param boolean_name Reference to a string containing the name of the boolean to be displayed on the button.
         * @param state Pointer to the current state of the boolean.
         * @return State of boolean
         */
        bool button_toggle(const std::string& boolean_name, bool* state);

        /**
         * @brief Function to create and display a labelled text box that shows the current text and is also able to be typed into to replace it.
         * @param data_name Reference to a string containing the current text data of the the object's condition.
         * @param codition_name Reference to a string containing the name of the condition.
         */
        void text_input(std::string& data_name, std::string& codition_name);

        /**
         * @brief Pushes back the prefab_name string to the prefab_names vector.
         * @param prefab_name Const char* string containing the prefab name
         */
        void fill_prefab_names(const char* prefab_name);

        /**
         * @brief Pushes back the prefab_name string to the prefab_names vector.
         * @param prefab_name Const char* string containing the prefab name
         */
        void fill_audio_file_names(std::string audio_file_name, std::string audio_filepath_name);

        /**
         * @brief Calls functions from IMGUI that renders the level editor in an OpenGL and GLFW context
         */
        void render();

        /**
         * @brief Shuts down all IMGUI_Manager services. Called after the game loop to ensure proper cleanup.
         */
        void shut_down() override;

        /**
         * @brief Calculates the mouse position in terms of the game world when in the IMGUI viewport
         * @param texture_pos position of the texture in the screen
         * @param mouse_pos position of the mouse in the screen
         * @param SCR_WIDTH Width of the screen
         * @param SCR_HEIGHT Height of the screen
         * @return Mouse position in terms of the game world
         */
        ImVec2 get_imgui_mouse_pos(ImVec2 texture_pos, ImVec2 mouse_pos, unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT);

        /**
         * @brief Getter for the mouse position in terms of the game world when in the IMGUI viewport
         * @return Mouse position in terms of the game world
         */
        ImVec2 imgui_mouse_pos();

        /**
         * @brief Setter of current file to note down the file that is loaded.
         * @param current_file The name of the current file.
         */
        void set_current_file_shown(std::string current_file);

        /**
         * @brief Getter for the current file.
         * @return The name of the current file.
         */
        std::string get_current_file_shown();

        /**
         * @brief Function to fill up the vector of sound names.
         */
        void fill_up_sound_names();

        /**
         * @brief Function to turn off in-game GUI.
         */
        void disable_GUI();
    };

} // namespace lof

#endif // LOF_IMGUI_MANAGER_H
