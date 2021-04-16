#ifndef FRACTALS_PLATONIC4D_WINDOW_H
#define FRACTALS_PLATONIC4D_WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "../headers/shader.h"
#include "../headers/sponge.h"

using namespace std;

/**
 * ID of vertex array objects
 * NUMBER is the count of useful members in this enum
 */
enum VAO_ID {
    CUBE = 0,
    TRAPEZE = 1,
    NUMBER = 2,
};

class Window {
private:
    static int WIDTH;
    static int HEIGHT;
    static float cameraDistance;
    static double scroll_speed;
    static bool leftButtonPressed;
    static bool wireframe;

    double mouse_speed = 0.2;
    double xpos = 0.0, ypos = 0.0;
    double mouse_pos_x = 0.0;
    double mouse_pos_y = 0.0;
    float horizontal_angle = 0.0f;
    float vertical_angle = 0.0f;

    double deltaTime = 0.0f;
    double lastFrame = 0.0f;

    GLFWwindow* window{};
    uint32_t program = 0;
    uint32_t VAO[VAO_ID::NUMBER]{}, VBO[VAO_ID::NUMBER]{}, NBO[VAO_ID::NUMBER]{}, IBO[VAO_ID::NUMBER]{};
    vector<float> points[VAO_ID::NUMBER]{};
    vector<float> vertices[VAO_ID::NUMBER]{};
    vector<float> normals[VAO_ID::NUMBER]{};
    vector<uint32_t> indices[VAO_ID::NUMBER]{};

public:
    Window();
    ~Window() = default;

    void render();

    void close();
private:
    void initOpenGL();

    void loadShaders();

    void createArraysAndBuffers();

    void fillVertexArray(VAO_ID ID);

    void loadUniformMat4f(const char* name, const glm::mat4 &mat) const;

    void loadUniformVec4f(const char* name, const glm::vec4 &vec) const;

    void drawScene(VAO_ID ID);

    static void clear();

    void blit();

    bool continueLoop();

    glm::vec3 updateCamera();

    static void enableBlending();

    static void enableFaceCulling();

    static void enableDepthTest();

    static void disableDepthTest();

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};


#endif //FRACTALS_PLATONIC4D_WINDOW_H
