#include "../headers/Sponge.h"

using namespace std;

bool Sponge::killComputation = false;

Sponge::Sponge(){
    frontFaceIndices = { 0,   4,  3,
                         4,   7,  3,
                         4,   8,  5,
                         8,   9,  5,
                         6,  10,  7,
                         10, 11,  7,
                         8,  12, 11,
                         12, 15, 11};

    topFaceIndices = {48, 32, 51,
                      32, 35, 51,
                      32, 16, 33,
                      16, 17, 33,
                      34, 18, 35,
                      18, 19, 35,
                      16,  0, 19,
                      0,   3, 19};

    rightFaceIndices = { 3,   7, 51,
                         7,  55, 51,
                         7,  11, 23,
                         11, 27, 23,
                         39, 43, 55,
                         43, 59, 55,
                         11, 15, 59,
                         15, 63, 59};

    bottomFaceIndices = {12, 28, 15,
                         28, 31, 15,
                         28, 44, 29,
                         44, 45, 29,
                         30, 46, 31,
                         46, 47, 31,
                         44, 60, 47,
                         60, 63, 47};

    leftFaceIndices = {48, 52,  0,
                       52,  4,  0,
                       52, 56, 36,
                       56, 40, 36,
                       20, 24,  4,
                       24,  8,  4,
                       56, 60,  8,
                       60, 12,  8};

    backFaceIndices = {60, 56, 63,
                       56, 59, 63,
                       56, 52, 57,
                       52, 53, 57,
                       58, 54, 59,
                       54, 55, 59,
                       52, 48, 55,
                       48, 51, 55};

    faceIndicesList = {&frontFaceIndices, &topFaceIndices, &rightFaceIndices, &bottomFaceIndices, &leftFaceIndices,
                       &backFaceIndices};

    innerParts = { 5,   9, 21,
                   9,  25, 21,
                   9,  10, 25,
                   10, 26, 25,
                   10,  6, 26,
                   6,  22, 26,
                   6,   5, 22,
                   5,  21, 22,

                   23, 27, 22,
                   27, 26, 22,
                   27, 43, 26,
                   43, 42, 26,
                   43, 39, 42,
                   39, 38, 42,
                   39, 23, 38,
                   23, 22, 38,

                   29, 45, 25,
                   45, 41, 25,
                   45, 46, 41,
                   46, 42, 41,
                   46, 30, 42,
                   30, 26, 42,
                   30, 29, 26,
                   29, 25, 26,

                   57, 53, 41,
                   53, 37, 41,
                   53, 54, 37,
                   54, 38, 37,
                   54, 58, 38,
                   58, 42, 38,
                   58, 57, 42,
                   57, 41, 42,

                   36, 40, 37,
                   40, 41, 37,
                   40, 24, 41,
                   24, 25, 41,
                   24, 20, 25,
                   20, 21, 25,
                   20, 36, 21,
                   36, 37, 21,

                   33, 17, 37,
                   17, 21, 37,
                   17, 18, 21,
                   18, 22, 21,
                   18, 34, 22,
                   34, 38, 22,
                   34, 33, 38,
                   33, 37, 38};
}

void Sponge::subdivide(uint8_t depth, const vector<float> &parallelepiped, vector<float> &vertices,
                       vector<uint32_t> &indices) {
    /* Indicates that every single faces are visible by the camera */
    vector<Faces> apparentFaces = {Back, Bottom, Right, Top, Left, Front};
    recursiveSubdivide(depth, parallelepiped, vertices, indices, apparentFaces);
}

void Sponge::computeSpongeNormals(const vector<float> &vertices, const vector<uint32_t> &indices,
                                  vector<float> &normals) {
    normals.resize(vertices.size());
    for (uint32_t i = 0; i < indices.size(); i += 6) {
        if (Sponge::killComputation) throw WorkerKilled();

        glm::vec3 a(
                getPointAbscissa(vertices, indices[i]),
                getPointOrdinate(vertices, indices[i]),
                getPointHeight(vertices, indices[i])
        );
        glm::vec3 b(
                getPointAbscissa(vertices, indices[i + 1]),
                getPointOrdinate(vertices, indices[i + 1]),
                getPointHeight(vertices, indices[i + 1])
        );
        glm::vec3 c(
                getPointAbscissa(vertices, indices[i + 2]),
                getPointOrdinate(vertices, indices[i + 2]),
                getPointHeight(vertices, indices[i + 2])
        );
        glm::vec3 U = b - a, V = c - a;
        glm::vec3 normal = {U.y * V.z - U.z * V.y, U.z * V.x - U.x * V.z, U.x * V.y - U.y * V.x};

        for (uint8_t j = 0; j < 6; ++j) {
            normals[3 * indices[i + j]] = normal.x;
            normals[3 * indices[i + j] + 1] = normal.y;
            normals[3 * indices[i + j] + 2] = normal.z;
        }
    }
}

void Sponge::duplicateVertices(vector<float> &vertices, vector<uint32_t> &indices) {
    vector<float> newVertices;
    uint32_t nextIndex = vertices.size() / 3;

    vector<uint8_t> count(indices.size(), 0);
    for (uint32_t &index: indices) {
        if (Sponge::killComputation) throw WorkerKilled();

        ++count[index];
        if (count[index] > 1) {
            newVertices.push_back(getPointAbscissa(vertices, index));
            newVertices.push_back(getPointOrdinate(vertices, index));
            newVertices.push_back(getPointHeight(vertices, index));
            index = nextIndex++;
        }
    }
    vertices.reserve(vertices.size() + newVertices.size());
    vertices.insert(vertices.end(), newVertices.begin(), newVertices.end());
}

void Sponge::addFace(uint64_t shift, vector<uint32_t> &indices, Faces face) {
    /* Add the list of vertices needed to describe the requested face to the indices list
     * (a shift needs to be applied to compensate for the indices vector not being empty) */
    for (uint8_t index : *(faceIndicesList[face])) {
        indices.push_back(index + shift);
    }
}

void Sponge::addFaces(uint64_t shift, vector<uint32_t> &indices, const vector<Faces> &apparentFaces) {
    /* Add each apparent faces */
    for (Faces face : apparentFaces) {
        addFace(shift, indices, face);
    }

    /* Add the tube made apparent by the holes of the sponge */
    for (uint8_t index : innerParts) {
        indices.push_back(index + shift);
    }
}

void Sponge::subdivideLine(const vector<float> &line, vector<float> &result) {
    /* Add the start of the line to the result */
    addPointToVector(result, line, 0);

    /* Add a point one third of the way between the two points that describe the line */
    addPointOneThirdOfTheWayToVector(result, line, 0, 1);

    /* Add a point two thirds of the way between the two points that describe the line */
    addPointOneThirdOfTheWayToVector(result, line, 1, 0);

    /* Add the end of the line to the result vector */
    addPointToVector(result, line, 1);
}

void Sponge::subdivideQuadrilateral(const vector<float> &quadrilateral, vector<float> &result) {
    /* Extract the first line from the quad (this is simply a side of the polygon) */
    {
        vector<float> line(quadrilateral.begin(), quadrilateral.begin() + 6);
        subdivideLine(line, result);
    }

    /* Create two lines between the chosen side and its opposite.
     * Those lines are parallels and equidistant */
    {
        float startOfLineX, startOfLineY, startOfLineZ;
        startOfLineX = getCoordinateOneThirdOfTheWay(getPointAbscissa(quadrilateral, 0), getPointAbscissa(quadrilateral, 2));
        startOfLineY = getCoordinateOneThirdOfTheWay(getPointOrdinate(quadrilateral, 0), getPointOrdinate(quadrilateral, 2));
        startOfLineZ = getCoordinateOneThirdOfTheWay(getPointHeight(quadrilateral, 0), getPointHeight(quadrilateral, 2));

        float endOfLineX, endOfLineY, endOfLineZ;
        endOfLineX = getCoordinateOneThirdOfTheWay(getPointAbscissa(quadrilateral, 1), getPointAbscissa(quadrilateral, 3));
        endOfLineY = getCoordinateOneThirdOfTheWay(getPointOrdinate(quadrilateral, 1), getPointOrdinate(quadrilateral, 3));
        endOfLineZ = getCoordinateOneThirdOfTheWay(getPointHeight(quadrilateral, 1), getPointHeight(quadrilateral, 3));

        vector<float> line = {
                startOfLineX, startOfLineY, startOfLineZ,
                endOfLineX, endOfLineY, endOfLineZ,
        };
        subdivideLine(line, result);
    }

    {
        float startOfLineX, startOfLineY, startOfLineZ;
        startOfLineX = getCoordinateOneThirdOfTheWay(getPointAbscissa(quadrilateral, 2), getPointAbscissa(quadrilateral, 0));
        startOfLineY = getCoordinateOneThirdOfTheWay(getPointOrdinate(quadrilateral, 2), getPointOrdinate(quadrilateral, 0));
        startOfLineZ = getCoordinateOneThirdOfTheWay(getPointHeight(quadrilateral, 2), getPointHeight(quadrilateral, 0));


        float endOfLineX, endOfLineY, endOfLineZ;
        endOfLineX = getCoordinateOneThirdOfTheWay(getPointAbscissa(quadrilateral, 3), getPointAbscissa(quadrilateral, 1));
        endOfLineY = getCoordinateOneThirdOfTheWay(getPointOrdinate(quadrilateral, 3), getPointOrdinate(quadrilateral, 1));
        endOfLineZ = getCoordinateOneThirdOfTheWay(getPointHeight(quadrilateral, 3), getPointHeight(quadrilateral, 1));

        vector<float> line = {
                startOfLineX, startOfLineY, startOfLineZ,
                endOfLineX, endOfLineY, endOfLineZ,
        };
        subdivideLine(line, result);
    }

    /* Extract the last line from the quad (this is simply the side opposite to the first side) */
    {
        vector<float> line(quadrilateral.begin() + 6, quadrilateral.begin() + 12);
        subdivideLine(line, result);
    }
}

void Sponge::subdivideParallelepiped(const vector<float> &parallelepiped, vector<float> &result) {
    /* Extract the first face of the polygon */
    {
        vector<float> quadrilateral(parallelepiped.begin(), parallelepiped.begin() + 12);
        subdivideQuadrilateral(quadrilateral, result);
    }

    /* Create two quadrilateral between the chosen face and its opposite.
    * Those quadrilateral are parallels and equidistant */
    {
        float upLeftCornerX, upLeftCornerY, upLeftCornerZ;
        upLeftCornerX = getCoordinateOneThirdOfTheWay(getPointAbscissa(parallelepiped, 0), getPointAbscissa(parallelepiped, 4));
        upLeftCornerY = getCoordinateOneThirdOfTheWay(getPointOrdinate(parallelepiped, 0), getPointOrdinate(parallelepiped, 4));
        upLeftCornerZ = getCoordinateOneThirdOfTheWay(getPointHeight(parallelepiped, 0), getPointHeight(parallelepiped, 4));

        float upRightCornerX, upRightCornerY, upRightCornerZ;
        upRightCornerX = getCoordinateOneThirdOfTheWay(getPointAbscissa(parallelepiped, 1), getPointAbscissa(parallelepiped, 5));
        upRightCornerY = getCoordinateOneThirdOfTheWay(getPointOrdinate(parallelepiped, 1), getPointOrdinate(parallelepiped, 5));
        upRightCornerZ = getCoordinateOneThirdOfTheWay(getPointHeight(parallelepiped, 1), getPointHeight(parallelepiped, 5));

        float downLeftCornerX, downLeftCornerY, downLeftCornerZ;
        downLeftCornerX = getCoordinateOneThirdOfTheWay(getPointAbscissa(parallelepiped, 2), getPointAbscissa(parallelepiped, 6));
        downLeftCornerY = getCoordinateOneThirdOfTheWay(getPointOrdinate(parallelepiped, 2), getPointOrdinate(parallelepiped, 6));
        downLeftCornerZ = getCoordinateOneThirdOfTheWay(getPointHeight(parallelepiped, 2), getPointHeight(parallelepiped, 6));

        float downRightCornerX, downRightCornerY, downRightCornerZ;
        downRightCornerX = getCoordinateOneThirdOfTheWay(getPointAbscissa(parallelepiped, 3), getPointAbscissa(parallelepiped, 7));
        downRightCornerY = getCoordinateOneThirdOfTheWay(getPointOrdinate(parallelepiped, 3), getPointOrdinate(parallelepiped, 7));
        downRightCornerZ = getCoordinateOneThirdOfTheWay(getPointHeight(parallelepiped, 3), getPointHeight(parallelepiped, 7));

        vector<float> quadrilateral = {
                upLeftCornerX,    upLeftCornerY,    upLeftCornerZ,
                upRightCornerX,   upRightCornerY,   upRightCornerZ,
                downLeftCornerX,  downLeftCornerY,  downLeftCornerZ,
                downRightCornerX, downRightCornerY, downRightCornerZ,
        };
        subdivideQuadrilateral(quadrilateral, result);
    }

    /* Create two quadrilateral between the chosen face and its opposite.
    * Those quadrilateral are parallels and equidistant */
    {
        float upLeftCornerX, upLeftCornerY, upLeftCornerZ;
        upLeftCornerX = getCoordinateOneThirdOfTheWay(getPointAbscissa(parallelepiped, 4), getPointAbscissa(parallelepiped, 0));
        upLeftCornerY = getCoordinateOneThirdOfTheWay(getPointOrdinate(parallelepiped, 4), getPointOrdinate(parallelepiped, 0));
        upLeftCornerZ = getCoordinateOneThirdOfTheWay(getPointHeight(parallelepiped, 4), getPointHeight(parallelepiped, 0));

        float upRightCornerX, upRightCornerY, upRightCornerZ;
        upRightCornerX = getCoordinateOneThirdOfTheWay(getPointAbscissa(parallelepiped, 5), getPointAbscissa(parallelepiped, 1));
        upRightCornerY = getCoordinateOneThirdOfTheWay(getPointOrdinate(parallelepiped, 5), getPointOrdinate(parallelepiped, 1));
        upRightCornerZ = getCoordinateOneThirdOfTheWay(getPointHeight(parallelepiped, 5), getPointHeight(parallelepiped, 1));

        float downLeftCornerX, downLeftCornerY, downLeftCornerZ;
        downLeftCornerX = getCoordinateOneThirdOfTheWay(getPointAbscissa(parallelepiped, 6), getPointAbscissa(parallelepiped, 2));
        downLeftCornerY = getCoordinateOneThirdOfTheWay(getPointOrdinate(parallelepiped, 6), getPointOrdinate(parallelepiped, 2));
        downLeftCornerZ = getCoordinateOneThirdOfTheWay(getPointHeight(parallelepiped, 6), getPointHeight(parallelepiped, 2));

        float downRightCornerX, downRightCornerY, downRightCornerZ;
        downRightCornerX = getCoordinateOneThirdOfTheWay(getPointAbscissa(parallelepiped, 7), getPointAbscissa(parallelepiped, 3));
        downRightCornerY = getCoordinateOneThirdOfTheWay(getPointOrdinate(parallelepiped, 7), getPointOrdinate(parallelepiped, 3));
        downRightCornerZ = getCoordinateOneThirdOfTheWay(getPointHeight(parallelepiped, 7), getPointHeight(parallelepiped, 3));

        vector<float> quadrilateral = {
                upLeftCornerX,    upLeftCornerY,    upLeftCornerZ,
                upRightCornerX,   upRightCornerY,   upRightCornerZ,
                downLeftCornerX,  downLeftCornerY,  downLeftCornerZ,
                downRightCornerX, downRightCornerY, downRightCornerZ,
        };
        subdivideQuadrilateral(quadrilateral, result);
    }

    /* Extract the last quadrilateral from the parallelepiped (this is simply the face opposite to the first face) */
    {
        vector<float> quadrilateral(parallelepiped.begin() + 12, parallelepiped.begin() + 24);
        subdivideQuadrilateral(quadrilateral, result);
    }
}

void Sponge::subdivideChild(uint8_t depth, vector<float> &vertices, vector<uint32_t> &indices,
                            const vector<float> &parentVertices, const vector<uint8_t> &childIndices,
                            const vector<Faces> &parentApparentFaces, const vector<Faces> &childPossiblyApparentFaces,
                            const vector<Faces> &childMandatoryFaces) {

    /* Extract the child parallelepiped */
    vector<float> childParallelepiped;
    for (uint8_t index : childIndices) {
        addPointToVector(childParallelepiped, parentVertices, index);
    }

    /* Check if the apparent faces aren't hidden because of the a neighbor of the parent cube */
    vector<Faces> childApparentFaces;
    for (Faces face : childPossiblyApparentFaces) {
        if (contains(parentApparentFaces, face)) {
            childApparentFaces.push_back(face);
        }
    }

    /* Indicate which faces will be worth drawing */
    childApparentFaces.insert(childApparentFaces.end(), childMandatoryFaces.begin(), childMandatoryFaces.end());

    /* Subdivide the child parallelepiped */
    recursiveSubdivide(depth - 1, childParallelepiped, vertices, indices, childApparentFaces);
}

void Sponge::recursiveSubdivide(uint8_t depth, const vector<float> &parallelepiped, vector<float> &vertices,
                                vector<uint32_t> &indices, const vector<Faces> &parentApparentFaces) {
    if (killComputation) throw WorkerKilled();

    if (depth > 0) {
        /* Subdivide the given parallelepiped into 27 smaller one */
        vector<float> subdivisionResult;
        subdivideParallelepiped(parallelepiped, subdivisionResult);

        /* Speaking inside the parent parallelepiped, X=0, Y=0, Z=2 (front face, bottom row, last column) */
        {
            vector<uint8_t> childIndices = {0, 1, 4, 5, 16, 17, 20, 21};
            /* Create the set of faces that could be apparent from the exterior, however neighbor cube might hide those
             * faces */
            vector<Faces> childPossiblyApparentFaces = {Back, Left, Bottom};
            vector<Faces> childMandatoryFaces;
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=1, Y=0, Z=2 */
        {
            vector<uint8_t> childIndices = {1, 2, 5, 6, 17, 18, 21, 22};
            /* Create the set of faces that could be apparent from the exterior, however neighbor cube might hide those
             * faces */
            vector<Faces> childPossiblyApparentFaces = {Back, Bottom};
            /* Create the set of faces that will definitely be apparent from the exterior (those are made apparent
             * through the hole of the sponge) */
            vector<Faces> childMandatoryFaces = {Top, Front};
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=2, Y=0, Z=2 */
        {
            vector<uint8_t> childIndices = {2, 3, 6, 7, 18, 19, 22, 23};
            /* Create the set of faces that could be apparent from the exterior, however neighbor cube might hide those
            * faces */
            vector<Faces> childPossiblyApparentFaces = {Back, Bottom, Right};
            vector<Faces> childMandatoryFaces;
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=2, Y=0, Z=1 */
        {
            vector<uint8_t> childIndices = {18, 19, 22, 23, 34, 35, 38, 39};
            /* Create the set of faces that could be apparent from the exterior, however neighbor cube might hide those
            * faces */
            vector<Faces> childPossiblyApparentFaces = {Bottom, Right};
            /* Create the set of faces that will definitely be apparent from the exterior (those are made apparent
            * through the hole of the sponge) */
            vector<Faces> childMandatoryFaces = {Top, Left};
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=0, Y=0, Z=1 */
        {
            vector<uint8_t> childIndices = {16, 17, 20, 21, 32, 33, 36, 37};
            /* Create the set of faces that could be apparent from the exterior, however neighbor cube might hide those
            * faces */
            vector<Faces> childPossiblyApparentFaces = {Left, Bottom};
            /* Create the set of faces that will definitely be apparent from the exterior (those are made apparent
             * through the hole of the sponge) */
            vector<Faces> childMandatoryFaces = {Top, Right};
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=0, Y=0, Z=0 */
        {
            vector<uint8_t> childIndices = {32, 33, 36, 37, 48, 49, 52, 53};
            /* Create the set of faces that could be apparent from the exterior, however neighbor cube might hide those
            * faces */
            vector<Faces> childPossiblyApparentFaces = {Left, Bottom, Front};
            vector<Faces> childMandatoryFaces;
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=1, Y=0, Z=0 */
        {
            vector<uint8_t> childIndices = {33, 34, 37, 38, 49, 50, 53, 54};
            /* Create the set of faces that could be apparent from the exterior, however neighbor cube might hide those
            * faces */
            vector<Faces> childPossiblyApparentFaces = {Bottom, Front};
            /* Create the set of faces that will definitely be apparent from the exterior (those are made apparent
            * through the hole of the sponge) */
            vector<Faces> childMandatoryFaces = {Top, Back};
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=2, Y=0, Z=0 */
        {
            vector<uint8_t> childIndices = {34, 35, 38, 39, 50, 51, 54, 55};
            /* Create the set of faces that could be apparent from the exterior, however neighbor cube might hide those
            * faces */
            vector<Faces> childPossiblyApparentFaces = {Bottom, Front, Right};
            vector<Faces> childMandatoryFaces;
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=2, Y=1, Z=0 */
        {
            vector<uint8_t> childIndices = {38, 39, 42, 43, 54, 55, 58, 59};
            /* Create the set of faces that could be apparent from the exterior, however neighbor cube might hide those
            * faces */
            vector<Faces> childPossiblyApparentFaces = {Front, Right};
            /* Create the set of faces that will definitely be apparent from the exterior (those are made apparent
            * through the hole of the sponge) */
            vector<Faces> childMandatoryFaces = {Back, Left};
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=0, Y=1, Z=0 */
        {
            vector<uint8_t> childIndices = {36, 37, 40, 41, 52, 53, 56, 57};
            /* Create the set of faces that could be apparent from the exterior, however neighbor cube might hide those
            * faces */
            vector<Faces> childPossiblyApparentFaces = {Front, Left};
            /* Create the set of faces that will definitely be apparent from the exterior (those are made apparent
             * through the hole of the sponge) */
            vector<Faces> childMandatoryFaces = {Back, Right};
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=0, Y=1, Z=2 */
        {
            vector<uint8_t> childIndices = {4, 5, 8, 9, 20, 21, 24, 25};
            /* Create the set of faces that could be apparent from the exterior, however neighbor cube might hide those
            * faces */
            vector<Faces> childPossiblyApparentFaces = {Back, Left};
            /* Create the set of faces that will definitely be apparent from the exterior (those are made apparent
            * through the hole of the sponge) */
            vector<Faces> childMandatoryFaces = {Front, Right};
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=2, Y=1, Z=2 */
        {
            vector<uint8_t> childIndices = {6, 7, 10, 11, 22, 23, 26, 27};
            /* Create the set of faces that could be apparent from the exterior, however neighbor cube might hide those
            * faces */
            vector<Faces> childPossiblyApparentFaces = {Back, Right};
            /* Create the set of faces that will definitely be apparent from the exterior (those are made apparent
            * through the hole of the sponge) */
            vector<Faces> childMandatoryFaces = {Front, Left};
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=2, Y=2, Z=2 */
        {
            vector<uint8_t> childIndices = {10, 11, 14, 15, 26, 27, 30, 31};
            /* Create the set of faces that could be apparent from the exterior, however neighbor cube might hide those
            * faces */
            vector<Faces> childPossiblyApparentFaces = {Back, Right, Top};
            vector<Faces> childMandatoryFaces;
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=1, Y=2, Z=2 */
        {
            vector<uint8_t> childIndices = {9, 10, 13, 14, 25, 26, 29, 30};
            /* Create the set of faces that could be apparent from the exterior, however neighbor cube might hide those
            * faces */
            vector<Faces> childPossiblyApparentFaces = {Back, Top};
            /* Create the set of faces that will definitely be apparent from the exterior (those are made apparent
            * through the hole of the sponge) */
            vector<Faces> childMandatoryFaces = {Front, Bottom};
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=0, Y=2, Z=2 */
        {
            vector<uint8_t> childIndices = {8, 9, 12, 13, 24, 25, 28, 29};
            /* Create the set of faces that could be apparent from the exterior, however neighbor cube might hide those
             * faces */
            vector<Faces> childPossiblyApparentFaces = {Back, Left, Top};
            vector<Faces> childMandatoryFaces;
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=0, Y=2, Z=1 */
        {
            vector<uint8_t> childIndices = {24, 25, 28, 29, 40, 41, 44, 45};
            /* Create the set of faces that could be apparent from the exterior, however neighbor cube might hide those
             * faces */
            vector<Faces> childPossiblyApparentFaces = {Left, Top};
            /* Create the set of faces that will definitely be apparent from the exterior (those are made apparent
            * through the hole of the sponge) */
            vector<Faces> childMandatoryFaces = {Bottom, Right};
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=2, Y=2, Z=1 */
        {
            vector<uint8_t> childIndices = {26, 27, 30, 31, 42, 43, 46, 47};
            /* Create the set of faces that could be apparent from the exterior, however neighbor cube might hide those
            * faces */
            vector<Faces> childPossiblyApparentFaces = {Right, Top};
            /* Create the set of faces that will definitely be apparent from the exterior (those are made apparent
             * through the hole of the sponge) */
            vector<Faces> childMandatoryFaces = {Bottom, Left};
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=2, Y=2, Z=0 */
        {
            vector<uint8_t> childIndices = {42, 43, 46, 47, 58, 59, 62, 63};
            /* Create the set of faces that could be apparent from the exterior, however neighbor cube might hide those
            * faces */
            vector<Faces> childPossiblyApparentFaces = {Right, Top, Front};
            vector<Faces> childMandatoryFaces;
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=1, Y=2, Z=0 */
        {
            vector<uint8_t> childIndices = {41, 42, 45, 46, 57, 58, 61, 62};
            /* Create the set of faces that could be apparent from the exterior, however neighbor cube might hide those
             * faces */
            vector<Faces> childPossiblyApparentFaces = {Front, Top};
            /* Create the set of faces that will definitely be apparent from the exterior (those are made apparent
            * through the hole of the sponge) */
            vector<Faces> childMandatoryFaces = {Bottom, Back};
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=1, Y=2, Z=0 */
        {
            vector<uint8_t> childIndices = {40, 41, 44, 45, 56, 57, 60, 61};
            /* Create the set of faces that could be apparent from the exterior, however neighbor cube might hide those
             * faces */
            vector<Faces> childPossiblyApparentFaces = {Left, Top, Front};
            vector<Faces> childMandatoryFaces;
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

    } else {
        /* Max depth is achieved, the given parallelepiped is subdivided and the subdivision is added to the vertices
         * list */
        subdivideParallelepiped(parallelepiped, vertices);
        /* Indices are added in order to draw the faces described by the newly created vertices */
        addFaces((vertices.size() - 192) / 3, indices, parentApparentFaces);
    }
}


