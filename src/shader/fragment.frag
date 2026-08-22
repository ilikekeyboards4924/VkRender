#version 450

layout(location = 0) in vec4 fragColor;
layout(location = 0) out vec4 outColor;

void main() {
	outColor = vec4(0.5, 1.0, 0.3, 1.0);
	//outColor = fragColor;
}