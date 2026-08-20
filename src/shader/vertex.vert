#version 450

layout(location = 0) out vec4 fragColor;
layout(location = 2) in vec2 position;
layout(location = 3) in vec3 color;
layout(location = 4) in vec2 offset;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

vec2 vertices[3] = vec2[](
    vec2(0.0, -1.0),
    vec2(1.0, 1.0),
    vec2(-1.0, 1.0)
);

vec4 colors[3] = vec4[](
    vec4(1.0, 0.0, 0.0, 1.0),
    vec4(0.0, 1.0, 0.0, 1.0),
    vec4(0.0, 0.0, 1.0, 1.0)
);

void main() {
    // make sure the w component is 1.0, not 0.0 (divide by zero error)
	gl_Position = vec4((position + offset), 0.0, 1.0) * ubo.model;
    //outColor = colors[gl_VertexIndex];
    fragColor = vec4(color, 1.0);
}