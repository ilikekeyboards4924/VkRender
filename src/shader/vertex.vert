#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;
layout(location = 0) out vec4 fragColor;

void main() {
    // make sure the w component is 1.0, not 0.0 (divide by zero error)
	gl_Position = vec4(position, 0.0, 1.0);

	fragColor = vec4(color, 1.0);
}