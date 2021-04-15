#ifndef FRACTALS_PLATONIC4D_TOOLS_H
#define FRACTALS_PLATONIC4D_TOOLS_H

using namespace std;

vector<float> computeTriangleNormalsFromQuads(const vector<float> &vertices);

vector<unsigned int> computeTriangleIndicesFromQuads(int nVertices);

void duplicateVertices(vector<float> &vertices, vector<unsigned int> &indices);

void enableBlending();

void enableDepthTest();

void disableDepthTest();

void enableFaceCulling();

#endif //FRACTALS_PLATONIC4D_TOOLS_H
