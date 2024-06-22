#version 410 core

// in vec4 position;

layout(location=0) in vec3 position;
layout(location=1) in vec3 vertexColors;

out vec3 v_vertexColors;

uniform float offset_y;
uniform float offset_x;

void main() {
    v_vertexColors = vertexColors;
    gl_Position = vec4(position.x + offset_x, position.y + offset_y, position.z, 1.0f);
}
