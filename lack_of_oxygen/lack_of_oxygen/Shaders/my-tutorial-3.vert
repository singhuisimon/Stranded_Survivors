/*!
@file		my-tutorial-3.vert
@author		c.wenbinkenny@digipen.edu
@date		22/05/2023

This file implements the vertex shader that read per-vertex data into
vertex attributes from vertex attributes indices. This shader sets up 
the vertices and the colors associated with it.

*//*__________________________________________________________________________*/
// OpenGL Version
#version 450 core

layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec3 aVertexColor;
layout(location = 0) out vec3 vColor;

uniform mat3 uModel_to_NDC;

void main() {
	gl_Position = vec4(vec2(uModel_to_NDC * vec3(aVertexPosition, 1.0f)), 0.0, 1.0);
	vColor = aVertexColor;
}