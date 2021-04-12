#version 460 core

in vec3 fragPos;
in vec3 vNormal;
in vec3 vColor;

uniform vec4 color;

out vec4 FragColor;

struct light {
    vec3 pos; // position of the light source
    vec3 color; // light color
};

light lights[2] = {
    light(vec3(5.0f, 6.0f, 3.0f), vec3(1.0f, 1.0f, 1.0f)),
    light(vec3(-3.0f, -10.0f, -5.0f), vec3(1.0f, 1.0f, 1.0f)),
};

void main() {
    vec3 result = 0.4f * vec3(1.0f, 1.0f, 1.0f); // ambiant light

    vec3 norm = normalize(vNormal);
    for (uint i = 0; i < 2; ++i) {
        vec3 lightDir = normalize(lights[i].pos - fragPos); // vector between source and fragment position
        float diff = max(dot(norm, lightDir), 0.0); // diffusion component
        vec3 diffuse = diff * lights[i].color; // apply light source color
        result += diffuse; // add to the resulting fragment color
    }

    result *= color.rgb; // apply the objects original color
    FragColor = vec4(result, color.a); // add transparency
}
