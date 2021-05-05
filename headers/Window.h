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
#include <thread>
#include <time.h>

#include "Hypercube.h"
#include "Sponge.h"
#include "Menu.h"

using namespace std;

/**
 * ID of vertex array objects
 * NUMBER is the count of useful members in this enum
 */
enum VAO_ID {
    PX = 0,
    NX = 1,
    PY = 2,
    NY = 3,
    PZ = 4,
    NZ = 5,
    PW = 6,
    NW = 7,
    WIRE_MESH = 8,
    OVERLAY = 9,
    NUMBER = 10,
};

/**
 * ID of texture objects
 * NUMBER is the count of useful members in this enum
 */
enum TEXTURE_ID {
    OVERLAY_TEXTURE = 0,
    NUMBER_TEXTURE = 1,
};

static const float PI = glm::pi<float>();
static const float PI2 = 2.0f * glm::pi<float>();

/**
 * Wrapping class for hypercube data management and OpenGL API interaction
 */
class Window : protected Hypercube {
private:
    static uint16_t WIDTH;
    static uint16_t HEIGHT;
    static float cameraDistance;
    static float cameraOffset4D;
    static float minCameraDistance;
    static float poleRadius;
    static double scroll_speed;
    static bool leftButtonPressed;
    static bool wire_mesh;

    glm::vec3 cameraPosition{};

    double mouse_speed = 0.005;
    static double xpos, ypos;
    double mouse_pos_x = 0.0;
    double mouse_pos_y = 0.0;
    float horizontal_angle = 0.0f;
    float vertical_angle = 0.0f;
    const float fov = PI / 4.0f;

    int8_t targetFPS = 60;
    double deltaTime = 0.0f;
    double lastFrame = 0.0f;

    GLFWwindow* window{};
    uint32_t programMain = 0, programTexture = 0;
    uint32_t VAO[VAO_ID::NUMBER]{}, VBO[VAO_ID::NUMBER]{}, NBO[VAO_ID::NUMBER]{}, IBO[VAO_ID::NUMBER]{};
    vector<vector<uint8_t>> cubesIndices;
    vector<float> points[VAO_ID::NUMBER]{};
    vector<float> vertices[VAO_ID::NUMBER]{};
    vector<float> normals[VAO_ID::NUMBER]{};
    uint32_t currentIndicesCount[VAO_ID::NUMBER]{};
    vector<uint32_t> indices[VAO_ID::NUMBER]{};

    Sponge sponge;
    uint8_t spongeDepth = 1;
    uint8_t maxSpongeDepth = 3;
    thread *spongeWorker = nullptr;
    bool spongeWorkerHasFinished = true;
    bool vertexComputationUpdated = false;

    glm::vec3 cubesColors[VAO_ID::NUMBER]{};

    static Menu menu;
    uint32_t textures[TEXTURE_ID::NUMBER_TEXTURE]{};
    vector<float> textureArrays[TEXTURE_ID::NUMBER_TEXTURE]{};

public:
    Window();
    ~Window() = default;

    /**
     * Initialize hypercube vertices, normals and indices and push them to the GPU
     */
    void createMengerSpongeLikeHypercube();

    /**
     * Render loop, compute view and projection matrix then calls drawVAOContents for each VAO
     */
    void renderMengerSpongeLikeHypercube();

    /**
     * Update the overlay image
     * @param array of size (W * H * 4)
     */
    void setOverlayArray(vector<float> &array);

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
     * Reads and compile shaders for 3D then adds them to the program
     */
    void loadMainShaders();

    /**
     * Reads and compile shaders for 2D textures then adds them to the program
     */
    void loadOverlayShaders();

    /**
     * Allocate and link array/buffers to OpenGL
     */
    void createArraysAndBuffers();

    /**
     * Project the 4D hypercube coordinates to 3D space
     */
    void projectHypercubeTo3D();

    /**
     * Link 3D points to form a cube using stored cubesIndices
     * @param ID
     */
    void create3DCube(VAO_ID ID);

    /**
     * Uses the VAOs points array to create vertices, indices and normals
     */
    void computeVertexArray();

    /**
     * Update the hypercube representation, updating the rotations if needed
     * Incrementally increase the sponge depth
     * Manage existing sponge computing thread
     */
    void update();

    /**
     * Transform the hypercube by the new rotations parameters and re-compute the individual cubes
     * Then resets depth to 1 and launch a sponge computing thread
     */
    void updateRotations();

    /**
     * Load vertices, normals and indices to buffers
     * @param ID of the VAO used to store the data
    */
    void fillSpongeVertexArray(VAO_ID ID);

    /**
     * Load vertices, normals and indices to buffers
     * @param ID of the VAO used to store the data
    */
    void fillWireMeshVertexArray();

    /**
     * Initialize overlay texture buffer and position vertices
     */
    void createOverlayTexture();

    /**
     * Sends a 4x4 matrix to a GPU uniform
     * @param name of the uniform in the shader
     * @param mat to send
     */
    static void loadUniformMat4f(uint32_t program, const char* name, const glm::mat4 &mat);

    /**
     * Sends a vec4 to a GPU uniform
     * @param name of the uniform in the shader
     * @param vec to send
     */
    static void loadUniformVec4f(uint32_t program, const char *name, const glm::vec4 &vec);

    /**
     * Sends a float to a GPU uniform
     * @param name of the uniform in the shader
     * @param vec to send
     */
    static void loadUniform1f(uint32_t program, const char *name, float value);

    /**
     * Binds the selected VAO and draw it's content
     * @param ID of the VAO to draw
     */
    void drawVAOContents(VAO_ID ID);

    /**
     * Compute distances between each cube's center of mass and the camera
     * Also save the closest and furthest cube from the origin
     * Use those to determine which cube must be drawn first
     * Then draw in order
     */
    void drawCubes();

    /**
     * Draw the hypercube wire mesh to the viewport
     */
    void drawWireMesh();

    /**
     * Draw the overlay texture to the viewport
     */
    void drawOverlay();

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
    void updateCamera();

    /**
     * Enable alpha blending for transparency
     */
    static void enableBlending();

    /**
     * Enable culling of faces pointing in the wrong direction
     */
    static void enableFaceCulling();

    /**
     * Disable culling of faces pointing in the wrong direction
     */
    static void disableFaceCulling();

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
    static void initProgram(uint32_t ID, const char *vShader, const char *fShader) ;

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
