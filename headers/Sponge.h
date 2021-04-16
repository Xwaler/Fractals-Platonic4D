#ifndef FRACTALS_PLATONIC4D_SPONGE_H
#define FRACTALS_PLATONIC4D_SPONGE_H

#include <glm/glm.hpp>

#include <cstdint>
#include <cmath>
#include <vector>
#include <map>
#include <algorithm>

#include "vectorTools.h"

using namespace std;

class Sponge {
private:
    std::vector<uint8_t> frontFaceIndices;
    std::vector<uint8_t> topFaceIndices;
    std::vector<uint8_t> rightFaceIndices;
    std::vector<uint8_t> bottomFaceIndices;
    std::vector<uint8_t> leftFaceIndices;
    std::vector<uint8_t> backFaceIndices;
    std::vector<std::vector<uint8_t> *> faceIndicesList;
    std::vector<uint8_t> innerParts;

public:
    Sponge();

    /**
    * Subdivide the given parallelepiped in a Menger Sponge like pattern, stopping at the given depth.
    * Vertices will be created, then face will be added using those new vertices.
    * @param depth is the depth when to stop subdivision. 0 is a basic cube
    * @param parallelepiped is a vector of eights points given in the following order :
    *        first face, then opposite face (each face is given in a Z like pattern, e.g : top left, top right,
    *        bottom left, bottom right).
    *        The second face must be given in the same order as the first one (e.g : if the first point given for the first
    *        face was the top left one, the second face must start with the top left one adn so on.)
    * @param vertices is an empty vector where the subdivision will be written to
    * @param indices is an empty vector where the indices describing the faces will be written to
    */
    void subdivide(uint8_t depth, const vector<float> &parallelepiped, vector<float> &vertices,
                   vector<uint32_t> &indices);

    static void computeSpongeNormals(const vector<float> &vertices, const vector<uint32_t> &indices,
                                     vector<float> &normals);

    static void duplicateVertices(vector<float> &vertices, vector<uint32_t> &indices);

private:
    void addFace(uint64_t size, vector<uint32_t> &indices, Faces face);

    void addFaces(uint64_t size, vector<uint32_t> &indices, const vector<Faces> &apparentFaces);

    /**
     * Subdivide the given line into four equidistant points
     * @param line is a vector containing two points
     * @param result is an empty vector where the subdivision will be written to
    */
    static void subdivideLine(const vector<float> &line, vector<float> &result);

    /**
     * Subdivide the given quadrilateral into four equidistant lines.
     * @param quadrilateral is a vector containing four points
     * @param result is an empty vector where the subdivision will be written to
     */
    static void subdivideQuadrilateral(const vector<float> &quadrilateral, vector<float> &result);

    /**
     * Subdivide the given parallelepiped into four equidistant faces
     * @param parallelepiped is a vector of eights points given in the following order :
     *        first face, then opposite face (each face is given in a Z like pattern, e.g : top left, top right,
     *        bottom left, bottom right).
     *        The second face must be given in the same order as the first one (e.g : if the first point given for the first
    *        face was the top left one, the second face must start with the top left one adn so on.)
    * @param result is an empty vector where the subdivision will be written to
    */
    static void subdivideParallelepiped(const vector<float> &parallelepiped, vector<float> &result);

    void subdivideChild(uint8_t depth, vector<float> &vertices, vector<uint32_t> &indices,
                        const vector<float> &subdivisionResult, const vector<uint8_t> &childIndices,
                        const vector<Faces> &parentApparentFaces, const vector<Faces> &childPossiblyApparentFaces,
                        const vector<Faces> &childMandatoryFaces);

    void recursiveSubdivide(uint8_t depth, const vector<float> &parallelepiped, vector<float> &vertices,
                            vector<uint32_t> &indices, const vector<Faces> &parentApparentFaces);

};

#endif //FRACTALS_PLATONIC4D_SPONGE_H
