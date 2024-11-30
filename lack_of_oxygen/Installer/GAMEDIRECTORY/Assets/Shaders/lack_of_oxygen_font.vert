/**
 * @file lack_of_oxygen_font.vert
 * @brief This file implements the vertex shader for fonts that read 
 *		  per-vertex data into vertex attributes from vertex attributes indices.
 * @author Chua Wen Bin Kenny (100%)
 * @date November 5, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
*/
// OpenGL Version
#version 450 core

// In
layout(location = 0) in vec4 aVertex;

// Out
layout(location = 0) out vec2 vTextCoord;

// Uniforms
uniform mat3 uModel_to_NDC_Mat;
//uniform mat4 uProjection;
//uniform mat3 uTranslation;

// Just my transform
void main() {
	gl_Position = vec4(vec2(uModel_to_NDC_Mat * vec3(aVertex.xy, 1.0f)), 0.0, 1.0); 
	vTextCoord = aVertex.zw; 
}

//// Just projection transform
//void main() {
//	gl_Position = uProjection * vec4(aVertex.xy, 0.0, 1.0); 
//	vTextCoord = aVertex.zw; 
//}

// Projection and translation
//void main() {
//	
//	gl_Position = uProjection * vec4(vec2(uTranslation * vec3(aVertex.xy, 1.0f)), 0.0, 1.0);
//	vTextCoord = aVertex.zw; 
//}

// Combining my transform with projection
//void main() {
//	gl_Position = uProjection * vec4(vec2(uModel_to_NDC_Mat * vec3(aVertex.xy, 1.0f)), 0.0, 1.0); 
//	vTextCoord = aVertex.zw; 
//}
