#version 430
layout (location = 0) in vec3 position;

out vec3 fragPos;

void main() {
    gl_Position = vec4( position, 1.0);
    fragPos = position;
}
