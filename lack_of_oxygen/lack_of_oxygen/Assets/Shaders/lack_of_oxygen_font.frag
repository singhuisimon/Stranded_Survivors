/**
 * @file lack_of_oxygen_font.frag
 * @brief This file implements the fragment shader for fonts that 
 *        sets the color of the font's fragments pixel.
 * @author Chua Wen Bin Kenny (100%)
 * @date November 5, 2024
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
uniform sampler2D uText;
uniform vec3 uTextColor;

void main() {
	vec4 SampledText = vec4(1.0, 1.0, 1.0, texture(uText, vTextCoord).r);
	fFragColor = vec4(uTextColor, 1.0) * SampledText;
}