#pragma once
/**
 * @file Model.h
 * @brief Defines the Model class for storing and managing model data.
 * @date September 21, 2024
 */

#ifndef LOF_MODEL_H
#define LOF_MODEL_H

// Include standard headers
#include <vector>
#include <string>

// Include Utility headers
#include "../Utility/Vector2D.h" // Adjust the path based on your project structure

namespace lof {

    /**
     * @class Model
     * @brief Represents a 2D model with vertices and triangles.
     */
    class Model {
    public:
        /**
         * @brief Default constructor for Model.
         */
        Model();

        /**
         * @brief Loads model data from a .msh file.
         * @param filepath Path to the .msh file.
         * @return True if loading is successful, false otherwise.
         */
        bool load_from_file(const std::string& filepath);

        /**
         * @brief Gets the vertices of the model.
         * @return Reference to the vector of vertices.
         */
        const std::vector<Vec2D>& get_vertices() const;

        /**
         * @brief Gets the triangle indices of the model.
         * @return Reference to the vector of triangle indices.
         */
        const std::vector<std::vector<int>>& get_triangles() const;

    private:
        std::vector<Vec2D> vertices;                   ///< List of vertices.
        std::vector<std::vector<int>> triangles;        ///< List of triangles (indices into vertices).
    };

} // namespace lof

#endif // LOF_MODEL_H
