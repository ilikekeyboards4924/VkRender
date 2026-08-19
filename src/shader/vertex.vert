#version 450

layout(location = 0) out vec4 fragColor;

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
	gl_Position = vec4(vertices[gl_VertexIndex], 0.0, 1.0);
    //outColor = colors[gl_VertexIndex];
    fragColor = colors[gl_VertexIndex];
}