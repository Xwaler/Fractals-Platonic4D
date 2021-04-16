#include "../headers/Window.h"

using namespace std;

int Window::WIDTH = 1280;
int Window::HEIGHT = 720;
float Window::cameraDistance = 3.0f;
double Window::scroll_speed = 0.2;
bool Window::leftButtonPressed = false;
bool Window::wireframe = false;

Window::Window() : sponge(){
    initOpenGL();
    loadShaders();
    createArraysAndBuffers();
}

/**
 * Initialize hypercube points, create corresponding vertices, normals and indices and push them to the GPU
 */
void Window::createMengerSpongeLikeHypercube() {
    points[VAO_ID::CUBE] = {
            0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 1.0f,  0.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f,
    };
    fillVertexArray(VAO_ID::CUBE);
    points[VAO_ID::TRAPEZE] = {
            0.00f, 0.00f, 0.00f,  1.00f, 0.00f, 0.00f,  0.00f, 1.00f, 0.00f,  1.00f, 1.00f, 0.00f,
            0.25f, 0.25f, 0.25f,  0.75f, 0.25f, 0.25f,  0.25f, 0.75f, 0.25f,  0.75f, 0.75f, 0.25f,
    };
    fillVertexArray(VAO_ID::TRAPEZE);
}

/**
 * Render loop, compute view and projection matrix then calls drawScene for each VAO
 */
void Window::renderMengerSpongeLikeHypercube() {
    while (continueLoop()) {
        /* Draw the background and clear OpenGL render bits */
        clear();

        /* Update the camera position based on mouse movements */
        glm::vec3 cameraPosition = updateCamera();

        /* Initialize view matrix from camera and perspective projection matrix */
        glm::mat4 view = glm::lookAt(cameraPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(glm::pi<float>() / 4.0f, (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);
        /* Push view and projection matrix to the gpu through uniforms */
        loadUniformMat4f("view", view);
        loadUniformMat4f("projection", projection);

        /* Set and push vertices color to the gpu through uniform */
        glm::vec4 color = glm::vec4(0.5f, 0.5f, 0.5f, 0.8f);
        loadUniformVec4f("color", color);
        /* Draw the scene from the trapeze vertex array */
        drawScene(VAO_ID::TRAPEZE);

        /* Set and push vertices color to the gpu through uniform */
        color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        loadUniformVec4f("color", color);
        /* Draw the scene from the cube vertex array */
        drawScene(VAO_ID::CUBE);

        /* Swap the framebuffer to apply changes onto the screen */
        blit();

        /* Limit framerate */
        waitNextFrame();
    }
}

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

/**
 * Gives window status
 * @return true if window must stay open, false otherwise
 */
bool Window::continueLoop() {
    return !glfwWindowShouldClose(window);
}

/**
 * Reads and compile shaders then adds them to the program
 */
void Window::loadShaders() {
    program = glCreateProgram();
    initProgram("../shaders/vShader.glsl", "../shaders/fShader.glsl");
}

/**
 * Allocate and link array/buffers to OpenGL
 */
void Window::createArraysAndBuffers() {
    glGenVertexArrays(VAO_ID::NUMBER, VAO);
    glGenBuffers(VAO_ID::NUMBER, VBO);
    glGenBuffers(VAO_ID::NUMBER, NBO);
    glGenBuffers(VAO_ID::NUMBER, IBO);
}

/**
 * Uses the VAOs points array to create vertices, indices and normals. Then load them onto the gpu buffers
 * and create pointers to those memory spaces for shaders to access them
 * @param ID of the VAO used to store the data
 */
void Window::fillVertexArray(VAO_ID ID) {
    /* Generate Menger's Sponge vertices and indices */
    sponge.subdivide(3, points[ID], vertices[ID], indices[ID]);
    cout << "VAO[" << ID << "]: subdivided to " << vertices[ID].size() << " vertices and " << indices[ID].size() << " indices" << endl;
    /* Duplicate vertices used by many "sides" to allow calculation of independent vertices normals */
    Sponge::duplicateVertices(vertices[ID], indices[ID]);
    cout << "VAO[" << ID << "]: duplicated to " << vertices[ID].size() << " vertices" << endl;
    /* Compute said normals */
    Sponge::computeSpongeNormals(vertices[ID], indices[ID], normals[ID]);

    /* Load vertices, normals and indices to buffers */
    /* Bind wanted vertex array */
    glBindVertexArray(VAO[ID]);
    /* Bind vertex buffer to vertex array */
    glBindBuffer(GL_ARRAY_BUFFER, VBO[ID]);
    /* Buffer vertices to vertex buffer */
    glBufferData(GL_ARRAY_BUFFER, vertices[ID].size() * sizeof(float), vertices[ID].data(), GL_STATIC_DRAW);
    /* Assign the buffer content to vertex array pointer 0 */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) nullptr);
    glEnableVertexAttribArray(0);
    /* Bind normals buffer to vertex array */
    glBindBuffer(GL_ARRAY_BUFFER, NBO[ID]);
    /* Buffer normals to normal buffer */
    glBufferData(GL_ARRAY_BUFFER, normals[ID].size() * sizeof(float), normals[ID].data(), GL_STATIC_DRAW);
    /* Assign the buffer content to vertex array pointer 1 */
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) nullptr);
    glEnableVertexAttribArray(1);
    /* Bind indices buffer to vertex array */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO[ID]);
    /* Buffer indices to vertex buffer */
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[ID].size() * sizeof(uint32_t), indices[ID].data(), GL_STATIC_DRAW);
}

/**
 * Sends a 4x4 matrix to a GPU uniform
 * @param name of the uniform in the shader
 * @param mat to send
 */
void Window::loadUniformMat4f(const char* name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE, glm::value_ptr(mat));
}

/**
 * Sends a vec4 to a GPU uniform
 * @param name of the uniform in the shader
 * @param vec to send
 */
void Window::loadUniformVec4f(const char* name, const glm::vec4 &vec) const {
    glUniform4fv(glGetUniformLocation(program, name), 1, glm::value_ptr(vec));
}

/**
 * Binds the selected VAO and draw it's content
 * @param ID of the VAO to draw
 */
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
            glDrawElements(GL_TRIANGLES, indices[ID].size(), GL_UNSIGNED_INT, nullptr);
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
                glDrawElements(GL_TRIANGLES, indices[ID].size(), GL_UNSIGNED_INT, nullptr);
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
                glDrawElements(GL_TRIANGLES, indices[ID].size(), GL_UNSIGNED_INT, nullptr);
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
 * Enables OpenGL depth test
 */
void Window::enableDepthTest() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

/**
 * Disable OpenGL depth test
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
 * Reads mouse input and return updated position vector for the camera
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
 * Reads a shader file and return a string of it's content
 * @param file path
 * @return string
 */
string Window::readShaderFile(const char* file) {
    std::string vertexCode;
    std::ifstream vShaderFile;
    std::stringstream vShaderStream;

    vShaderFile.open(file);
    vShaderStream << vShaderFile.rdbuf();
    vShaderFile.close();
    return vShaderStream.str();
}

/**
 * Checks a component for compilation errors
 * @param shader ID
 * @param type of the compiled component (VERTEX, FRAGMENT, PROGRAM)
 */
void Window::checkCompileErrors(uint32_t shader, const string& type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

/**
 * Compiles a shader from it's code
 * @param ID of the shader
 * @param code content as string
 * @param type of shader (VERTEX, FRAGMENT)
 */
void Window::createShader(uint32_t shader, const char* &code, const char* type) {
    glShaderSource(shader, 1, &code, nullptr);
    glCompileShader(shader);
    checkCompileErrors(shader, type);
}

/**
 * Compiles shaders and add the to the program
 * @param vShader file path
 * @param fShader file path
 */
void Window::initProgram(const char* vShader, const char* fShader) const {
    std::string vertexCode = readShaderFile(vShader);
    const char* vShaderCode = vertexCode.c_str();

    std::string fragmentCode = readShaderFile(fShader);
    const char* fShaderCode = fragmentCode.c_str();

    uint32_t vertex = glCreateShader(GL_VERTEX_SHADER), fragment = glCreateShader(GL_FRAGMENT_SHADER);
    createShader(vertex, vShaderCode, "VERTEX");
    createShader(fragment, fShaderCode, "FRAGMENT");

    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    checkCompileErrors(program, "PROGRAM");
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

/**
 * Called when the window is resized, update the viewport accordingly
 * @param window unused
 * @param new width of the window
 * @param new height of the window
 */
void Window::framebuffer_size_callback(GLFWwindow* w, int width, int height) {
    WIDTH = width;
    HEIGHT = height;
    glViewport(0, 0, WIDTH, HEIGHT);
}

/**
 * Called when the user presses a key
 * @param window unused
 * @param key pressed
 * @param scancode unused
 * @param action type
 * @param mods unused
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
 * Called when the user presses a mouse button
 * @param window unused
 * @param button pressed
 * @param action type
 * @param mods unused
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
 * Called when the user scrolls
 * @param window unused
 * @param xoffset is the rotation difference of the movement in the X axis
 * @param yoffset is the rotation difference of the movement in the Y axis
 */
void Window::scroll_callback(GLFWwindow* w, double xoffset, double yoffset) {
    cameraDistance = glm::max(0.5f, cameraDistance - (float) (yoffset * scroll_speed));
}

/**
 * Reset background and buffer bit
 */
void Window::clear() {
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f); // reset background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear opengl buffers
}

/**
 * Update the screen to reflect changes
 */
void Window::blit() {
    /* Swap the screen buffer to reflect changes */
    glfwSwapBuffers(window);
    /* Call input event callbacks */
    glfwPollEvents();
}

/**
 * Sleeps to limit framerate
 */
void Window::waitNextFrame() const {
    long toWait = (long) (1000000000.0 * ((1.0 / (double) targetFPS) - deltaTime));
    if (toWait > 0L) {
        const struct timespec time = (timespec){0, toWait};
        nanosleep(&time, nullptr);
    }
}

/**
 * Clear allocated buffers and closes the window
 */
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
