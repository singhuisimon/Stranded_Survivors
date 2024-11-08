// /**
//  * @file Path_Helper.cpp
//  * @brief Implements the Path_Helper utility class methods for handling dynamic file paths.
//  * @author Simon Chan (100%)
//  * @date October 3, 2024
//  * Copyright (C) 2024 DigiPen Institute of Technology.
//  * Reproduction or disclosure of this file or its contents without the
//  * prior written consent of DigiPen Institute of Technology is prohibited.
//  */
// #include "Path_Helper.h"

// #include <windows.h>
// #include <sstream>
// #include <vector>

//  // Helper function to split strings
// std::vector<std::string> split(const std::string& str, char delimiter) {
//     std::vector<std::string> tokens;
//     std::string token;
//     std::istringstream tokenStream(str);
//     while (std::getline(tokenStream, token, delimiter)) {
//         tokens.push_back(token);
//     }
//     return tokens;
// }

// // Helper function to join paths
// std::string join_path(const std::vector<std::string>& parts) {
//     std::string result;
//     for (const auto& part : parts) {
//         if (!result.empty() && result.back() != '\\') {
//             result += "\\";
//         }
//         result += part;
//     }
//     return result;
// }

// namespace lof {

//     // Function to get the directory where the executable is located
//     std::string Path_Helper::get_executable_directory() {
//         char buffer[MAX_PATH];
//         GetModuleFileNameA(NULL, buffer, MAX_PATH);
//         std::string::size_type pos = std::string(buffer).find_last_of("\\/");
//         return std::string(buffer).substr(0, pos);
//     }

//     // Function to resolve a relative path based on the current directory
//     std::string Path_Helper::resolve_relative_path(const std::string& relative_path) {
//         std::string base_path = get_executable_directory();
//         std::vector<std::string> base_parts = split(base_path, '\\');
//         std::vector<std::string> relative_parts = split(relative_path, '\\');

//         for (const auto& part : relative_parts) {
//             if (part == "..") {
//                 if (!base_parts.empty()) {
//                     base_parts.pop_back();
//                 }
//             }
//             else {
//                 base_parts.push_back(part);
//             }
//         }

//         return join_path(base_parts);
//     }

//     std::string Path_Helper::get_config_path() {
//         return get_executable_directory() + "\\..\\..\\lack_of_oxygen\\Data\\config.json";
//     }

//     std::string Path_Helper::get_prefabs_path() {
//         return get_executable_directory() + "\\..\\..\\lack_of_oxygen\\Data\\prefab.json";
//     }

//     std::string Path_Helper::get_scene_path() {
//         return get_executable_directory() + "\\..\\..\\lack_of_oxygen\\Data\\scene1.scn";
//     }

//     // Function to get the path to music file 1
//     std::string Path_Helper::get_music_path1() {
//         return get_executable_directory() + "\\..\\..\\lack_of_oxygen\\Data\\audio_test1.wav";
//     }

//     // Function to get the path to music file 2
//     std::string Path_Helper::get_music_path2() {
//         return get_executable_directory() + "\\..\\..\\lack_of_oxygen\\Data\\audio_test2.wav";
//     }

//     // Function to get the path to the vertex shader for objects
//     std::string Path_Helper::get_vertex_shader_obj_path() {
//         return get_executable_directory() + "\\..\\..\\lack_of_oxygen\\Shaders\\lack_of_oxygen_obj.vert";
//     }

//     // Function to get the path to the fragment shader for objects
//     std::string Path_Helper::get_fragment_shader_obj_path() {
//         return get_executable_directory() + "\\..\\..\\lack_of_oxygen\\Shaders\\lack_of_oxygen_obj.frag";
//     }

//     // Function to get the path to the vertex shader for debugging features
//     std::string Path_Helper::get_vertex_shader_debug_path() {
//         return get_executable_directory() + "\\..\\..\\lack_of_oxygen\\Shaders\\lack_of_oxygen_debug.vert";
//     }

//     // Function to get the path to the fragment shader debugging features
//     std::string Path_Helper::get_fragment_shader_debug_path() {
//         return get_executable_directory() + "\\..\\..\\lack_of_oxygen\\Shaders\\lack_of_oxygen_debug.frag";
//     }

//     // Function to get the path to the vertex shader for fonts
//     std::string Path_Helper::get_vertex_shader_font_path() {
//         return get_executable_directory() + "\\..\\..\\lack_of_oxygen\\Shaders\\lack_of_oxygen_font.vert";
//     }

//     // Function to get the path to the fragment shader for fonts
//     std::string Path_Helper::get_fragment_shader_font_path() {
//         return get_executable_directory() + "\\..\\..\\lack_of_oxygen\\Shaders\\lack_of_oxygen_font.frag";
//     }

//     // Function to get the path to the model file
//     std::string Path_Helper::get_model_file_path() {
//         return get_executable_directory() + "\\..\\..\\lack_of_oxygen\\Data\\models.msh";
//     }

//     // Function to get the path to the texture file
//     std::string Path_Helper::get_texture_file_path() {
//         return get_executable_directory() + "\\..\\..\\lack_of_oxygen\\Data\\Textures\\Texture_Names.txt";
//     }

//     // Function to get the path to the save file
//     std::string Path_Helper::get_save_file_path(const std::string& filename) {
//         return get_executable_directory() + "\\..\\..\\lack_of_oxygen\\Data\\" + filename;
//     }

//     // Function to get the path to the animation file
//     std::string Path_Helper::get_animation_file_path() {
//         return get_executable_directory() + "\\..\\..\\lack_of_oxygen\\Data\\Textures\\Prisoner_Atlas.txt";
//     } 

//     // Function to get the path to the font file
//     std::string Path_Helper::get_font_file_path() {
//         return get_executable_directory() + "\\..\\..\\lack_of_oxygen\\lack_of_oxygen\\Data\\Fonts\\Fonts.txt";
//     }

// } // namespace lof
