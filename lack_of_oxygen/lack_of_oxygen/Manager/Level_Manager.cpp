
// Include header file
#include "Log_Manager.h"
#include "ECS_Manager.h"
#include "Level_Manager.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>

namespace lof {

    rapidjson::Document load_scn_for_lvlm(const std::string& path) {

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

        file_str.close();
        return scn_document;
    }

    Level_Manager::Level_Manager() : ecs(ECSM) {}

    Level_Manager::Level_Manager(ECS_Manager& ecs_manager) : ecs(ecs_manager) {
        set_type("Level_Manager");
    }

    Level_Manager& Level_Manager::get_instance() {
        static Level_Manager instance;
        return instance;
    }

    int Level_Manager::start_up() {
        if (is_started()) {
            LM.write_log("Level_Manager::start_up(): Already started.");
            return 0; // Already started
        }
        return 0;
        LM.write_log("Level_Manager::start_up(): Level_Manager started successfully.");
    }

    void Level_Manager::shut_down() {
        LM.write_log("Level_Manager::shut_down(): Level_Manager shut down successfully.");
    }

    //main game loop goes here!!!
    void Level_Manager::update() {

        //switches between level files
        //function that opens specific level's file and serializes its data
        //game loop
    }
} // namespace lof
