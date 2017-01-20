#version 430 core
layout (location = 0) in vec2 position;

uniform mat4 vp; // projection * view
uniform mat4 modelMat;

void main()
{
    gl_Position = vp * modelMat * vec4(position, 0.0f, 1.0f);
}
