#version 430 core
in vec2 TexCoord;

out vec4 color;

uniform sampler2D diffuseMap;

void main()
{
    color = texture2D(diffuseMap, TexCoord);
}
