#ifndef FRACTALS_PLATONIC4D_SPONGE_H
#define FRACTALS_PLATONIC4D_SPONGE_H

using namespace std;

static void addLine(const vector<float> &line, vector<float> &result);

static void addSquare(const vector<float> &square, vector<float> &result);

void subdivide(uint8_t rank, const vector<float> &cube, vector<float> &vertices, vector<unsigned int> &indices);

void getSpongeNormals(const vector<float> &vertices, const vector<unsigned int> &indices, vector<float> &normals);

static uint64_t getNumberOfCubes(int8_t rank);

static uint64_t getNumberOfVertices(uint8_t rank);

#endif //FRACTALS_PLATONIC4D_SPONGE_H
