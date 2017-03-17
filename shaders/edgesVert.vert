#version 430 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec3 col;

out vec3 mCol;

uniform mat4 vp; // projection * view
uniform mat4 modelMat;

void main()
{
    gl_Position = vp * modelMat * vec4(position, 0.0f, 1.0f);
    mCol=col;
}
