/*!
@file		my-tutorial-4.frag
@author		c.wenbinkenny@digipen.edu
@date		28/05/2023

This file implements the fragment shader that sets the color  of the 
fragments' pixel by using the interpolated values from the shader
preceding the rasterizer.

*//*__________________________________________________________________________*/
#version 450 core

uniform vec3 uColor;

layout (location=0) out vec4 fFragColor;

void main() {
	fFragColor = vec4(uColor, 1.0);
}
