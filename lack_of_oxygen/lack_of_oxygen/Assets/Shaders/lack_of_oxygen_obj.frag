/**
 * @file lack_of_oxygen_obj.frag
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
uniform int uFrameNo;

void main() {
	if(uTexFlag == true && uAnimateFlag == true) {
		ivec2 tex_size;
		tex_size = textureSize(uTex2d, 0);
		int padding = 2;
		int frame_size = 16;

		// Size is always 16 for each spritesheet texture/frame
		float scaling_x = tex_size.x / float(frame_size); // Scaling factor to resize TextCoord.x accordingly
		float scaling_y = tex_size.y / float(frame_size); // Scaling factor to resize TextCoord.y accordingly
			
		// Find number of col and rows of textures/frames
		int cols = (tex_size.x - padding) / (frame_size + padding);
		int rows = (tex_size.y - padding) / (frame_size + padding);

		// Find position x & y based on frame number
		float Pos_X = padding + (uFrameNo % cols) * (frame_size + padding);
		int y_pos_multiplier = uFrameNo / cols;
		float Pos_Y = padding + (rows - 1 - y_pos_multiplier) * (frame_size + padding);

		float offset_x = Pos_X / frame_size;  // Accounting for offset due to position in texture
		float offset_y = Pos_Y / frame_size;
		fFragColor = texture(uTex2d, vec2(vTextCoord.x / scaling_x, vTextCoord.y / scaling_y) + vec2(offset_x / scaling_x, offset_y / scaling_y));
	} else if (uTexFlag == true){
		fFragColor = texture(uTex2d, vTextCoord);	
	} else {
		fFragColor = vec4(uColor, 1.0);
	}
}