#version 430 core
in vec2 TexCoord;

out vec4 color;

uniform sampler2D tex;

void main()
{
//    color = texture2D(tex, TexCoord);
    color = vec4(1.0f,1.0f,1.0f,1.0f);
}
