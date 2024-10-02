/*!
@file		my-tutorial-4.vert
@author		c.wenbinkenny@digipen.edu
@date		28/05/2023

This file implements the vertex shader that read per-vertex data into
vertex attributes from vertex attributes indices. This shader sets up 
the vertices and the colors associated with it.

*//*__________________________________________________________________________*/
// OpenGL Version
#version 450 core

layout(location = 0) in vec2 aVertexPosition;

uniform mat3 uModel_to_NDC_Mat;

void main() {
	gl_Position = vec4(vec2(uModel_to_NDC_Mat * vec3(aVertexPosition, 1.0f)), 0.0, 1.0);
}