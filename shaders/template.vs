#version 330 core

layout(location=0) in vec2 pos;
layout(location=1) in float speed;
//layout(location=2) in vec2 tex;

uniform mat4 mvp;

out float vert_speed;

void main() {
    vert_speed = speed;
    gl_Position = mvp * vec4(pos, 0.0, 1.0);

    gl_PointSize = 1.0;
}
