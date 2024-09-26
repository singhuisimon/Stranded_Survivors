/*!
@file		my-tutorial-3.frag
@author		c.wenbinkenny@digipen.edu
@date		22/05/2023

This file implements the fragment shader that sets the color  of the 
fragments' pixel by using the interpolated values from the shader
preceding the rasterizer.

*//*__________________________________________________________________________*/
#version 450 core

layout (location = 0) in vec3 vInterpColor;

layout (location = 0) out vec4 fFragColor;

void main() {
	fFragColor = vec4(vInterpColor, 1.0);
}
