#version 410 core

in vec3 v_vertexColors;

out vec4 color;

uniform float offset_y;
uniform float offset_x;

void main() {
    color = vec4(v_vertexColors.r - offset_y, v_vertexColors.g, v_vertexColors.b - offset_x, 1.0f);
}
