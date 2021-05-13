#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec4 color;
uniform int drawIndex;

out vec3 fragPos;
out vec4 vColor;
out vec3 vNormal;

void main() {
    float w =  1.0f + (drawIndex / 10000.0f); // used to move very slightly each cube's vertices using back to front ordering to avoid overlapping
    gl_Position = projection * view * model * vec4(position, w); // compute mvp matrix and apply to the vertex position
    fragPos = vec3(model * vec4(position, w)); // calculate only the fragment position for the fragment shader
    vNormal = normal; // pass normal to the fragment shader
    vColor = color;
}
