#ifndef FRACTALS_PLATONIC4D_TOOLS_H
#define FRACTALS_PLATONIC4D_TOOLS_H

#include <glm/glm.hpp>

#include <vector>

using namespace std;

vector<float> computeTriangleNormalsFromQuads(const vector<float> &vertices);

vector<unsigned int> computeTriangleIndicesFromQuads(int nVertices);

#endif //FRACTALS_PLATONIC4D_TOOLS_H
