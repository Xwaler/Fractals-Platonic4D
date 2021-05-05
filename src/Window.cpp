#include "../headers/Window.h"

using namespace std;

uint16_t Window::WIDTH = 1280;
uint16_t Window::HEIGHT = 720;
float Window::cameraDistance = 3.0f;
float Window::cameraOffset4D = 3.0f;
float Window::minCameraDistance = 0.1f;
double Window::scroll_speed = 0.2f;
bool Window::leftButtonPressed = false;
bool Window::wire_mesh = false;
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
 * Project the 4D hypercube coordinates to 3D space
 */
void Window::projectHypercubeTo3D() {
    points[VAO_ID::WIRE_MESH].clear();
    for (glm::vec4 &V: hypercubePoints) {
        glm::vec3 Vp = glm::vec3(V) / (cameraOffset4D - V.w);
        points[VAO_ID::WIRE_MESH].push_back(Vp.x);
        points[VAO_ID::WIRE_MESH].push_back(Vp.y);
        points[VAO_ID::WIRE_MESH].push_back(Vp.z);
    }
}

/**
 * Link 3D points to form a cube using stored cubesIndices
 * @param ID
 */
void Window::create3DCube(VAO_ID ID) {
    points[ID].clear();
    for (uint8_t index: cubesIndices[ID]) {
        float *p = &points[VAO_ID::WIRE_MESH][index * 3];
        points[ID].push_back(*(p + 0));
        points[ID].push_back(*(p + 1));
        points[ID].push_back(*(p + 2));
    }
}

/**
 * Initialize hypercube vertices, normals and indices and push them to the GPU
 */
void Window::createMengerSpongeLikeHypercube() {
    cubesIndices = {
            { 1, 3, 5, 7, 9, 11, 13, 15 },
            { 2, 0, 6, 4, 10, 8, 14, 12 },
            { 4, 5, 6, 7, 12, 13, 14, 15 },
            { 1, 0, 3, 2, 9, 8, 11, 10 },
            { 6, 7, 2, 3, 14, 15, 10, 11 },
            { 5, 4, 1, 0, 13, 12, 9, 8 },
            { 8, 9, 10, 11, 12, 13, 14, 15 },
            { 0, 1, 2, 3, 4, 5, 6, 7 },
    };
    cubesColors[VAO_ID::PX] = glm::vec3(235, 158, 160) / 255.0f;
    cubesColors[VAO_ID::NX] = glm::vec3(36, 106, 222) / 255.0f;
    cubesColors[VAO_ID::PY] = glm::vec3(124, 41, 141) / 255.0f;
    cubesColors[VAO_ID::NY] = glm::vec3(247, 246, 76) / 255.0f;
    cubesColors[VAO_ID::PZ] = glm::vec3(227, 222, 213) / 255.0f;
    cubesColors[VAO_ID::NZ] = glm::vec3(254, 165, 57) / 255.0f;
    cubesColors[VAO_ID::PW] = glm::vec3(204, 101, 42) / 255.0f;
    cubesColors[VAO_ID::NW] = glm::vec3(106, 255, 188) / 255.0f;
    indices[VAO_ID::WIRE_MESH] = {
            0, 1, 1, 3, 3, 2, 2, 0,         // NX
            4, 5, 5, 7, 7, 6, 6, 4,         // PX

            8, 9, 9, 11, 11, 10, 10, 8,     // NW
            12, 13, 13, 15, 15, 14, 14, 12, // PW

            0, 8, 1, 9, 2, 10, 3, 11,       // links NW
            4, 12, 5, 13, 6, 14, 7, 15,     // links PW

            0, 4, 1, 5, 2, 6, 3, 7,         // links NY
            8, 12, 9, 13, 10, 14, 11, 15,   // links PY
    };

    /* Project from 4D to 3D */
    projectHypercubeTo3D();
    /* Re-create cubes and sponge vertices */
    for (uint8_t i = 0; i < 8; ++i) {
        create3DCube((VAO_ID) i);
    }
    spongeWorker = new thread(&Window::computeVertexArray, this);
}

/**
 * Render loop, compute view and projection matrix then calls drawVAOContents for each VAO
 */
void Window::renderMengerSpongeLikeHypercube() {
    while (continueLoop()) {
        /* Draw the background and clear OpenGL render bits */
        clear();

        /* Update the camera position based on mouse movements */
        updateCamera();

        /* Update the overlay based on mouse events */
        menu.handleMouseMovement((uint16_t) (xpos / WIDTH * MenuProperties::width),
                                 (uint16_t) (ypos / HEIGHT * MenuProperties::height));

        setOverlayArray(menu.getAppearance());

        /* Initialize view matrix from camera and perspective projection matrix */
        glm::mat4 view = glm::lookAt(cameraPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(fov, (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);
        /* Push view and projection matrix to the gpu through uniforms */
        loadUniformMat4f(programMain, "view", view);
        loadUniformMat4f(programMain, "projection", projection);

        /* Update hypercube rotations if the user changed them */
        updateRotations();

        if (wire_mesh) {
            /* Draw projected hypercube wire mesh */
            drawWireMesh();
        } else {
            /* Draw the cubes in back to front order */
            drawCubes();
        }

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
 * Uses the VAOs points array to create vertices, indices and normals
 * @param ID of the VAO used to store the data
 */
void Window::computeVertexArray() {
    try {
        for (uint8_t ID = 0; ID < 8; ++ID) {
            vertices[ID].clear();
            indices[ID].clear();
            normals[ID].clear();
            /* Generate Menger's Sponge vertices and indices */
            sponge.subdivide(spongeDepth, points[ID], vertices[ID], indices[ID]);
            cout << "VAO[" << (VAO_ID) ID << "]: subdivided to " << vertices[ID].size() << " vertices and " << indices[ID].size() << " indices" << endl;
            /* Duplicate vertices used by many "sides" to allow calculation of independent vertices normals */
            Sponge::duplicateVertices(vertices[ID], indices[ID]);
            cout << "VAO[" << (VAO_ID) ID << "]: duplicated to " << vertices[ID].size() << " vertices" << endl;
            /* Compute said normals */
            Sponge::computeSpongeNormals(vertices[ID], indices[ID], normals[ID]);
            cout << "VAO[" << (VAO_ID) ID << "]: computed " << normals[ID].size() << " normals" << endl;
        }
        vertexComputationUpdated = true;
        spongeWorkerHasFinished = true;
    } catch (const std::exception& e) {
        cout << e.what() << endl;
        return;
    }
}

void Window::updateRotations() {
    if (spongeWorker != nullptr && spongeWorkerHasFinished) {
        spongeWorker->join();
        spongeWorker = nullptr;
    }

    if (spongeWorker == nullptr && vertexComputationUpdated) {
        if (wire_mesh) {
            fillWireMeshVertexArray();
        } else {
            for (uint8_t ID = 0; ID < 8; ++ID) {
                fillSpongeVertexArray((VAO_ID) ID);
            }
        }
        vertexComputationUpdated = false;
    }

    /* If the user changed the rotation parameters, re-compute hypercube and sponge vertices */
    if (menu.rotationWasModified) {
        menu.rotationWasModified = false;
        /* Reset transformations */
        init4DRotations();
        /* Apply all 4D rotations */
        if (menu.getGaugeValue(Gauges::ROTATION_XY) != 0) {
            rotateXY(menu.getGaugeValue(Gauges::ROTATION_XY) * PI2);
        }
        if (menu.getGaugeValue(Gauges::ROTATION_YZ) != 0) {
            rotateYZ(menu.getGaugeValue(Gauges::ROTATION_YZ) * PI2);
        }
        if (menu.getGaugeValue(Gauges::ROTATION_ZX) != 0) {
            rotateZX(menu.getGaugeValue(Gauges::ROTATION_ZX) * PI2);
        }
        if (menu.getGaugeValue(Gauges::ROTATION_XW) != 0) {
            rotateXW(menu.getGaugeValue(Gauges::ROTATION_XW) * PI2);
        }
        if (menu.getGaugeValue(Gauges::ROTATION_YW) != 0) {
            rotateYW(menu.getGaugeValue(Gauges::ROTATION_YW) * PI2);
        }
        if (menu.getGaugeValue(Gauges::ROTATION_ZW) != 0) {
            rotateZW(menu.getGaugeValue(Gauges::ROTATION_ZW) * PI2);
        }
        /* Project from 4D to 3D */
        projectHypercubeTo3D();
        /* Re-create cubes */
        for (uint8_t i = 0; i < 8; ++i) {
            create3DCube((VAO_ID) i);
        }

        spongeDepth = 1;
        if (spongeWorker != nullptr) {
            Sponge::killComputation = true;
            spongeWorker->join();
        }
        Sponge::killComputation = false;
        spongeWorkerHasFinished = false;
        spongeWorker = new thread(&Window::computeVertexArray, this);
    }

    if (spongeWorker == nullptr && spongeDepth != maxSpongeDepth && !wire_mesh) {
        spongeDepth = min((uint8_t) (spongeDepth + 1), maxSpongeDepth);
        Sponge::killComputation = false;
        spongeWorkerHasFinished = false;
        spongeWorker = new thread(&Window::computeVertexArray, this);
    }
}

/**
 * Load vertices, normals and indices to buffers
 * @param ID of the VAO used to store the data
 */
void Window::fillSpongeVertexArray(VAO_ID ID) {
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
    currentIndicesCount[ID] = indices[ID].size();
}

void Window::fillWireMeshVertexArray() {
    /* Bind wanted vertex array */
    glBindVertexArray(VAO[VAO_ID::WIRE_MESH]);
    /* Bind vertex buffer to vertex array */
    glBindBuffer(GL_ARRAY_BUFFER, VBO[VAO_ID::WIRE_MESH]);
    /* Buffer vertices to vertex buffer */
    glBufferData(GL_ARRAY_BUFFER, (long) (points[VAO_ID::WIRE_MESH].size() * sizeof(float)), points[VAO_ID::WIRE_MESH].data(), GL_STATIC_DRAW);
    /* Assign the buffer content to vertex array pointer 0 */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) nullptr);
    glEnableVertexAttribArray(0);
    /* Bind indices buffer to vertex array */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO[VAO_ID::WIRE_MESH]);
    /* Buffer indices to vertex buffer */
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (long) (indices[VAO_ID::WIRE_MESH].size() * sizeof(uint32_t)), indices[VAO_ID::WIRE_MESH].data(), GL_STATIC_DRAW);
    currentIndicesCount[VAO_ID::WIRE_MESH] = indices[VAO_ID::WIRE_MESH].size();
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
    currentIndicesCount[VAO_ID::OVERLAY] = indices[VAO_ID::OVERLAY].size();

    /* Select shader texture ID */
    glActiveTexture(GL_TEXTURE0 + TEXTURE_ID::OVERLAY_TEXTURE);
    /* Binds our texture for parametrisation */
    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_ID::OVERLAY_TEXTURE]);
    /* Set texture base parameters */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    /* Preallocate texture space */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, MenuProperties::width, MenuProperties::height, 0, GL_RGBA, GL_FLOAT, nullptr);
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
 * Binds the selected VAO and draw it's content
 * @param ID of the VAO to draw
 */
void Window::drawVAOContents(VAO_ID ID) {
    /* Bind vertex array object */
    glBindVertexArray(VAO[ID]);
    glUseProgram(programMain);

    /* Model matrix */
    glm::mat4 model = glm::mat4(1.0f);
    /* Push model matrix to gpu through uniform */
    loadUniformMat4f(programMain, "model", model);
    /* Draw vertices and create fragments with triangles */
    glDrawElements(GL_TRIANGLES, (int32_t) currentIndicesCount[ID], GL_UNSIGNED_INT, nullptr);
}

/**
 * Compute distances between each cube's center of mass and the camera
 * Also save the closest and furthest cube from the origin
 * Use those to determine which cube must be drawn first
 * Then draw in order
 */
void Window::drawCubes() {
    glm::vec4 color;
    vector<double> distances;
    vector<uint32_t> indexSortedDistance;
    uint32_t inner; uint32_t outer;
    float innerNorm = 1e9f; float outerNorm = 0.0f;

    for (uint8_t i = 0; i < 8; ++i) {
        glm::vec3 centerOfMass(0.0f); glm::vec3 summedCoords(0.0f);
        glm::vec3 pos;
        for (uint16_t j = 0; j < (uint16_t) points[(VAO_ID) i].size(); j += 3) {
            float *p = &(points[(VAO_ID) i][j]);
            pos = glm::vec3(*(p + 0), *(p + 1), *(p + 2));
            centerOfMass += pos;
            summedCoords += glm::abs(pos);
        }
        distances.push_back(glm::length(cameraPosition - centerOfMass / (float) points[(VAO_ID) i].size()));

        float norm = glm::length(summedCoords);
        if (norm < innerNorm) {
            innerNorm = norm;
            inner = i;
        }
        if (norm > outerNorm) {
            outerNorm = norm;
            outer = i;
        }
    }
    /* Initialize indices for synchronised sorting */
    for (uint32_t i = 0; i < distances.size(); ++i) {
        indexSortedDistance.push_back(i);
    }
    /* Sorts distances and indices together so that we can access our model matrices by index */
    for (uint32_t i = 0; i < distances.size(); ++i) {
        for (uint32_t j = i; j < distances.size(); ++j) {
            if (distances[i] < distances[j]) {
                double tmpDistance = distances[i]; uint32_t tmpIndex = indexSortedDistance[i];
                distances[i] = distances[j];
                distances[j] = tmpDistance;
                indexSortedDistance[i] = indexSortedDistance[j];
                indexSortedDistance[j] = tmpIndex;
            }
        }
    }

    uint32_t drawn = 0; uint32_t i = 0;
    /* Draw the first half faces in from back to front, following the sorted indices/distances
     * and ignoring the inner most and outer most cubes */
    do {
        uint8_t index = indexSortedDistance[i++];
        if (index == inner || index == outer) continue;

        /* Set and push vertices color to the gpu through uniform */
        color = glm::vec4(cubesColors[(VAO_ID) index], menu.getGaugeValue((Gauges) index));
        loadUniformVec4f(programMain, "color", color);
        /* Draw from the vertex array */
        drawVAOContents((VAO_ID) index);
        drawn++;
    } while (drawn < 3);

    /* Draw the inner most cube */
    /* Set and push vertices color to the gpu through uniform */
    color = glm::vec4(cubesColors[inner], menu.getGaugeValue((Gauges) inner));
    loadUniformVec4f(programMain, "color", color);
    /* Draw from the vertex array */
    drawVAOContents((VAO_ID) inner);
    drawn++;

    /* Draw the second half faces in from back to front, following the sorted indices/distances
     * and ignoring the inner most and outer most cubes */
    do {
        uint8_t index = indexSortedDistance[i++];
        if (index == inner || index == outer) continue;

        /* Set and push vertices color to the gpu through uniform */
        color = glm::vec4(cubesColors[(VAO_ID) index], menu.getGaugeValue((Gauges) index));
        loadUniformVec4f(programMain, "color", color);
        /* Draw from the vertex array */
        drawVAOContents((VAO_ID) index);
        drawn++;
    } while (drawn < 7);

    /* Draw the outer most cube */
    /* Set and push vertices color to the gpu through uniform */
    color = glm::vec4(cubesColors[outer], menu.getGaugeValue((Gauges) outer));
    loadUniformVec4f(programMain, "color", color);
    /* Draw from the vertex array */
    drawVAOContents((VAO_ID) outer);
}

void Window::drawWireMesh() {
    glBindVertexArray(VAO[VAO_ID::WIRE_MESH]);
    glUseProgram(programMain);

    /* Matte black color */
    glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    loadUniformVec4f(programMain, "color", color);
    /* Model matrix */
    glm::mat4 model = glm::mat4(1.0f);
    /* Push model matrix to gpu through uniform */
    loadUniformMat4f(programMain, "model", model);
    /* Draw vertices and create fragments with triangles */
    glDrawElements(GL_LINES, (int32_t) currentIndicesCount[VAO_ID::WIRE_MESH], GL_UNSIGNED_INT, nullptr);
}

/**
 * Draw the overlay texture to the viewport
 */
void Window::drawOverlay() {
    /* Bind overlay vertex array and load overlay shader program */
    glBindVertexArray(VAO[VAO_ID::OVERLAY]);
    glUseProgram(programTexture);
    /* Disable depth test to ensure the overlay to be on top of everything */
    disableDepthTest();

    /* Activate our texture ID and fill it with the image data */
    glActiveTexture(GL_TEXTURE0 + TEXTURE_ID::OVERLAY_TEXTURE);
    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_ID::OVERLAY_TEXTURE]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, MenuProperties::width, MenuProperties::height, 0, GL_RGBA, GL_FLOAT, textureArrays[TEXTURE_ID::OVERLAY_TEXTURE].data());

    /* Setup orthonormal projection matrix */
    glm::mat4 projection = glm::ortho(0.0f, 1.0f, 1.0f, 0.0f, -0.1f, 0.1f);
    loadUniformMat4f(programTexture, "projection", projection);
    /* Bind our texture ID to the shader */
    loadUniform1f(programTexture, "overlayTexture", TEXTURE_ID::OVERLAY_TEXTURE);
    glDrawElements(GL_TRIANGLES, (int32_t) currentIndicesCount[VAO_ID::OVERLAY], GL_UNSIGNED_INT, nullptr);

    /* Re-enable for following draws */
    enableDepthTest();
    glUseProgram(programMain);
}

/**
 * Enable alpha blending for transparency
 */
void Window::enableBlending() {
    glEnable(GL_BLEND);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
}

/**
 * Enables OpenGL depth test
 */
void Window::enableDepthTest() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
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
    if (leftButtonPressed && !menu.isInputCaptured()) { // if the user is pressing the mouse left button, update the camera angle
        horizontal_angle += (float) (mouse_speed * (mouse_pos_x - xpos));
        vertical_angle += (float) (mouse_speed * (ypos - mouse_pos_y));
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
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) { // toggle wire_mesh display mode
        wire_mesh = !wire_mesh;
        menu.rotationWasModified = true;
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
                            (uint16_t) (xpos / WIDTH * MenuProperties::width),
                            (uint16_t) (ypos / HEIGHT * MenuProperties::height));
        if (!menu.isInputCaptured()) {
            leftButtonPressed = true;
        }
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        glfwGetCursorPos(w, &xpos, &ypos);
        menu.handleKeyPress(action,
                            (uint16_t) (xpos / WIDTH * MenuProperties::width),
                            (uint16_t) (ypos / HEIGHT * MenuProperties::height));
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
    cameraDistance = glm::max(minCameraDistance, cameraDistance - (float) (yoffset * scroll_speed));
}

/**
 * Reset background and buffer bit
 */
void Window::clear() {
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f); // reset background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // clear opengl buffers
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
