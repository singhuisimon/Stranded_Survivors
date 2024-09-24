/**
 * @file Model.cpp
 * @brief Implements the Model class.
 * @date September 21, 2024
 */

// Include header file
#include "Model.h"

// Include Log_Manager for logging
#include "../manager/Log_Manager.h" // Ensure Log_Manager is correctly referenced

// Include standard headers
#include <fstream>
#include <sstream>

namespace lof {

    Model::Model() {
        // Initialize with empty vectors
        vertices.clear();
        triangles.clear();
    }

    bool Model::load_from_file(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            LM.write_log("Model::load_from_file(): Failed to open file '%s'.", filepath.c_str());
            return false;
        }

        std::string line;
        size_t line_number = 0;
        while (std::getline(file, line)) {
            line_number++;
            // Skip empty lines
            if (line.empty()) continue;

            std::istringstream iss(line);
            char prefix;
            iss >> prefix;

            if (prefix == 'v') { // Vertex
                float x, y;
                if (!(iss >> x >> y)) {
                    LM.write_log("Model::load_from_file(): Invalid vertex format at line %zu in '%s'.", line_number, filepath.c_str());
                    continue; // Skip invalid vertex
                }
                vertices.emplace_back(x, y);
            }
            else if (prefix == 't') { // Triangle
                int idx1, idx2, idx3;
                if (!(iss >> idx1 >> idx2 >> idx3)) {
                    LM.write_log("Model::load_from_file(): Invalid triangle format at line %zu in '%s'.", line_number, filepath.c_str());
                    continue; // Skip invalid triangle
                }
                // Validate indices
                if (idx1 < 0 || idx1 >= static_cast<int>(vertices.size()) ||
                    idx2 < 0 || idx2 >= static_cast<int>(vertices.size()) ||
                    idx3 < 0 || idx3 >= static_cast<int>(vertices.size())) {
                    LM.write_log("Model::load_from_file(): Triangle indices out of range at line %zu in '%s'.", line_number, filepath.c_str());
                    continue; // Skip invalid triangle
                }
                triangles.emplace_back(std::vector<int>{idx1, idx2, idx3});
            }
            else {
                // Unknown prefix, can log or ignore
                LM.write_log("Model::load_from_file(): Unknown prefix '%c' at line %zu in '%s'. Skipping.", prefix, line_number, filepath.c_str());
                continue; // Skip unknown lines
            }
        }

        LM.write_log("Model::load_from_file(): Loaded model '%s' with %zu vertices and %zu triangles.", filepath.c_str(), vertices.size(), triangles.size());
        return true;
    }

    const std::vector<Vec2D>& Model::get_vertices() const {
        return vertices;
    }

    const std::vector<std::vector<int>>& Model::get_triangles() const {
        return triangles;
    }

} // namespace lof
