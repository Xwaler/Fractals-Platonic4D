#include <iostream>
#include "../headers/sponge.h"

std::vector<uint8_t> frontFaceIndices = {0, 4, 3,
                                         4, 7, 3,
                                         4, 8, 5,
                                         8, 9, 5,
                                         6, 10, 7,
                                         10, 11, 7,
                                         8, 12, 11,
                                         12, 15, 11,};

std::vector<uint8_t> topFaceIndices = {48, 32, 51,
                                       32, 35, 51,
                                       32, 16, 33,
                                       16, 17, 33,
                                       34, 18, 35,
                                       18, 19, 35,
                                       16, 0, 19,
                                       0, 3, 19,};

std::vector<uint8_t> rightFaceIndices = {3, 7, 51,
                                         7, 55, 51,
                                         7, 11, 23,
                                         11, 27, 23,
                                         39, 43, 55,
                                         43, 59, 55,
                                         11, 15, 59,
                                         15, 63, 59,};

std::vector<uint8_t> bottomFaceIndices = {12, 28, 15,
                                          28, 31, 15,
                                          28, 44, 29,
                                          44, 45, 29,
                                          30, 46, 31,
                                          46, 47, 31,
                                          44, 60, 47,
                                          60, 63, 47,};

std::vector<uint8_t> leftFaceIndices = {48, 52, 0,
                                        52, 4, 0,
                                        52, 56, 36,
                                        56, 40, 36,
                                        20, 24, 4,
                                        24, 8, 4,
                                        56, 60, 8,
                                        60, 12, 8,};

std::vector<uint8_t> backFaceIndices = {60, 56, 63,
                                        56, 59, 63,
                                        56, 52, 57,
                                        52, 53, 57,
                                        58, 54, 59,
                                        54, 55, 59,
                                        52, 48, 55,
                                        48, 51, 55,};

std::vector<std::vector<uint8_t> *> faceIndicesList = {&frontFaceIndices, &topFaceIndices, &rightFaceIndices,
                                                       &bottomFaceIndices, &leftFaceIndices, &backFaceIndices};

std::vector<uint8_t> innerParts = {5, 9, 21,
                                   9, 25, 21,
                                   9, 10, 25,
                                   10, 26, 25,
                                   10, 6, 26,
                                   6, 22, 26,
                                   6, 5, 22,
                                   5, 21, 22,

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
                                   33, 37, 38,};


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

bool contains(const vector<Faces> &vector, Faces &element) {
    return any_of(vector.begin(), vector.end(), [&element](Faces face){ return face == element; });
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
        upLeftCornerX = parallelepiped[0] - (parallelepiped[0] - parallelepiped[12]) * (float) i /
                                            3.0f; //todo replace with getter functions
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
                upLeftCornerX, upLeftCornerY, upLeftCornerZ,
                upRightCornerX, upRightCornerY, upRightCornerZ,
                downLeftCornerX, downLeftCornerY, downLeftCornerZ,
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
            vector<uint8_t> childIndices = {40, 42, 44, 45, 56, 57, 60, 61};
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
