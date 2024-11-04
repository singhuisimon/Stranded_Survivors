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
uniform bool uTexFlag;		// Flag for texture
uniform bool  uAnimateFlag; // Flag for animation
uniform float uTex_W;
uniform float uTex_H;
uniform float uFrame_Size;	// Usually frame w and h is the same, split if needed
uniform float uPos_X;		// Pos x in texture
uniform float uPos_Y;		// Pos y in texture

void main() {
	if(uTexFlag == true && uAnimateFlag == true) {
		float scaling_x = uTex_W / uFrame_Size; // Scaling factor to resize TextCoord.x accordingly
		float scaling_y = uTex_H / uFrame_Size; // Scaling factor to resize TextCoord.y accordingly	
		float offset_x = uPos_X / uFrame_Size;  // Accounting for offset due to position in texture
		float offset_y = uPos_Y / uFrame_Size;
		fFragColor = texture(uTex2d, vec2(vTextCoord.x / scaling_x, vTextCoord.y / scaling_y) + vec2(offset_x / scaling_x, offset_y / scaling_y));
	} else if (uTexFlag == true){
		fFragColor = texture(uTex2d, vTextCoord);	
	} else {
		fFragColor = vec4(uColor, 1.0);
	}
}