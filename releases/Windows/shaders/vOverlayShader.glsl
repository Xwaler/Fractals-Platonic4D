#version 450 core

layout (location = 0) in vec2 textureCoords;

uniform mat4 projection;

out vec2 TexCoord;

void main() {
    gl_Position = projection * vec4(textureCoords, 0.0f, 1.0f); // apply ortho projection matrix to coordinates
    TexCoord = textureCoords;
}
