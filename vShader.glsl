#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec4 color;

out vec3 fragPos;
out vec4 vColor;
out vec3 vNormal;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0f); // compute mvp matrix and apply to the vertex position
    fragPos = vec3(model * vec4(position, 1.0f)); // calculate only the fragment position for the fragment shader
    vNormal = normal; // pass normal to the fragment shader
    vColor = color;
}
