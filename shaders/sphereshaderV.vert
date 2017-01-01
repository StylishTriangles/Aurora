#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal;

uniform mat4 vp; // projection * view
uniform mat4 modelMat;
uniform mat4 modelNorm;

void main()
{
    gl_Position = vp * modelMat * vec4(position, 1.0f);
    FragPos = vec3(modelMat * vec4(position, 1.0f));
    TexCoord = vec2(texCoord.x, texCoord.y);
    Normal = mat3(modelNorm)*normal;
}
