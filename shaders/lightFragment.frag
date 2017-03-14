#version 430 core
in vec2 TexCoord;

out vec4 color;

uniform sampler2D tex;
uniform vec3 mColor;

void main()
{
//    color = texture2D(tex, TexCoord);
    color = vec4(mColor,1.0f);
}
