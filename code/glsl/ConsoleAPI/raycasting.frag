#version 330 core

layout(location = 0) out vec4 color;

void main() {
    vec2 uv = gl_FragCoord.xy;
    color = vec4(uv, 0.0, 1.0);
}