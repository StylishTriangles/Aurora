#version 430 core
in vec2 TexCoord;

out vec4 color;

uniform sampler2D ourTexture1;

void main()
{
    color = texture2D(ourTexture1, TexCoord);
//    color = vec4(1.0f,0.0f,0.0f,0.0f);
}
