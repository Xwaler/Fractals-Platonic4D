#ifndef FRACTALS_PLATONIC4D_SPONGE_H
#define FRACTALS_PLATONIC4D_SPONGE_H

#include <glm/glm.hpp>

#include <cstdint>
#include <cmath>
#include <vector>
#include <map>
#include <algorithm>
#include <future>

#include "vectorTools.h"

using namespace std;

struct WorkerKilled : public exception {
    const char * what () const throw () {
        return "SpongeWorker was purposely killed";
    }
};

class Sponge {
public:
    static bool killComputation;
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

    /**
     * Computes normals relative to vertices that will be used for lighting
     * @param vertices is a vector containing the vertices
     * @param indices is a vector containing indices that describe triangles to draw
     * @param normals is an empty vector that will contain normal in the same order as the vertices parameter
     */
    static void computeSpongeNormals(const vector<float> &vertices, const vector<uint32_t> &indices,
                                     vector<float> &normals);

    /**
     * Duplicates vertices involve in several faces to avoid lighting issues
     * @param vertices is a vector containing vertices, will be modified
     * @param indices is a vector containing indices that describe triangle, will be modified
     */
    static void duplicateVertices(vector<float> &vertices, vector<uint32_t> &indices);

private:

    /**
     * Add the face indicated in parameter at the end the indices vector
     * @param shift is the shift that need to be applied to base value of indices
     * (this should be the number of vertices that were in the vector before this batch)
     * @param indices is a vector containing indices that describe triangle. The new face will be added at the end
     * @param face is the face that will be added at the end of the indices vector
     */
    void addFace(uint64_t shift, vector<uint32_t> &indices, Faces face);

    /**
     * Add the faces indicated in parameter at the end of the indices vector
     * @param shift is the shift that need to be applied to base value of indices
     * (this should be the number of vertices that were in the vector before this batch)
     * @param indices is a vector containing indices that describe triangle. The new face will be added at the end
     * @param apparentFaces is a vector containing the faces that needs to be added
     */
    void addFaces(uint64_t shift, vector<uint32_t> &indices, const vector<Faces> &apparentFaces);

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

    /**
     * Subdivide the child parallelepiped indicated in parameter in a Menger Sponge like pattern
     * @param depth is the depth where the subdivision should stop
     * @param vertices is a vector where the result vertices will be append to
     * @param indices is a vector where the result faces will be append to
     * @param parentVertices is a vector containing the vertices of the parent (a subdivided parallelepiped)
     * @param childIndices is a vector of the 8 vertices that describe the child that will be subdivided
     * @param parentApparentFaces is a vector indicating which faces of the parent are visible
     * @param childPossiblyApparentFaces is a vector indicated which faces of the child might be visible
     * (those faces will not be visible is the parent faces they belong to isn't)
     * @param childMandatoryFaces is a vector indicated which faces of the child will definitely be visible
     */
    void subdivideChild(uint8_t depth, vector<float> &vertices, vector<uint32_t> &indices,
                        const vector<float> &parentVertices, const vector<uint8_t> &childIndices,
                        const vector<Faces> &parentApparentFaces, const vector<Faces> &childPossiblyApparentFaces,
                        const vector<Faces> &childMandatoryFaces);

    /**
     * Recursive function that will subdivide a parallelepiped into a Menger sponge like pattern
     * @param depth is the depth where to stop the subdivision
     * @param parallelepiped is a vector containing 8 vertices that describe the parallelepiped that will be subdivide
     * @param vertices is a vector where the result vertices will be append to
     * @param indices is a vector where the result faces will be append to
     * @param parentApparentFaces is a vector indicating which faces of the parent are visible
     */
    void recursiveSubdivide(uint8_t depth, const vector<float> &parallelepiped, vector<float> &vertices,
                            vector<uint32_t> &indices, const vector<Faces> &parentApparentFaces);

};

#endif //FRACTALS_PLATONIC4D_SPONGE_H
