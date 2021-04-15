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
