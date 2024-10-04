/**
 * @file lack_of_oxygen_1.frag
 * @brief This file implements the fragment shader that sets the color of the fragments' pixel
 * @author Chua Wen Bin Kenny (100%)
 * @date September 20, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
*/
// OpenGL Version
#version 450 core

uniform vec3 uColor;

layout (location=0) out vec4 fFragColor;

void main() {
	fFragColor = vec4(uColor, 1.0);
}