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

int WIDTH;
int HEIGHT;

const float cameraDistance = 3.0f;
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

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

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
    vector<float> cubeNormals = computeQuadNormals(cubeVertices);
    vector<unsigned int> cubeIndices = computeQuadIndices((int) cubeVertices.size() / 3);

    unsigned int VAO, VBO[2], IBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(2, VBO);
    glGenBuffers(1, &IBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(float), cubeVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (GLvoid*) nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, cubeNormals.size() * sizeof(float), cubeNormals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (GLvoid*) nullptr);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices.size() * sizeof(float), cubeIndices.data(), GL_STATIC_DRAW);

    while (!glfwWindowShouldClose(window)) {
        glm::vec3 cameraPosition = updateCamera(window);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(cameraPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(glm::pi<float>() / 4.0f, (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

        glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f)) * glm::translate(glm::mat4(1), glm::vec3(-0.5));
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniform4fv(glGetUniformLocation(program, "color"), 1, glm::value_ptr(glm::vec4(0.9f, 0.0f, 0.0f, 0.6f)));
        glDrawElements(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, nullptr);

        model = glm::translate(glm::mat4(1), glm::vec3(-0.5));
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniform4fv(glGetUniformLocation(program, "color"), 1, glm::value_ptr(glm::vec4(0.8f, 0.8f, 0.8f, 0.6f)));
        glDrawElements(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(2, VBO);
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
#pragma clang diagnostic pop
