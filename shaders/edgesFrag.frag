#version 430 core
in vec3 mCol;

out vec4 color;

void main()
{
    color = vec4(mCol,1.0f);
}
