#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec2 TexCoord;
out vec3 fPosition;

uniform mat4 vp; // projection * view
uniform mat4 modelMat;

void main()
{
    gl_Position = vp * modelMat * vec4(position, 1.0f);
    fPosition = vec3(modelMat * vec4(position, 1.0f));
    TexCoord = vec2(texCoord.x, texCoord.y);
}
