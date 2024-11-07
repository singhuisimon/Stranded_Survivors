/**
 * @file lack_of_oxygen_1.frag
 * @brief This file implements the fragment shader for game objects that 
 *        sets the color of the fragments' pixel.
 * @author Chua Wen Bin Kenny (100%)
 * @date September 20, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
*/
// OpenGL Version
#version 450 core

// In
layout (location = 0) in vec2 vTextCoord;

// Out
layout (location = 0) out vec4 fFragColor;

// Uniforms
uniform vec3 uColor;
uniform sampler2D uTex2d; 
uniform bool TexFlag;

void main() {
	if(TexFlag == true) {
		fFragColor = texture(uTex2d, vTextCoord);
	} else {
		fFragColor = vec4(uColor, 1.0);
	}
}