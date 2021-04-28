#include "../headers/Window.h"

using namespace std;

uint16_t Window::WIDTH = 1280;
uint16_t Window::HEIGHT = 720;
float Window::cameraDistance = 3.0f;
double Window::scroll_speed = 0.2;
bool Window::leftButtonPressed = false;
bool Window::wireframe = false;
double Window::xpos = 0.0;
double Window::ypos = 0.0;
Menu Window::menu; //TODO: éviter cette chose, on doit pouvoir acceder à menu depuis des méthodes statiques (event handlers)

Window::Window() : sponge() {
    initOpenGL();
    loadMainShaders();
    loadOverlayShaders();
    createArraysAndBuffers();
    createOverlayTexture();
}

/**
 * Initialize hypercube points, create corresponding vertices, normals and indices and push them to the GPU
 */
void Window::createMengerSpongeLikeHypercube() {
    points[VAO_ID::CUBE] = {
            0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 1.0f,  0.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f,
    };
    fillSpongeVertexArray(VAO_ID::CUBE);
    points[VAO_ID::TRAPEZE] = {
            0.00f, 0.00f, 0.00f,  1.00f, 0.00f, 0.00f,  0.00f, 1.00f, 0.00f,  1.00f, 1.00f, 0.00f,
            0.25f, 0.25f, 0.25f,  0.75f, 0.25f, 0.25f,  0.25f, 0.75f, 0.25f,  0.75f, 0.75f, 0.25f,
    };
    fillSpongeVertexArray(VAO_ID::TRAPEZE);
    prepareBackToFrontDrawing();
}

/**
 * Render loop, compute view and projection matrix then calls drawScene for each VAO
 */
void Window::renderMengerSpongeLikeHypercube() {
    while (continueLoop()) {
        /* Draw the background and clear OpenGL render bits */
        clear();

        /* Update the camera position based on mouse movements */
        updateCamera();

        /* Update the overlay based on mouse events */
        menu.handleMouseMovement((uint16_t) (xpos / WIDTH * MenuConstants::width),
                                 (uint16_t) (ypos / HEIGHT * MenuConstants::height));

        setOverlayArray(menu.appearance);

        /* Initialize view matrix from camera and perspective projection matrix */
        glm::mat4 view = glm::lookAt(cameraPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(glm::pi<float>() / 4.0f, (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);
        /* Push view and projection matrix to the gpu through uniforms */
        loadUniformMat4f(programMain, "view", view);
        loadUniformMat4f(programMain, "projection", projection);

        /* Set and push vertices color to the gpu through uniform */
        glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        loadUniformVec4f(programMain, "color", color);
        /* Draw the scene from the cube vertex array */
        drawScene(VAO_ID::CUBE);

        /* Set and push vertices color to the gpu through uniform */
        color = glm::vec4(0.5f, 0.5f, 0.5f, 0.7f);
        loadUniformVec4f(programMain, "color", color);
        /* Draw the scene from the trapeze vertex array */
        drawScene(VAO_ID::TRAPEZE);

        /* Draw overlay over the viewport */
        drawOverlay();

        /* Swap the framebuffer to apply changes onto the screen */
        blit();

        /* Limit framerate */
        waitNextFrame();
    }
}

/**
 * Update the overlay image
 * @param array of size (W * H * 4)
 */
void Window::setOverlayArray(vector<float> &array) {
    textureArrays[TEXTURE_ID::OVERLAY_TEXTURE] = array;
}

/**
 * Initialize overlay texture buffer and position vertices
 */
void Window::createOverlayTexture() {
    glBindVertexArray(VAO[VAO_ID::OVERLAY]);
    vertices[VAO_ID::OVERLAY] = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
    };
    indices[VAO_ID::OVERLAY] = {
            0, 2, 1,
            1, 2, 3,
    };
    /* Bind vertex buffer to vertex array */
    glBindBuffer(GL_ARRAY_BUFFER, VBO[VAO_ID::OVERLAY]);
    /* Buffer vertices to vertex buffer */
    glBufferData(GL_ARRAY_BUFFER, (long) (vertices[VAO_ID::OVERLAY].size() * sizeof(float)), vertices[VAO_ID::OVERLAY].data(), GL_STATIC_DRAW);
    /* Assign the buffer content to vertex array pointer 0 */
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) nullptr);
    glEnableVertexAttribArray(0);
    /* Bind vertex buffer to vertex array */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO[VAO_ID::OVERLAY]);
    /* Buffer vertices to vertex buffer */
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (long) (indices[VAO_ID::OVERLAY].size() * sizeof(float)), indices[VAO_ID::OVERLAY].data(), GL_STATIC_DRAW);

    /* Select shader texture ID */
    glActiveTexture(GL_TEXTURE0 + TEXTURE_ID::OVERLAY_TEXTURE);
    /* Binds our texture for parametrisation */
    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_ID::OVERLAY_TEXTURE]);
    /* Set texture base parameters */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    /* Initialize texture image to empty (black) and transparent */
    textureArrays[TEXTURE_ID::OVERLAY_TEXTURE] = vector<float>(MenuConstants::width * MenuConstants::height * 4, 0.0f);
    /* Fill the texture with image to preallocate space */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, MenuConstants::width, MenuConstants::height, 0, GL_RGBA, GL_FLOAT, textureArrays[TEXTURE_ID::OVERLAY_TEXTURE].data());
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
    enableDepthTest();
    enableFaceCulling();
    glEnable(GL_TEXTURE_2D);
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
 * Reads and compile shaders for 3D then adds them to the program
 */
void Window::loadMainShaders() {
    programMain = glCreateProgram();
    initProgram(programMain, "../shaders/vShader.glsl", "../shaders/fShader.glsl");
}

/**
 * Reads and compile shaders for 2D textures then adds them to the program
 */
void Window::loadOverlayShaders() {
    programTexture = glCreateProgram();
    initProgram(programTexture, "../shaders/vOverlayShader.glsl", "../shaders/fOverlayShader.glsl");
}

/**
 * Allocate and link array/buffers to OpenGL
 */
void Window::createArraysAndBuffers() {
    glGenVertexArrays(VAO_ID::NUMBER, VAO);
    glGenBuffers(VAO_ID::NUMBER, VBO);
    glGenBuffers(VAO_ID::NUMBER, NBO);
    glGenBuffers(VAO_ID::NUMBER, IBO);
    glGenTextures(TEXTURE_ID::NUMBER_TEXTURE, textures);
}

/**
 * Uses the VAOs points array to create vertices, indices and normals. Then load them onto the gpu buffers
 * and create pointers to those memory spaces for shaders to access them
 * @param ID of the VAO used to store the data
 */
void Window::fillSpongeVertexArray(VAO_ID ID) {
    /* Generate Menger's Sponge vertices and indices */
    sponge.subdivide(3, points[ID], vertices[ID], indices[ID]);
    cout << "VAO[" << ID << "]: subdivided to " << vertices[ID].size() << " vertices and " << indices[ID].size() << " indices" << endl;
    /* Duplicate vertices used by many "sides" to allow calculation of independent vertices normals */
    Sponge::duplicateVertices(vertices[ID], indices[ID]);
    cout << "VAO[" << ID << "]: duplicated to " << vertices[ID].size() << " vertices" << endl;
    /* Compute said normals */
    Sponge::computeSpongeNormals(vertices[ID], indices[ID], normals[ID]);
    cout << "VAO[" << ID << "]: computed " << normals[ID].size() << " normals" << endl;

    /* Load vertices, normals and indices to buffers */
    /* Bind wanted vertex array */
    glBindVertexArray(VAO[ID]);
    /* Bind vertex buffer to vertex array */
    glBindBuffer(GL_ARRAY_BUFFER, VBO[ID]);
    /* Buffer vertices to vertex buffer */
    glBufferData(GL_ARRAY_BUFFER, (long) (vertices[ID].size() * sizeof(float)), vertices[ID].data(), GL_STATIC_DRAW);
    /* Assign the buffer content to vertex array pointer 0 */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) nullptr);
    glEnableVertexAttribArray(0);
    /* Bind normals buffer to vertex array */
    glBindBuffer(GL_ARRAY_BUFFER, NBO[ID]);
    /* Buffer normals to normal buffer */
    glBufferData(GL_ARRAY_BUFFER, (long) (normals[ID].size() * sizeof(float)), normals[ID].data(), GL_STATIC_DRAW);
    /* Assign the buffer content to vertex array pointer 1 */
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) nullptr);
    glEnableVertexAttribArray(1);
    /* Bind indices buffer to vertex array */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO[ID]);
    /* Buffer indices to vertex buffer */
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (long) (indices[ID].size() * sizeof(uint32_t)), indices[ID].data(), GL_STATIC_DRAW);
}

/**
 * Sends a 4x4 matrix to a GPU uniform
 * @param name of the uniform in the shader
 * @param mat to send
 */
void Window::loadUniformMat4f(uint32_t program, const char* name, const glm::mat4 &mat) {
    glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE, glm::value_ptr(mat));
}

/**
 * Sends a vec4 to a GPU uniform
 * @param name of the uniform in the shader
 * @param vec to send
 */
void Window::loadUniformVec4f(uint32_t program, const char *name, const glm::vec4 &vec) {
    glUniform4fv(glGetUniformLocation(program, name), 1, glm::value_ptr(vec));
}

/**
 * Sends a float to a GPU uniform
 * @param name of the uniform in the shader
 * @param vec to send
 */
void Window::loadUniform1f(uint32_t program, const char *name, float value) {
    glUniform1f(glGetUniformLocation(program, name), value);
}

/**
 * Initialize the faces position vector used to determine those closest to the camera
 */
void Window::prepareBackToFrontDrawing() {
    glm::vec4 center(0.5f, 0.5f, 0.125f, 1.0f);
    for (uint8_t i = 0; i < 4; ++i) {
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), (float) i * glm::pi<float>() / 2.0f, glm::vec3(0.0f, 1.0f, 0.0f)) *
                                   glm::translate(glm::mat4(1), glm::vec3(-0.5));
        transparentSidesModelMatrix.push_back(rotationMatrix);
        transparentSidesPosition.emplace_back(rotationMatrix * center);
    }
    for (int8_t i = -1; i < 2; i += 2) {
        glm::mat4 modelMatrix = glm::rotate(glm::mat4(1.0f), (float) i * glm::pi<float>() / 2.0f, glm::vec3(1.0f, 0.0f, 0.0f)) *
                                glm::translate(glm::mat4(1), glm::vec3(-0.5));
        transparentSidesModelMatrix.push_back(modelMatrix);
        transparentSidesPosition.emplace_back(modelMatrix * center);
    }
}

/**
 * Binds the selected VAO and draw it's content
 * @param ID of the VAO to draw
 */
void Window::drawScene(VAO_ID ID) {
    /* Bind the trapeze vertex array object */
    glBindVertexArray(VAO[ID]);
    glUseProgram(programMain);

    switch (ID) {
        case VAO_ID::CUBE: {
            // Compute model matrix for one trapeze, translate to center the object then downscale it by 2
            glm::mat4 model =
                    glm::scale(glm::mat4(1.0f), glm::vec3(0.5f)) *
                    glm::translate(glm::mat4(1), glm::vec3(-0.5));
            // Push model matrix to gpu through uniform
            loadUniformMat4f(programMain, "model", model);
            // Draw vertices and create fragments with triangles
            glDrawElements(GL_TRIANGLES, (int32_t) (indices[ID].size()), GL_UNSIGNED_INT, nullptr);
            break;
        }
        case VAO_ID::TRAPEZE: {
            /* Compute distances between each side and the camera */
            vector<double> distance; vector<uint32_t> indexSortedDistance;
            for (glm::vec3 sidePosition: transparentSidesPosition) {
                distance.push_back(glm::length(cameraPosition - sidePosition));
            }
            /* Initialize indices for synchronised sorting */
            for (uint32_t i = 0; i < distance.size(); ++i) {
                indexSortedDistance.push_back(i);
            }
            /* Sorts distances and indices together so that we can access our model matrices by index */
            for (uint32_t i = 0; i < distance.size(); ++i) {
                for (uint32_t j = i; j < distance.size(); ++j) {
                    if (distance[i] < distance[j]) {
                        double tmpDistance = distance[i]; uint32_t tmpIndex = indexSortedDistance[i];
                        distance[i] = distance[j];
                        distance[j] = tmpDistance;
                        indexSortedDistance[i] = indexSortedDistance[j];
                        indexSortedDistance[j] = tmpIndex;
                    }
                }
            }
            /* Draw the faces from back to front, following the sorted indices/distances */
            for (uint32_t index: indexSortedDistance) {
                // Push model matrix to gpu through uniform
                loadUniformMat4f(programMain, "model", transparentSidesModelMatrix[index]);
                // Draw vertices and create fragments with triangles
                glDrawElements(GL_TRIANGLES, (int32_t) indices[ID].size(), GL_UNSIGNED_INT, nullptr);
            }
            break;
        }
        default: cout << "Unknown VAO_ID = " << ID << endl;
    }
}

/**
 * Draw the overlay texture to the viewport
 */
void Window::drawOverlay() {
    /* Bind overlay vertex array and load overlay shader program */
    glBindVertexArray(VAO[VAO_ID::OVERLAY]);
    glUseProgram(programTexture);

    /* Activate our texture ID and fill it with the image data */
    glActiveTexture(GL_TEXTURE0 + TEXTURE_ID::OVERLAY_TEXTURE);
    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_ID::OVERLAY_TEXTURE]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, MenuConstants::width, MenuConstants::height, 0, GL_RGBA, GL_FLOAT, textureArrays[TEXTURE_ID::OVERLAY_TEXTURE].data());

    /* Setup orthonormal projection matrix */
    glm::mat4 projection = glm::ortho(0, 1, 1, 0, -1, 1);
    loadUniformMat4f(programTexture, "projection", projection);
    /* Bind our texture ID to the shader */
    loadUniform1f(programTexture, "overlayTexture", TEXTURE_ID::OVERLAY_TEXTURE);
    glDrawElements(GL_TRIANGLES, (int32_t) indices[VAO_ID::OVERLAY].size(), GL_UNSIGNED_INT, nullptr);

    glUseProgram(programMain);
}

/**
 * Enable alpha blending for transparency
 */
void Window::enableBlending() {
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
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
 * Disable culling of faces pointing in the wrong direction
 */
void Window::disableFaceCulling() {
    glDisable(GL_CULL_FACE);
}

/**
 * Reads mouse input and return updated position vector for the camera
 * @return
 */
void Window::updateCamera() {
    // get elapsed time since last frame
    double currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // get new cursor position
    glfwGetCursorPos(window, &xpos, &ypos);
    if (leftButtonPressed && !menu.isInputCaptured) { // if the user is pressing the mouse left button, update the camera angle
        horizontal_angle += (float) (deltaTime * mouse_speed * (mouse_pos_x - xpos));
        vertical_angle += (float) (deltaTime * mouse_speed * (ypos - mouse_pos_y));
    }
    mouse_pos_x = xpos;
    mouse_pos_y = ypos;
    // use magic rotation trigonometry to compute the new camera position centered around the origin
    cameraPosition = cameraDistance * glm::vec3(
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
void Window::initProgram(uint32_t ID, const char *vShader, const char *fShader) {
    std::string vertexCode = readShaderFile(vShader);
    const char* vShaderCode = vertexCode.c_str();

    std::string fragmentCode = readShaderFile(fShader);
    const char* fShaderCode = fragmentCode.c_str();

    uint32_t vertex = glCreateShader(GL_VERTEX_SHADER), fragment = glCreateShader(GL_FRAGMENT_SHADER);
    createShader(vertex, vShaderCode, "VERTEX");
    createShader(fragment, fShaderCode, "FRAGMENT");

    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
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
        glfwGetCursorPos(w, &xpos, &ypos);
        menu.handleKeyPress(action,
                            (uint16_t) (xpos / WIDTH * MenuConstants::width),
                            (uint16_t) (ypos / HEIGHT * MenuConstants::height));
        if (!menu.isInputCaptured) {
            leftButtonPressed = true;
        }
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        glfwGetCursorPos(w, &xpos, &ypos);
        menu.handleKeyPress(action,
                            (uint16_t) (xpos / WIDTH * MenuConstants::width),
                            (uint16_t) (ypos / HEIGHT * MenuConstants::height));
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
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f); // reset background color
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
    glDeleteVertexArrays(VAO_ID::NUMBER, VAO);
    glDeleteBuffers(VAO_ID::NUMBER, VBO);
    glDeleteBuffers(VAO_ID::NUMBER, NBO);
    glDeleteBuffers(VAO_ID::NUMBER, IBO);
    /* Deallocate textures */
    glDeleteTextures(TEXTURE_ID::NUMBER_TEXTURE, textures);
    /* Delete compiled program */
    glDeleteProgram(programMain);
    glDeleteProgram(programTexture);
    /* Kill the window */
    glfwTerminate();
}
