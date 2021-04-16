#include <iostream>
#include "../headers/sponge.h"

std::vector<uint8_t> frontFaceIndices = { 0,  3,  4,
                                          4,  3,  7,
                                          4,  5,  8,
                                          8,  5,  9,
                                          6,  7, 10,
                                         10,  7, 11,
                                          8, 11, 12,
                                         12, 11, 15};

std::vector<uint8_t> topFaceIndices = {48, 51, 32,
                                       32, 51, 35,
                                       32, 33, 16,
                                       16, 33, 17,
                                       34, 35, 18,
                                       18, 35, 19,
                                       16, 19,  0,
                                        0, 19,  3};

std::vector<uint8_t> rightFaceIndices = { 3, 51,  7,
                                          7, 51, 55,
                                          7, 23, 11,
                                         11, 23, 27,
                                         39, 55, 43,
                                         43, 55, 59,
                                         11, 59, 15,
                                         15, 59, 63};

std::vector<uint8_t> bottomFaceIndices = {12, 15, 28,
                                          28, 15, 31,
                                          28, 29, 44,
                                          44, 29, 45,
                                          30, 31, 46,
                                          46, 31, 47,
                                          44, 47, 60,
                                          60, 47, 63};

std::vector<uint8_t> leftFaceIndices = {48,  0, 52,
                                        52,  0,  4,
                                        52, 36, 56,
                                        56, 36, 40,
                                        20,  4, 24,
                                        24,  4,  8,
                                        56,  8, 60,
                                        60,  8, 12};

std::vector<uint8_t> backFaceIndices = {60, 63, 56,
                                        56, 63, 59,
                                        56, 57, 52,
                                        52, 57, 53,
                                        58, 59, 54,
                                        54, 59, 55,
                                        52, 55, 48,
                                        48, 55, 51};

std::vector<std::vector<uint8_t> *> faceIndicesList = {&frontFaceIndices, &topFaceIndices, &rightFaceIndices,
                                                       &bottomFaceIndices, &leftFaceIndices, &backFaceIndices};

std::vector<uint8_t> innerParts = { 5, 21,  9,
                                    9, 21, 25,
                                    9, 25, 10,
                                   10, 25, 26,
                                   10, 26,  6,
                                    6, 26, 22,
                                    6, 22,  5,
                                    5, 22, 21,

                                   23, 22, 27,
                                   27, 22, 26,
                                   27, 26, 43,
                                   43, 26, 42,
                                   43, 42, 39,
                                   39, 42, 38,
                                   39, 38, 23,
                                   23, 38, 22,

                                   29, 25, 45,
                                   45, 25, 41,
                                   45, 41, 46,
                                   46, 41, 42,
                                   46, 42, 30,
                                   30, 42, 26,
                                   30, 26, 29,
                                   29, 26, 25,

                                   57, 41, 53,
                                   53, 41, 37,
                                   53, 37, 54,
                                   54, 37, 38,
                                   54, 38, 58,
                                   58, 38, 42,
                                   58, 42, 57,
                                   57, 42, 41,

                                   36, 37, 40,
                                   40, 37, 41,
                                   40, 41, 24,
                                   24, 41, 25,
                                   24, 25, 20,
                                   20, 25, 21,
                                   20, 21, 36,
                                   36, 21, 37,

                                   33, 37, 17,
                                   17, 37, 21,
                                   17, 21, 18,
                                   18, 21, 22,
                                   18, 22, 34,
                                   34, 22, 38,
                                   34, 38, 33,
                                   33, 38, 37};


enum Faces {
    Back = 0,
    Bottom = 1,
    Right = 2,
    Top = 3,
    Left = 4,
    Front = 5,
};

using namespace std;

void recursiveSubdivide(uint8_t depth, const vector<float> &parallelepiped, vector<float> &vertices,
                        vector<uint32_t> &indices, const vector<Faces> &parentApparentFaces);

void addFace(uint64_t size, vector<uint32_t> &indices, Faces face) {
    for (uint8_t index : *(faceIndicesList[face])) {
        indices.push_back(index + (size - 192) / 3);
    }
}

void addFaces(uint64_t size, vector<uint32_t> &indices, const vector<Faces> &apparentFaces) {
    if (!apparentFaces.empty()) {
        for (Faces face : apparentFaces) {
            addFace(size, indices, face);
        }
    }
    for (uint8_t index : innerParts) {
        indices.push_back(index + (size - 192) / 3);
    }
}

bool contains(const vector<Faces> &vector, Faces element) {
    return any_of(vector.begin(), vector.end(), [&element](Faces face){
        return face == element;
    });
}

float getPointAbscissa(const vector<float> &sourceVector, uint32_t pointIndex) {
    return sourceVector[pointIndex * 3];
}

float getPointOrdinate(const vector<float> &sourceVector, uint32_t pointIndex) {
    return sourceVector[pointIndex * 3 + 1];
}

float getPointHeight(const vector<float> &sourceVector, uint32_t pointIndex) {
    return sourceVector[pointIndex * 3 + 2];
}

vector<float> &addPointToVector(vector<float> &targetVector, const vector<float> &sourceVector, uint32_t pointIndex) {
    targetVector.push_back(getPointAbscissa(sourceVector, pointIndex));
    targetVector.push_back(getPointOrdinate(sourceVector, pointIndex));
    targetVector.push_back(getPointHeight(sourceVector, pointIndex));
    return targetVector;
}

/**
 * Subdivide the given line into four equidistant points
 * @param line is a vector containing two points
 * @param result is an empty vector where the subdivision will be written to
 */
static void subdivideLine(const vector<float> &line, vector<float> &result) {
    /* Add the start of the line to the result */
    addPointToVector(result, line, 0);

    /* Add a point one third of the way between the two points that describe the line */
    result.push_back(getPointAbscissa(line, 0) - (getPointAbscissa(line, 0) - getPointAbscissa(line, 1)) / 3.0f);
    result.push_back(getPointOrdinate(line, 0) - (getPointOrdinate(line, 0) - getPointOrdinate(line, 1)) / 3.0f);
    result.push_back(getPointHeight(line, 0) - (getPointHeight(line, 0) - getPointHeight(line, 1)) / 3.0f);

    /* Add a point two thirds of the way between the two points that describe the line */
    result.push_back(getPointAbscissa(line, 0) - (getPointAbscissa(line, 0) - getPointAbscissa(line, 1)) * 2.0f / 3.0f);
    result.push_back(getPointOrdinate(line, 0) - (getPointOrdinate(line, 0) - getPointOrdinate(line, 1)) * 2.0f / 3.0f);
    result.push_back(getPointHeight(line, 0) - (getPointHeight(line, 0) - getPointHeight(line, 1)) * 2.0f / 3.0f);

    /* Add the end of the line to the result vector */
    addPointToVector(result, line, 1);
}

/**
 * Subdivide the given quadrilateral into four equidistant lines.
 * @param quadrilateral is a vector containing four points
 * @param result is an empty vector where the subdivision will be written to
 */
static void subdivideQuadrilateral(const vector<float> &quadrilateral, vector<float> &result) {
    /* Extract the first line from the quad (this is simply a side of the polygon) */
    {
        vector<float> line(quadrilateral.begin(), quadrilateral.begin() + 6);
        subdivideLine(line, result);
    }

    /* Create two lines between the chosen side and its opposite.
     * Those lines are parallels and equidistant */
    for (uint8_t i = 1; i < 3; ++i) {
        float startOfLineX, startOfLineY, startOfLineZ;
        startOfLineX = getPointAbscissa(quadrilateral, 0) -
                       (getPointAbscissa(quadrilateral, 0) - getPointAbscissa(quadrilateral, 2)) * (float) i / 3.0f;
        startOfLineY = getPointOrdinate(quadrilateral, 0) -
                       (getPointOrdinate(quadrilateral, 0) - getPointOrdinate(quadrilateral, 2)) * (float) i / 3.0f;
        startOfLineZ = getPointHeight(quadrilateral, 0) -
                       (getPointHeight(quadrilateral, 0) - getPointHeight(quadrilateral, 2)) * (float) i / 3.0f;

        float endOfLineX, endOfLineY, endOfLineZ;
        endOfLineX = getPointAbscissa(quadrilateral, 1) -
                     (getPointAbscissa(quadrilateral, 1) - getPointAbscissa(quadrilateral, 3)) * (float) i / 3.0f;
        endOfLineY = getPointOrdinate(quadrilateral, 1) -
                     (getPointOrdinate(quadrilateral, 1) - getPointOrdinate(quadrilateral, 3)) * (float) i / 3.0f;
        endOfLineZ = getPointHeight(quadrilateral, 1) -
                     (getPointHeight(quadrilateral, 1) - getPointHeight(quadrilateral, 3)) * (float) i / 3.0f;

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

/**
 * Subdivide the given parallelepiped into four equidistant faces
 * @param parallelepiped is a vector of eights points given in the following order :
 *        first face, then opposite face (each face is given in a Z like pattern, e.g : top left, top right,
 *        bottom left, bottom right).
 *        The second face must be given in the same order as the first one (e.g : if the first point given for the first
 *        face was the top left one, the second face must start with the top left one adn so on.)
 * @param result is an empty vector where the subdivision will be written to
 */
static void subdivideParallelepiped(const vector<float> &parallelepiped, vector<float> &result) {
    /* Extract the first face of the polygon */
    {
        vector<float> quadrilateral(parallelepiped.begin(), parallelepiped.begin() + 12);
        subdivideQuadrilateral(quadrilateral, result);
    }

    /* Create two quadrilateral between the chosen face and its opposite.
    * Those quadrilateral are parallels and equidistant */
    for (uint8_t i = 1; i < 3; ++i) {
        float upLeftCornerX, upLeftCornerY, upLeftCornerZ;
        upLeftCornerX = parallelepiped[0] - (parallelepiped[0] - parallelepiped[12]) * (float) i / 3.0f; //todo replace with getter functions
        upLeftCornerY = parallelepiped[1] - (parallelepiped[1] - parallelepiped[13]) * (float) i / 3.0f;
        upLeftCornerZ = parallelepiped[2] - (parallelepiped[2] - parallelepiped[14]) * (float) i / 3.0f;

        float upRightCornerX, upRightCornerY, upRightCornerZ;
        upRightCornerX = parallelepiped[3] - (parallelepiped[3] - parallelepiped[15]) * (float) i / 3.0f;
        upRightCornerY = parallelepiped[4] - (parallelepiped[4] - parallelepiped[16]) * (float) i / 3.0f;
        upRightCornerZ = parallelepiped[5] - (parallelepiped[5] - parallelepiped[17]) * (float) i / 3.0f;

        float downLeftCornerX, downLeftCornerY, downLeftCornerZ;
        downLeftCornerX = parallelepiped[6] - (parallelepiped[6] - parallelepiped[18]) * (float) i / 3.0f;
        downLeftCornerY = parallelepiped[7] - (parallelepiped[7] - parallelepiped[19]) * (float) i / 3.0f;
        downLeftCornerZ = parallelepiped[8] - (parallelepiped[8] - parallelepiped[20]) * (float) i / 3.0f;

        float downRightCornerX, downRightCornerY, downRightCornerZ;
        downRightCornerX = parallelepiped[9] - (parallelepiped[9] - parallelepiped[21]) * (float) i / 3.0f;
        downRightCornerY = parallelepiped[10] - (parallelepiped[10] - parallelepiped[22]) * (float) i / 3.0f;
        downRightCornerZ = parallelepiped[11] - (parallelepiped[11] - parallelepiped[23]) * (float) i / 3.0f;

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

void subdivideChild(uint8_t depth, vector<float> &vertices, vector<uint32_t> &indices,
                    const vector<float> &subdivisionResult, const vector<uint8_t> &childIndices,
                    const vector<Faces> &parentApparentFaces, const vector<Faces> &childPossiblyApparentFaces,
                    const vector<Faces> &childMandatoryFaces) {

    /* Extract the first child parallelepiped */
    vector<float> childParallelepiped;
    for (uint8_t index : childIndices) {
        addPointToVector(childParallelepiped, subdivisionResult, index);
    }

    /* Indicate which faces are worth drawing */
    vector<Faces> childApparentFaces;
    for (Faces face : childPossiblyApparentFaces) {
        if (contains(parentApparentFaces, face)) {
            childApparentFaces.push_back(face);
        }
    }

    childApparentFaces.insert(childApparentFaces.end(), childMandatoryFaces.begin(), childMandatoryFaces.end());

    /* Subdivide the child parallelepiped */
    recursiveSubdivide(depth - 1, childParallelepiped, vertices, indices, childApparentFaces);
}

void recursiveSubdivide(uint8_t depth, const vector<float> &parallelepiped, vector<float> &vertices,
                        vector<uint32_t> &indices, const vector<Faces> &parentApparentFaces) {

    if (depth > 0) {
        /* Subdivide the given parallelepiped into 27 smaller one */
        vector<float> subdivisionResult;
        subdivideParallelepiped(parallelepiped, subdivisionResult);

        /* Speaking inside the parent parallelepiped, X=0, Y=0, Z=2 (front face, bottom row, last column) */
        {
            vector<uint8_t> childIndices = {0, 1, 4, 5, 16, 17, 20, 21};
            vector<Faces> childPossiblyApparentFaces = {Back, Left, Bottom};
            vector<Faces> childMandatoryFaces;
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=1, Y=0, Z=2 */
        {
            vector<uint8_t> childIndices = {1, 2, 5, 6, 17, 18, 21, 22};
            vector<Faces> childPossiblyApparentFaces = {Back, Bottom};
            vector<Faces> childMandatoryFaces = {Top, Front};
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=2, Y=0, Z=2 */
        {
            vector<uint8_t> childIndices = {2, 3, 6, 7, 18, 19, 22, 23};
            vector<Faces> childPossiblyApparentFaces = {Back, Bottom, Right};
            vector<Faces> childMandatoryFaces;
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=2, Y=0, Z=1 */
        {
            vector<uint8_t> childIndices = {18, 19, 22, 23, 34, 35, 38, 39};
            vector<Faces> childPossiblyApparentFaces = {Bottom, Right};
            vector<Faces> childMandatoryFaces = {Top, Left};
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=0, Y=0, Z=1 */
        {
            vector<uint8_t> childIndices = {16, 17, 20, 21, 32, 33, 36, 37};
            vector<Faces> childPossiblyApparentFaces = {Left, Bottom};
            vector<Faces> childMandatoryFaces = {Top, Right};
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=0, Y=0, Z=0 */
        {
            vector<uint8_t> childIndices = {32, 33, 36, 37, 48, 49, 52, 53};
            vector<Faces> childPossiblyApparentFaces = {Left, Bottom, Front};
            vector<Faces> childMandatoryFaces;
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=1, Y=0, Z=0 */
        {
            vector<uint8_t> childIndices = {33, 34, 37, 38, 49, 50, 53, 54};
            vector<Faces> childPossiblyApparentFaces = {Bottom, Front};
            vector<Faces> childMandatoryFaces = {Top, Back};
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=2, Y=0, Z=0 */
        {
            vector<uint8_t> childIndices = {34, 35, 38, 39, 50, 51, 54, 55};
            vector<Faces> childPossiblyApparentFaces = {Bottom, Front, Right};
            vector<Faces> childMandatoryFaces;
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=2, Y=1, Z=0 */
        {
            vector<uint8_t> childIndices = {38, 39, 42, 43, 54, 55, 58, 59};
            vector<Faces> childPossiblyApparentFaces = {Front, Right};
            vector<Faces> childMandatoryFaces = {Back, Left};
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=0, Y=1, Z=0 */
        {
            vector<uint8_t> childIndices = {36, 37, 40, 41, 52, 53, 56, 57};
            vector<Faces> childPossiblyApparentFaces = {Front, Left};
            vector<Faces> childMandatoryFaces = {Back, Right};
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=0, Y=1, Z=2 */
        {
            vector<uint8_t> childIndices = {4, 5, 8, 9, 20, 21, 24, 25};
            vector<Faces> childPossiblyApparentFaces = {Back, Left};
            vector<Faces> childMandatoryFaces = {Front, Right};
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=2, Y=1, Z=2 */
        {
            vector<uint8_t> childIndices = {6, 7, 10, 11, 22, 23, 26, 27};
            vector<Faces> childPossiblyApparentFaces = {Back, Right};
            vector<Faces> childMandatoryFaces = {Front, Left};
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=2, Y=2, Z=2 */
        {
            vector<uint8_t> childIndices = {10, 11, 14, 15, 26, 27, 30, 31};
            vector<Faces> childPossiblyApparentFaces = {Back, Right, Top};
            vector<Faces> childMandatoryFaces;
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=1, Y=2, Z=2 */
        {
            vector<uint8_t> childIndices = {9, 10, 13, 14, 25, 26, 29, 30};
            vector<Faces> childPossiblyApparentFaces = {Back, Top};
            vector<Faces> childMandatoryFaces = {Front, Bottom};
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=0, Y=2, Z=2 */
        {
            vector<uint8_t> childIndices = {8, 9, 12, 13, 24, 25, 28, 29};
            vector<Faces> childPossiblyApparentFaces = {Back, Left, Top};
            vector<Faces> childMandatoryFaces;
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=0, Y=2, Z=1 */
        {
            vector<uint8_t> childIndices = {24, 25, 28, 29, 40, 41, 44, 45};
            vector<Faces> childPossiblyApparentFaces = {Left, Top};
            vector<Faces> childMandatoryFaces = {Bottom, Right};
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=2, Y=2, Z=1 */
        {
            vector<uint8_t> childIndices = {26, 27, 30, 31, 42, 43, 46, 47};
            vector<Faces> childPossiblyApparentFaces = {Right, Top};
            vector<Faces> childMandatoryFaces = {Bottom, Left};
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=2, Y=2, Z=0 */
        {
            vector<uint8_t> childIndices = {42, 43, 46, 47, 58, 59, 62, 63};
            vector<Faces> childPossiblyApparentFaces = {Right, Top, Front};
            vector<Faces> childMandatoryFaces;
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=1, Y=2, Z=0 */
        {
            vector<uint8_t> childIndices = {41, 42, 45, 46, 57, 58, 61, 62};
            vector<Faces> childPossiblyApparentFaces = {Front, Top};
            vector<Faces> childMandatoryFaces = {Bottom, Back};
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

        /* X=1, Y=2, Z=0 */
        {
            vector<uint8_t> childIndices = {40, 41, 44, 45, 56, 57, 60, 61};
            vector<Faces> childPossiblyApparentFaces = {Left, Top, Front};
            vector<Faces> childMandatoryFaces;
            subdivideChild(depth, vertices, indices, subdivisionResult, childIndices, parentApparentFaces,
                           childPossiblyApparentFaces, childMandatoryFaces);
        }

    } else {
        subdivideParallelepiped(parallelepiped, vertices);
        addFaces(vertices.size(), indices, parentApparentFaces);
    }
}


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
               vector<uint32_t> &indices) {
    vector<Faces> apparentFaces = {Back, Bottom, Right, Top, Left, Front};
    recursiveSubdivide(depth, parallelepiped, vertices, indices, apparentFaces);
}

void computeSpongeNormals(const vector<float> &vertices, const vector<uint32_t> &indices, vector<float> &normals) {
    normals.resize(vertices.size());
    for (uint32_t i = 0; i < indices.size(); i += 6) {
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

static uint64_t getNumberOfCubes(int8_t depth) {
    if (depth == -1) {
        return 0;
    } else {
        return pow(20, depth);
    }
}

static uint64_t getNumberOfVertices(uint8_t depth) {
    return getNumberOfCubes(depth) * 32 + 8;
}

void duplicateVertices(vector<float> &vertices, vector<uint32_t> &indices) {
    vector<float> newVertices;
    uint32_t nextIndex = vertices.size() / 3;

    map<uint32_t, uint8_t> count;
    for (uint32_t &index: indices) {
        if (count.find(index) == count.end()) count[index] = 1;
        else {
            ++count[index];
            newVertices.push_back(getPointAbscissa(vertices, index));
            newVertices.push_back(getPointOrdinate(vertices, index));
            newVertices.push_back(getPointHeight(vertices, index));
            index = nextIndex++;
        }
    }
    vertices.insert(vertices.end(), newVertices.begin(), newVertices.end());
}
