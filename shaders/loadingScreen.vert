layout (location = 0) in vec3 position;
//layout (location = 2) in vec2 uv;

out vec3 fragPos;
//out vec2 texPos;

void main() {
    gl_Position = vec4( position, 1.0);
    fragPos = position;
//    texPos = fragPos.xy;
}
