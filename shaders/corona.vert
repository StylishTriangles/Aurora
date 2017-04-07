#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec2 TexCoord;
out vec3 fPosition;

uniform mat4 vp; // projection * view
uniform mat4 modelMat;
uniform vec3 camFront;
uniform vec3 camUp;
uniform float radius;

const float eps = 0.001;
const float pi = 3.1415926535897932384;
float angles[4] = float[](pi/4, pi/4+pi/2, pi/4+pi, -pi/4);
vec3 pts[4] = vec3[](vec3(1,1,0), vec3(1,-1,0), vec3(-1,-1,0), vec3(-1,1,0));

float dist2(vec3 a, vec3 b)
{
    return (a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y)+(a.z-b.z)*(a.z-b.z);
}

mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

void main()
{
//    mat4 rot;
//    for (int i = 0; i < 4; i++) {
//        if (dist2(pts[i],position) < eps)
//            rot = rotationMatrix(camFront, angles[i]);
//    }
    gl_Position = vp * modelMat * vec4(position*4, 1.0f);
    fPosition = position;
    TexCoord = vec2(texCoord.x, texCoord.y);
}
