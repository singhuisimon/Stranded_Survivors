#pragma once
/**
 * @file PathHelper.h
 * @brief Declares the PathHelper utility class functions for handling dynamic file paths.
 * @author Simon Chan (100%)
 * @date October 3, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#pragma once
#ifndef PATH_HELPER_H
#define PATH_HELPER_H

#include <string>

namespace lof {

    /**
     * @class PathHelper
     * @brief Utility class for generating file paths relative to the executable directory.
     */
    class Path_Helper {
    public:
        /**
         * @brief Get the directory of the currently running executable.
         * @return The executable's directory as a string.
         */
        static std::string get_executable_directory();

        /**
         * @brief Resolve a relative path based on the current directory.
         * @param relative_path The relative path to resolve.
         * @return The full resolved path as a string.
         */
        static std::string resolve_relative_path(const std::string& relative_path);

        // Getter functions for common project paths
        static std::string get_config_path();
        static std::string get_prefabs_path();
        static std::string get_scene_path();
        static std::string get_music_path1();
        static std::string get_music_path2();
        static std::string get_vertex_shader_path_1();
        static std::string get_fragment_shader_path_1();
        static std::string get_vertex_shader_path_2();
        static std::string get_fragment_shader_path_2();
        static std::string get_model_file_path();
        static std::string get_texture_file_path();
        static std::string get_save_file_path(const std::string& filename);
        static std::string get_animation_file_path();
    };

} // namespace lof

#endif // PATH_HELPER_H
