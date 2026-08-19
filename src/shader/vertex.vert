#version 450

vec2 vertices[3] = vec2[](
    vec2(0.0, -1.0),
    vec2(1.0, 1.0),
    vec2(-1.0, 1.0)
);

void main() {
    // make sure the w component is 1.0, not 0.0 (divide by zero error)
	gl_Position = vec4(vertices[gl_VertexIndex], 0.0, 1.0);
}