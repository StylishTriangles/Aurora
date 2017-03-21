#version 430 core
out vec4 color;

uniform vec3 col;

void main()
{
    color = vec4(col,0.2f);
}
