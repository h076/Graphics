#version 450 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D diffuseMap;

void main() {
    FragColor = texture(diffuseMap, TexCoord);
}