#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include <vector>

#include "../headers/Window.h"
#include "../headers/sponge.h"

using namespace std;

/**
 * Main function
 * @return
 */
int main() {
    Window window;
    window.initOpenGL();
    window.loadShaders();
    window.createArraysAndBuffers();

    /* CUBE VERTICES
     * Define cube unique points used to generate vertices, indices and normals */
    vector<float> cubePoints = {
            0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 1.0f,  0.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f,
    };
    vector<float> spongeCubeVertices; vector<float> spongeCubeNormals; vector<uint32_t> spongeCubeIndices;
    /* Generate Menger's Sponge vertices and indices */
    subdivide(3, cubePoints, spongeCubeVertices, spongeCubeIndices);
    /* Duplicate vertices used by many "sides" to allow calculation of independent vertices normals */
    duplicateVertices(spongeCubeVertices, spongeCubeIndices);
    /* Compute said normals */
    computeSpongeNormals(spongeCubeVertices, spongeCubeIndices, spongeCubeNormals);
    /* Load vertices, normals and indices to buffers */
    window.fillVertexArray(VAO_ID::CUBE, spongeCubeVertices, spongeCubeNormals, spongeCubeIndices);

    /* TRAPEZE VERTICES
     * Define trapeze unique points used to generate cube vertices, indices and normals */
    vector<float> trapezePoints = {
            0.00f, 0.00f, 0.00f,  1.00f, 0.00f, 0.00f,  0.00f, 1.00f, 0.00f,  1.00f, 1.00f, 0.00f,
            0.25f, 0.25f, 0.25f,  0.75f, 0.25f, 0.25f,  0.25f, 0.75f, 0.25f,  0.75f, 0.75f, 0.25f,
    };
    vector<float> spongeTrapezeVertices; vector<float> spongeTrapezeNormals; vector<uint32_t> spongeTrapezeIndices;
    /* Generate Menger's Sponge vertices and indices */
    subdivide(3, trapezePoints, spongeTrapezeVertices, spongeTrapezeIndices);
    /* Duplicate vertices used by many "sides" to allow calculation of independent vertices normals */
    duplicateVertices(spongeTrapezeVertices, spongeTrapezeIndices);
    /* Compute said normals */
    computeSpongeNormals(spongeTrapezeVertices, spongeTrapezeIndices, spongeTrapezeNormals);
    /* Load vertices, normals and indices to buffers */
    window.fillVertexArray(VAO_ID::TRAPEZE, spongeTrapezeVertices, spongeTrapezeNormals, spongeTrapezeIndices);

    /* RENDER LOOP */
    while (window.continueLoop()) {
        /* Draw the background and clear OpenGL render bits */
        Window::clear();

        /* Update the camera position based on mouse movements */
        glm::vec3 cameraPosition = window.updateCamera();

        /* Initialize view matrix from camera and perspective projection matrix */
        glm::mat4 view = glm::lookAt(cameraPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(glm::pi<float>() / 4.0f, (float) Window::WIDTH / (float) Window::HEIGHT, 0.1f, 100.0f);
        /* Push view and projection matrix to the gpu through uniforms */
        window.loadUniformMat4f("view", view);
        window.loadUniformMat4f("projection", projection);

        /* Set and push vertices color to the gpu through uniform */
        glm::vec4 color = glm::vec4(0.5f, 0.5f, 0.5f, 0.7f);
        window.loadUniformVec4f("color", color);
        /* Draw the scene from the trapeze vertex array */
        window.drawScene(VAO_ID::TRAPEZE);

        /* Set and push vertices color to the gpu through uniform */
        color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        window.loadUniformVec4f("color", color);
        /* Draw the scene from the cube vertex array */
        window.drawScene(VAO_ID::CUBE);

        /* Swap the framebuffer to apply changes onto the screen */
        window.blit();
    }

    /* Release buffers and close the window */
    window.close();
    return 0;
}
