#ifndef FRACTALS_PLATONIC4D_SPONGE_H
#define FRACTALS_PLATONIC4D_SPONGE_H

using namespace std;

static void subdivideLine(const vector<float> &line, vector<float> &result);

static void subdivideQuadrilateral(const vector<float> &quadrilateral, vector<float> &result);

void subdivide(uint8_t rank, const vector<float> &parallelepiped, vector<float> &vertices, vector<uint32_t> &indices);

void getSpongeNormals(const vector<float> &vertices, const vector<uint32_t> &indices, vector<float> &normals);

static uint64_t getNumberOfCubes(int8_t rank);

static uint64_t getNumberOfVertices(uint8_t rank);

#endif //FRACTALS_PLATONIC4D_SPONGE_H
