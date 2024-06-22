#version 410 core

// in vec4 position;

layout(location=0) in vec3 position;
layout(location=1) in vec3 vertexColors;

out vec3 v_vertexColors;

uniform mat4 model_matrix;
uniform mat4 perspective;

void main() {
    v_vertexColors = vertexColors;
    vec4 position_new = perspective * model_matrix * vec4(position, 1.0f);
    // what the fuck is the .w component???
    gl_Position = vec4(position_new.x, position_new.y, position_new.z, position_new.w);
}
