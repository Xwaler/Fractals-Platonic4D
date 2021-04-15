#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <map>
#include <algorithm>

#include "../headers/tools.h"

using namespace std;

vector<float> computeTriangleNormalsFromQuads(const vector<float> &vertices) {
    vector<float> normals;
    for (unsigned int i = 0; i < vertices.size(); i += 12) {
        glm::vec3 a(vertices[i], vertices[i + 1], vertices[i + 2]);
        glm::vec3 b(vertices[i + 3], vertices[i + 4], vertices[i + 5]);
        glm::vec3 c(vertices[i + 6], vertices[i + 7], vertices[i + 8]);
        glm::vec3 U = b - a, V = c - a;
        glm::vec3 normal = {U.y * V.z - U.z * V.y, U.z * V.x - U.x * V.z, U.x * V.y - U.y * V.x};
        for (unsigned int j = 0; j < 4; ++j) {
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);
        }
    }
    return normals;
}

vector<unsigned int> computeTriangleIndicesFromQuads(int nVertices) {
    vector<unsigned int> indices;
    for (unsigned int i = 0; i < nVertices; i += 4) {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
        indices.push_back(i + 2);
        indices.push_back(i + 3);
        indices.push_back(i);
    }
    return indices;
}

void duplicateVertices(vector<float> &vertices, vector<uint32_t> &indices) {
    vector<float> newVertices;
    unsigned int maxIndex = *max_element(indices.begin(), indices.end());

    map<uint32_t, uint8_t> count;
    for (uint32_t &index: indices) {
        if (count.find(index) == count.end()) count[index] = 1;
        else {
            ++count[index];
            newVertices.push_back(vertices[3 * index]);
            newVertices.push_back(vertices[3 * index + 1]);
            newVertices.push_back(vertices[3 * index + 2]);
            index = ++maxIndex;
        }
    }
    vertices.insert(vertices.end(), newVertices.begin(), newVertices.end());
}

void enableBlending() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
}

void enableDepthTest() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

void disableDepthTest() {
    glDisable(GL_DEPTH_TEST);
}

void enableFaceCulling() {
    glDisable(GL_CULL_FACE);
    glCullFace(GL_CCW);
}
