#ifndef FRACTALS_PLATONIC4D_WINDOW_H
#define FRACTALS_PLATONIC4D_WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <time.h>

#include "Sponge.h"

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

/**
 * Wrapping class for hypercube data management and OpenGL API interaction
 */
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

    int8_t targetFPS = 60;
    double deltaTime = 0.0f;
    double lastFrame = 0.0f;

    GLFWwindow* window{};
    uint32_t program = 0;
    uint32_t VAO[VAO_ID::NUMBER]{}, VBO[VAO_ID::NUMBER]{}, NBO[VAO_ID::NUMBER]{}, IBO[VAO_ID::NUMBER]{};
    vector<float> points[VAO_ID::NUMBER]{};
    vector<float> vertices[VAO_ID::NUMBER]{};
    vector<float> normals[VAO_ID::NUMBER]{};
    vector<uint32_t> indices[VAO_ID::NUMBER]{};

    Sponge sponge;

public:
    Window();
    ~Window() = default;

    /**
     * Initialize points, create corresponding vertices, normals and indices and push them to the GPU
     */
    void createMengerSpongeLikeHypercube();

    /**
     * Render loop, compute view and projection matrix then calls drawScene for each VAO
     */
    void renderMengerSpongeLikeHypercube();

    /**
     * Clear allocated buffers and closes the window
     */
    void close();
private:
    /**
     * Initialize OpenGL, glfw and glad
     * @return
     */
    void initOpenGL();

    /**
     * Reads and compile shaders then adds them to the program
     */
    void loadShaders();

    /**
     * Allocate and link array/buffers to OpenGL
     */
    void createArraysAndBuffers();

    /**
     * Uses the VAOs points array to create vertices, indices and normals. Then load them onto the gpu buffers
     * and create pointers to those memory spaces for shaders to access them
     * @param ID of the VAO used to store the data
     */
    void fillVertexArray(VAO_ID ID);

    /**
     * Sends a 4x4 matrix to a GPU uniform
     * @param name of the uniform in the shader
     * @param mat to send
     */
    void loadUniformMat4f(const char* name, const glm::mat4 &mat) const;

    /**
     * Sends a vec4 to a GPU uniform
     * @param name of the uniform in the shader
     * @param vec to send
     */
    void loadUniformVec4f(const char* name, const glm::vec4 &vec) const;

    /**
     * Binds the selected VAO and draw it's content
     * @param ID of the VAO to draw
     */
    void drawScene(VAO_ID ID);

    /**
     * Reset background and buffer bit
     */
    static void clear();

    /**
     * Update the screen to reflect changes
     */
    void blit();

    /**
     * Sleeps to limit framerate
     */
    void waitNextFrame() const;

    /**
     * Gives window status
     * @return true if window must stay open, false otherwise
     */
    bool continueLoop();

    /**
     * Reads mouse input and return updated position vector for the camera
     * @return
     */
    glm::vec3 updateCamera();

    /**
     * Enable alpha blending for transparency
     */
    static void enableBlending();

    /**
     * Enable culling of faces pointing in the wrong direction
     */
    static void enableFaceCulling();

    /**
     * Enables OpenGL depth test
     */
    static void enableDepthTest();

    /**
     * Disable OpenGL depth test
     */
    static void disableDepthTest();

    /**
     * Reads a shader file and return a string of it's content
     * @param file path
     * @return string
     */
    static string readShaderFile(const char* file);

    /**
     * Checks a component for compilation errors
     * @param shader ID
     * @param type of the compiled component (VERTEX, FRAGMENT, PROGRAM)
     */
    static void checkCompileErrors(uint32_t shader, const string& type);

    /**
     * Compiles a shader from it's code
     * @param ID of the shader
     * @param code content as string
     * @param type of shader (VERTEX, FRAGMENT)
     */
    static void createShader(uint32_t shader, const char* &code, const char* type);

    /**
     * Compiles shaders and add the to the program
     * @param vShader file path
     * @param fShader file path
     */
    void initProgram(const char* vShader, const char* fShader) const;

    /**
     * Called when the window is resized, update the viewport accordingly
     * @param window unused
     * @param new width of the window
     * @param new height of the window
     */
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    /**
     * Called when the user presses a key
     * @param window unused
     * @param key pressed
     * @param scancode unused
     * @param action type
     * @param mods unused
     */
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    /**
     * Called when the user presses a mouse button
     * @param window unused
     * @param button pressed
     * @param action type
     * @param mods unused
     */
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

    /**
     * Called when the user scrolls
     * @param window unused
     * @param xoffset is the rotation difference of the movement in the X axis
     * @param yoffset is the rotation difference of the movement in the Y axis
     */
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};


#endif //FRACTALS_PLATONIC4D_WINDOW_H
