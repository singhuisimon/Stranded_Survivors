/**
 * @file lack_of_oxygen_2.vert
 * @brief This file implements the vertex shader for debug shapes that read 
 *		  per-vertex data into vertex attributes from vertex attributes indices.
 * @author Chua Wen Bin Kenny (100%)
 * @date September 20, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
*/
// OpenGL Version
#version 450 core

// In
layout(location = 0) in vec2 aVertexPosition;

// Uniforms
uniform mat3 uModel_to_NDC_Mat;

void main() {
	gl_Position = vec4(vec2(uModel_to_NDC_Mat * vec3(aVertexPosition, 1.0f)), 0.0, 1.0);
}