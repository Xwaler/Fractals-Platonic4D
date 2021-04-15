#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "window.h"
#include "shader.h"
#include "sponge.h"
#include "tools.h"

using namespace std;

/**
 * Main function
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char* argv[]) {
    /* OPENGL INITIALIZATION */

    // Initialize glfw and its base parameters
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Initialize glfw window and glad utilities
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Fractals-Platonic4D", nullptr, nullptr);
    if (window == nullptr) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // set input event callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // enable some opengl capacities
    enableBlending();
    enableDepthTest();
    enableFaceCulling();
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(4);

    // compile shaders and program
    unsigned int program = glCreateProgram();
    initProgram(program, "../shaders/vShader.glsl", "../shaders/fShader.glsl");
    glUseProgram(program);

    // initialize vertex arrays and buffers
    unsigned int VAO[2], VBO[2], NBO[2], IBO[2];
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);
    glGenBuffers(2, NBO);
    glGenBuffers(2, IBO);


    /* CUBE VERTICES */

    // define cube unique points used to generate vertices, indices and normals
    vector<float> cubePoints = {
            0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 1.0f,  0.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f,
    };
    vector<float> spongeCubeVertices; vector<unsigned int> spongeCubeIndices; vector<float> spongeCubeNormals;
    // generate menger vertices and indices
    subdivide(0, cubePoints, spongeCubeVertices, spongeCubeIndices);
    // duplicate vertices used by many "sides" to allow calculation of independent vertices normals
    duplicateVertices(spongeCubeVertices, spongeCubeIndices);
    // compute said normals
    getSpongeNormals(spongeCubeVertices, spongeCubeIndices, spongeCubeNormals);

    // bind the first vertex array reserved for the cube vertices
    glBindVertexArray(VAO[0]);
    // bind vertex buffer 0 to vertex array 0
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    // buffer cube vertices to vertex buffer 0
    glBufferData(GL_ARRAY_BUFFER, spongeCubeVertices.size() * sizeof(float), spongeCubeVertices.data(), GL_STATIC_DRAW);
    // assign the buffer content to vertex array pointer 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (GLvoid*) nullptr);
    glEnableVertexAttribArray(0);
    // bind normals buffer 0 to vertex array 0
    glBindBuffer(GL_ARRAY_BUFFER, NBO[0]);
    // buffer cube normals to vertex buffer 0
    glBufferData(GL_ARRAY_BUFFER, spongeCubeNormals.size() * sizeof(float), spongeCubeNormals.data(), GL_STATIC_DRAW);
    // assign the buffer content to vertex array pointer 1
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (GLvoid*) nullptr);
    glEnableVertexAttribArray(1);
    // bind indices buffer 0 to vertex array 0
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO[0]);
    // buffer cube indices to vertex buffer 0
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, spongeCubeIndices.size() * sizeof(float), spongeCubeIndices.data(), GL_STATIC_DRAW);


    /* TRAPEZE VERTICES */

    // define trapeze unique points used to generate cube vertices, indices and normals
    vector<float> trapezePoints = {
            0.00f, 0.00f, 0.00f,  1.00f, 0.00f, 0.00f,  0.00f, 1.00f, 0.00f,  1.00f, 1.00f, 0.00f,
            0.25f, 0.25f, 0.25f,  0.75f, 0.25f, 0.25f,  0.25f, 0.75f, 0.25f,  0.75f, 0.75f, 0.25f,
    };
    vector<float> spongeTrapezeVertices; vector<unsigned int> spongeTrapezeIndices; vector<float> spongeTrapezeNormals;
    // generate menger vertices and indices
    subdivide(0, trapezePoints, spongeTrapezeVertices, spongeTrapezeIndices);
    // duplicate vertices used by many "sides" to allow calculation of independent vertices normals
    duplicateVertices(spongeTrapezeVertices, spongeTrapezeIndices);
    // compute said normals
    getSpongeNormals(spongeTrapezeVertices, spongeTrapezeIndices, spongeTrapezeNormals);

    // bind the second vertex array reserved for the trapeze vertices
    glBindVertexArray(VAO[1]);
    // bind vertex buffer 1 to vertex array 1
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    // buffer trapeze vertices to vertex buffer 1
    glBufferData(GL_ARRAY_BUFFER, spongeTrapezeVertices.size() * sizeof(float), spongeTrapezeVertices.data(), GL_STATIC_DRAW);
    // assign the buffer content to vertex array pointer 1
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (GLvoid*) nullptr);
    glEnableVertexAttribArray(0);
    // bind normals buffer 1 to vertex array 1
    glBindBuffer(GL_ARRAY_BUFFER, NBO[1]);
    // buffer trapeze normals to vertex buffer 1
    glBufferData(GL_ARRAY_BUFFER, spongeTrapezeNormals.size() * sizeof(float), spongeTrapezeNormals.data(), GL_STATIC_DRAW);
    // assign the buffer content to vertex array pointer 1
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (GLvoid*) nullptr);
    glEnableVertexAttribArray(1);
    // bind indices buffer 1 to vertex array 1
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO[1]);
    // buffer trapeze indices to vertex buffer 1
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, spongeTrapezeIndices.size() * sizeof(float), spongeTrapezeIndices.data(), GL_STATIC_DRAW);


    /* RENDER LOOP */

    while (!glfwWindowShouldClose(window)) {
        glm::vec3 cameraPosition = updateCamera(window); // update the camera position based on mouse movements

        glClearColor(0.7f, 0.7f, 0.7f, 1.0f); // reset background color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear opengl buffers

        glm::mat4 model; // initialize objects model matrix
        // compute the (camera) view matrix
        glm::mat4 view = glm::lookAt(cameraPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0f, 0.0f));
        // compute the perspective projection matrix
        glm::mat4 projection = glm::perspective(glm::pi<float>() / 4.0f, (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);
        // push view and projection matrix to the gpu through uniforms
        glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // bind the trapeze vertex array object
        glBindVertexArray(VAO[1]);
        // disable depth test for transparency
        disableDepthTest();
        // push unique vertices color to the gpu through uniform
        glUniform4fv(glGetUniformLocation(program, "color"), 1, glm::value_ptr(glm::vec4(0.5f, 0.5f, 0.5f, 0.7f)));
        // four inline trapezes (with y=0.5)
        for (unsigned int i = 0; i < 1; ++i) {
            // compute model matrix for one trapeze, translate to center the object then rotate it to the right side
            model = glm::rotate(glm::mat4(1.0f), (float) i * glm::pi<float>() / 2.0f, glm::vec3(0.0f, 1.0f, 0.0f)) *
                    glm::translate(glm::mat4(1), glm::vec3(-0.5));
            // push model matrix to gpu through uniform
            glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
            // draw vertices and create fragments with triangles
            glDrawElements(GL_TRIANGLES, spongeTrapezeIndices.size(), GL_UNSIGNED_INT, nullptr);
        }
        // up (y=1.0) and down (y=0.0) trapezes
        for (int i = -1; i < -1; i += 2) {
            // compute model matrix for one trapeze, translate to center the object then rotate it to the right side
            model = glm::rotate(glm::mat4(1.0f), (float) i * glm::pi<float>() / 2.0f, glm::vec3(1.0f, 0.0f, 0.0f)) *
                    glm::translate(glm::mat4(1), glm::vec3(-0.5));
            // push model matrix to gpu through uniform
            glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
            // draw vertices and create fragments with triangles
            glDrawElements(GL_TRIANGLES, spongeTrapezeIndices.size(), GL_UNSIGNED_INT, nullptr);
        }

        // bind the cube vertex array object
        glBindVertexArray(VAO[0]);
        // re-enable depth test for solid objects
        enableDepthTest();
        // push unique vertices color to the gpu through uniform
        glUniform4fv(glGetUniformLocation(program, "color"), 1, glm::value_ptr(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)));
        // compute model matrix for one trapeze, translate to center the object then downscale it by 2
        model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f)) * glm::translate(glm::mat4(1), glm::vec3(-0.5));
        // push model matrix to gpu through uniform
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        // draw vertices and create fragments with triangles
        glDrawElements(GL_TRIANGLES, spongeCubeIndices.size(), GL_UNSIGNED_INT, nullptr);

        // glUniform4fv(glGetUniformLocation(program, "color"), 1, glm::value_ptr(glm::vec4(0.0f, 1.0f, 0.0f, 0.7f)));
        // model = glm::translate(glm::mat4(1), glm::vec3(-0.5));
        // glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        // glDrawElements(GL_TRIANGLES, spongeCubeIndices.size(), GL_UNSIGNED_INT, nullptr);

        // swap the screen buffer to reflect changes
        glfwSwapBuffers(window);
        // call predefined input event callbacks
        glfwPollEvents();
    }


    /* CLEANUP */

    // deallocate vertex arrays and buffer
    glDeleteVertexArrays(2, VAO);
    glDeleteBuffers(2, VBO);
    glDeleteBuffers(2, NBO);
    glDeleteBuffers(2, IBO);
    // delete compiled shader
    glDeleteProgram(program);
    // kill the window
    glfwTerminate();
    return 0;
}

/**
 * Get the current cursor position and update the camera position accordingly
 * @param window
 * @return
 */
glm::vec3 updateCamera(GLFWwindow* window) {
    // get elapsed time since last frame
    double currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // get new cursor position
    glfwGetCursorPos(window, &xpos, &ypos);
    if (leftButtonPressed) { // if the user is pressing the mouse left button, update the camera angle
        horizontal_angle += (float) (deltaTime * mouse_speed * (mouse_pos_x - xpos));
        vertical_angle += (float) (deltaTime * mouse_speed * (ypos - mouse_pos_y));
    }
    mouse_pos_x = xpos;
    mouse_pos_y = ypos;
    // use magic rotation trigonometry to compute the new camera position centered around the origin
    return cameraDistance * glm::vec3(
            glm::sin(horizontal_angle) * glm::cos(vertical_angle),
            glm::sin(vertical_angle),
            glm::cos(horizontal_angle) * glm::cos(vertical_angle)
    );
}

/**
 * Callback to resize the window
 * @param window
 * @param width
 * @param height
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    WIDTH = width;
    HEIGHT = height;
}

/**
 * Callback to process key presses
 * @param window
 * @param key
 * @param scancode
 * @param action
 * @param mods
 */
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) { // close the window
        glfwSetWindowShouldClose(window, true);
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) { // toggle wireframe display mode
        wireframe = !wireframe;
        wireframe ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE): glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

/**
 * Callback to process mouse button presses, used to detect when the user wants to move the camera
 * @param window
 * @param button
 * @param action
 * @param mods
 */
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        leftButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        leftButtonPressed = false;
    }
}

/**
 * Callback to process mouse wheel inputs
 * @param window
 * @param xoffset
 * @param yoffset
 */
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    cameraDistance = glm::max(0.5f, cameraDistance - (float) (yoffset * scroll_speed));
}
