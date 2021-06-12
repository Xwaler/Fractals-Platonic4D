#version 450 core

in vec2 TexCoord;

uniform sampler2D overlayTexture;

out vec4 FragColor;

void main() {
    FragColor = texture(overlayTexture, TexCoord); // compute fragment from the texture
}
