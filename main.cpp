#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "shader.h"
#include "tools.h"

using namespace std;

glm::vec3 updateCamera(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

int WIDTH;
int HEIGHT;

float cameraDistance = 3.0f;
double scroll_speed = 0.2;
double mouse_speed = 0.2;
double xpos, ypos;
double mouse_pos_x = 0.0;
double mouse_pos_y = 0.0;
float horizontal_angle = 0.0f;
float vertical_angle = 0.0f;
bool leftButtonPressed = false;

bool wireframe = false;

double deltaTime = 0.0f;
double lastFrame = 0.0f;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    WIDTH = 1280;
    HEIGHT = 720;
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Fractals-Platonic4D", nullptr, nullptr);
    if (window == nullptr) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    glEnable(GL_MULTISAMPLE);

    glEnable(GL_LINE_SMOOTH);
    glLineWidth(4);

    unsigned int program = glCreateProgram();
    initProgram(program, "../vShader.glsl", "../fShader.glsl");
    glUseProgram(program);

    vector<float> cubeVertices = {
            1.0f, 1.0f, 1.0f,  1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f, // (right)
            1.0f, 1.0f, 1.0f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 1.0f, // (top)
            1.0f, 1.0f, 1.0f,  0.0f, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 1.0f, // (front)
            0.0f, 1.0f, 1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, // (left)
            0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f, // (bottom)
            1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, 0.0f, // (behind)
    };
    vector<float> cubeNormals = computeTriangleNormalsFromQuads(cubeVertices);
    vector<float> trapezeVertices = {
            0.75f, 0.75f, 0.25f,  0.75f, 0.25f, 0.25f,  1.00f, 0.00f, 0.00f,  1.00f, 1.00f, 0.00f, // (right)
            0.75f, 0.75f, 0.25f,  1.00f, 1.00f, 0.00f,  0.00f, 1.00f, 0.00f,  0.25f, 0.75f, 0.25f, // (top)
            0.75f, 0.75f, 0.25f,  0.25f, 0.75f, 0.25f,  0.25f, 0.25f, 0.25f,  0.75f, 0.25f, 0.25f, // (front)
            0.25f, 0.75f, 0.25f,  0.00f, 1.00f, 0.00f,  0.00f, 0.00f, 0.00f,  0.25f, 0.25f, 0.25f, // (left)
            0.00f, 0.00f, 0.00f,  1.00f, 0.00f, 0.00f,  0.75f, 0.25f, 0.25f,  0.25f, 0.25f, 0.25f, // (bottom)
            1.00f, 0.00f, 0.00f,  0.00f, 0.00f, 0.00f,  0.00f, 1.00f, 0.00f,  1.00f, 1.00f, 0.00f, // (behind)
    };
    vector<float> trapezeNormals = computeTriangleNormalsFromQuads(trapezeVertices);
    vector<unsigned int> triangleIndices = computeTriangleIndicesFromQuads((int) trapezeVertices.size() / 3);

    unsigned int VAO[2], VBO[2], NBO[2], IBO;
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);
    glGenBuffers(2, NBO);
    glGenBuffers(1, &IBO);

    glBindVertexArray(VAO[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(float), cubeVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (GLvoid*) nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, NBO[0]);
    glBufferData(GL_ARRAY_BUFFER, cubeNormals.size() * sizeof(float), cubeNormals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (GLvoid*) nullptr);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangleIndices.size() * sizeof(float), triangleIndices.data(), GL_STATIC_DRAW);

    glBindVertexArray(VAO[1]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, trapezeVertices.size() * sizeof(float), trapezeVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (GLvoid*) nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, NBO[1]);
    glBufferData(GL_ARRAY_BUFFER, trapezeNormals.size() * sizeof(float), trapezeNormals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (GLvoid*) nullptr);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangleIndices.size() * sizeof(float), triangleIndices.data(), GL_STATIC_DRAW);

    while (!glfwWindowShouldClose(window)) {
        glm::vec3 cameraPosition = updateCamera(window);

        glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model;
        glm::mat4 view = glm::lookAt(cameraPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(glm::pi<float>() / 4.0f, (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO[1]);
        glDisable(GL_DEPTH_TEST);

        glUniform4fv(glGetUniformLocation(program, "color"), 1, glm::value_ptr(glm::vec4(0.5f, 0.5f, 0.5f, 0.7f)));
        for (unsigned int i = 0; i < 4; ++i) {
            model = glm::rotate(glm::mat4(1.0f), (float) i * glm::pi<float>() / 2.0f, glm::vec3(0.0f, 1.0f, 0.0f)) *
                    glm::translate(glm::mat4(1), glm::vec3(-0.5));
            glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glDrawElements(GL_TRIANGLES, triangleIndices.size(), GL_UNSIGNED_INT, nullptr);
        }
        for (int i = -1; i < 2; i += 2) {
            model = glm::rotate(glm::mat4(1.0f), (float) i * glm::pi<float>() / 2.0f, glm::vec3(1.0f, 0.0f, 0.0f)) *
                    glm::translate(glm::mat4(1), glm::vec3(-0.5));
            glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glDrawElements(GL_TRIANGLES, triangleIndices.size(), GL_UNSIGNED_INT, nullptr);
        }

        glBindVertexArray(VAO[0]);
        glEnable(GL_DEPTH_TEST);

        glUniform4fv(glGetUniformLocation(program, "color"), 1, glm::value_ptr(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)));
        model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f)) * glm::translate(glm::mat4(1), glm::vec3(-0.5));
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawElements(GL_TRIANGLES, triangleIndices.size(), GL_UNSIGNED_INT, nullptr);

        glUniform4fv(glGetUniformLocation(program, "color"), 1, glm::value_ptr(glm::vec4(0.0f, 1.0f, 0.0f, 0.1f)));
        model = glm::translate(glm::mat4(1), glm::vec3(-0.5));
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawElements(GL_TRIANGLES, triangleIndices.size(), GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(2, VAO);
    glDeleteBuffers(2, VBO);
    glDeleteBuffers(2, NBO);
    glDeleteBuffers(1, &IBO);
    glDeleteProgram(program);
    glfwTerminate();
    return 0;
}

glm::vec3 updateCamera(GLFWwindow* window) {
    double currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glfwGetCursorPos(window, &xpos, &ypos);
    if (leftButtonPressed) {
        horizontal_angle += (float) (deltaTime * mouse_speed * (mouse_pos_x - xpos));
        vertical_angle += (float) (deltaTime * mouse_speed * (ypos - mouse_pos_y));
    }
    mouse_pos_x = xpos;
    mouse_pos_y = ypos;
    return cameraDistance * glm::vec3(
            glm::sin(horizontal_angle) * glm::cos(vertical_angle),
            glm::sin(vertical_angle),
            glm::cos(horizontal_angle) * glm::cos(vertical_angle)
    );
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnusedLocalVariable"
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    WIDTH = width;
    HEIGHT = height;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        wireframe = !wireframe;
        wireframe ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE): glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        leftButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        leftButtonPressed = false;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    cameraDistance = glm::max(0.5f, cameraDistance - (float) (yoffset * scroll_speed));
}
#pragma clang diagnostic pop
