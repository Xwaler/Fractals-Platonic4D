#include "../headers/Window.h"

using namespace std;

int Window::WIDTH = 1280;
int Window::HEIGHT = 720;
float Window::cameraDistance = 3.0f;
double Window::scroll_speed = 0.2;
bool Window::leftButtonPressed = false;
bool Window::wireframe = false;

/**
 * Initialize OpenGL, glfw and glad
 * @return 
 */
void Window::initOpenGL() {
    /* Initialize glfw library and its base parameters */
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    /* Initialize glfw window */
    window = glfwCreateWindow(WIDTH, HEIGHT, "Fractals-Platonic4D", nullptr, nullptr);
    if (window == nullptr) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        exit(-1);
    }
    /* Initialize glad utilities */
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        exit(-1);
    }

    /* Set input event callbacks */
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    /* Enable some opengl capacities */
    enableBlending();
    enableFaceCulling();
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(4);
}

bool Window::continueLoop() {
    return !glfwWindowShouldClose(window);
}

/**
 * Compile shaders and add them to the window program
 */
void Window::loadShaders() {
    program = glCreateProgram();
    initProgram(program, "../shaders/vShader.glsl", "../shaders/fShader.glsl");
}

/**
 * Initialize vertex arrays and buffers
 */
void Window::createArraysAndBuffers() {
    glGenVertexArrays(VAO_ID::NUMBER, VAO);
    glGenBuffers(VAO_ID::NUMBER, VBO);
    glGenBuffers(VAO_ID::NUMBER, NBO);
    glGenBuffers(VAO_ID::NUMBER, IBO);
}

void Window::fillVertexArray(VAO_ID ID, vector<float> &vertices, vector<float> &normals, vector<uint32_t> &indices) {
    /* Bind wanted vertex array */
    glBindVertexArray(VAO[ID]);
    /* Bind vertex buffer to vertex array */
    glBindBuffer(GL_ARRAY_BUFFER, VBO[ID]);
    /* Buffer vertices to vertex buffer */
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    /* Assign the buffer content to vertex array pointer 0 */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) nullptr);
    glEnableVertexAttribArray(0);
    /* Bind normals buffer to vertex array */
    glBindBuffer(GL_ARRAY_BUFFER, NBO[ID]);
    /* Buffer normals to normal buffer */
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
    /* Assign the buffer content to vertex array pointer 1 */
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) nullptr);
    glEnableVertexAttribArray(1);
    /* Bind indices buffer to vertex array */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO[ID]);
    /* Save indices size for latter use */
    numberIndices[ID] = indices.size();
    /* Buffer indices to vertex buffer */
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numberIndices[ID] * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
}

void Window::loadUniformMat4f(const char* name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE, glm::value_ptr(mat));
}

void Window::loadUniformVec4f(const char* name, const glm::vec4 &vec) const {
    glUniform4fv(glGetUniformLocation(program, name), 1, glm::value_ptr(vec));
}

void Window::drawScene(VAO_ID ID) {
    /* Bind the trapeze vertex array object */
    glBindVertexArray(VAO[ID]);
    glUseProgram(program);

    switch (ID) {
        case VAO_ID::CUBE: {
            // Enable depth test for solid objects
            enableDepthTest();

            // Compute model matrix for one trapeze, translate to center the object then downscale it by 2
            glm::mat4 model =
                    glm::scale(glm::mat4(1.0f), glm::vec3(0.5f)) *
                    glm::translate(glm::mat4(1), glm::vec3(-0.5));
            // Push model matrix to gpu through uniform
            loadUniformMat4f("model", model);
            // Draw vertices and create fragments with triangles
            glDrawElements(GL_TRIANGLES, numberIndices[ID], GL_UNSIGNED_INT, nullptr);
            break;
        }
        case VAO_ID::TRAPEZE: {
            // Disable depth test for transparency
            disableDepthTest();

            // Four inline trapezes (with y=0.5)
            for (uint8_t i = 0; i < 4; ++i) {
                // Compute model matrix for one trapeze, translate to center the object then rotate it to the right side
                glm::mat4 model =
                        glm::rotate(glm::mat4(1.0f), (float) i * glm::pi<float>() / 2.0f, glm::vec3(0.0f, 1.0f, 0.0f)) *
                        glm::translate(glm::mat4(1), glm::vec3(-0.5));
                // Push model matrix to gpu through uniform
                loadUniformMat4f("model", model);
                // Draw vertices and create fragments with triangles
                glDrawElements(GL_TRIANGLES, numberIndices[ID], GL_UNSIGNED_INT, nullptr);
            }
            // up (y=1.0) and down (y=0.0) trapezes
            for (int8_t i = -1; i < 2; i += 2) {
                // Compute model matrix for one trapeze, translate to center the object then rotate it to the right side
                glm::mat4 model =
                        glm::rotate(glm::mat4(1.0f), (float) i * glm::pi<float>() / 2.0f, glm::vec3(1.0f, 0.0f, 0.0f)) *
                        glm::translate(glm::mat4(1), glm::vec3(-0.5));
                // Push model matrix to gpu through uniform
                loadUniformMat4f("model", model);
                // Draw vertices and create fragments with triangles
                glDrawElements(GL_TRIANGLES, numberIndices[ID], GL_UNSIGNED_INT, nullptr);
            }
            break;
        }
        default: cout << "Unknown VAO_ID = " << ID << endl;
    }
    glBindVertexArray(0);
}

/**
 * Enable alpha blending for transparency
 */
void Window::enableBlending() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
}

/**
 * Enable depth test to allow OpenGL to draw from back to front
 */
void Window::enableDepthTest() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

/**
 * Disable depth test
 */
void Window::disableDepthTest() {
    glDisable(GL_DEPTH_TEST);
}

/**
 * Enable culling of faces pointing in the wrong direction
 */
void Window::enableFaceCulling() {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_CCW);
}

/**
 * Get the current cursor position and update the camera position accordingly
 * @param window
 * @return
 */
glm::vec3 Window::updateCamera() {
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
void Window::framebuffer_size_callback(GLFWwindow* w, int width, int height) {
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
void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
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
void Window::mouse_button_callback(GLFWwindow* w, int button, int action, int mods) {
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
void Window::scroll_callback(GLFWwindow* w, double xoffset, double yoffset) {
    cameraDistance = glm::max(0.5f, cameraDistance - (float) (yoffset * scroll_speed));
}

void Window::clear() {
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f); // reset background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear opengl buffers
}

void Window::blit() {
    /* Swap the screen buffer to reflect changes */
    glfwSwapBuffers(window);
    /* Call input event callbacks */
    glfwPollEvents();
}

void Window::close() {
    /* Deallocate vertex arrays and buffer */
    glDeleteVertexArrays(2, VAO);
    glDeleteBuffers(2, VBO);
    glDeleteBuffers(2, NBO);
    glDeleteBuffers(2, IBO);
    /* Delete compiled program */
    glDeleteProgram(program);
    /* Kill the window */
    glfwTerminate();
}
