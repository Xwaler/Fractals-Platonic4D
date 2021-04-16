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

std::vector<std::vector<uint8_t> *> faceIndicesList = {&frontFaceIndices, &topFaceIndices, &rightFaceIndices, &bottomFaceIndices, &leftFaceIndices, &backFaceIndices};

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


enum Faces{
    Front = 0,
    Top = 1,
    Right = 2,
    Bottom = 3,
    Left = 4,
    Back = 5,
};

using namespace std;

void addFace(uint64_t size, vector<uint32_t> &indices, Faces face) {
    std::cout << unsigned(face) << std::endl;
    std::cout << unsigned(size) << std::endl;
    for (uint8_t index : *(faceIndicesList[face])){
        indices.push_back(index + size - 192);
    }
}

void addFaces(uint64_t size, vector<uint32_t> &indices, const vector<Faces> &apparentFaces) {
    if (!apparentFaces.empty()) {
        for (Faces face : apparentFaces){
            addFace(size, indices, face);
        }
    }
    for (uint8_t index : innerParts){
        indices.push_back(index + size - 192);
    }
}

bool contains(const vector<Faces> &vector, Faces element){
    for ( Faces face : vector ) {
        if (face == element){
            return true;
        }
    }
    return false;
}

float getPointAbscissa(const vector<float>& sourceVector, uint32_t pointIndex){
    return sourceVector[pointIndex * 3];
}

float getPointOrdinate(const vector<float>& sourceVector, uint32_t pointIndex){
    return sourceVector[pointIndex * 3 + 1];
}

float getPointHeight(const vector<float>& sourceVector, uint32_t pointIndex){
    return sourceVector[pointIndex * 3 + 2];
}

vector<float>& addPointToVector(vector<float>& targetVector, const vector<float>& sourceVector, uint32_t pointIndex){
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
    result.push_back(getPointHeight(line, 0) - (getPointHeight(line, 0)- getPointHeight(line, 1)) / 3.0f);

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
        startOfLineX = getPointAbscissa(quadrilateral, 0) - (getPointAbscissa(quadrilateral, 0) - getPointAbscissa(quadrilateral, 2)) * (float) i / 3.0f;
        startOfLineY = getPointOrdinate(quadrilateral, 0) - (getPointOrdinate(quadrilateral, 0) - getPointOrdinate(quadrilateral, 2)) * (float) i / 3.0f;
        startOfLineZ = getPointHeight(quadrilateral, 0) - (getPointHeight(quadrilateral, 0) - getPointHeight(quadrilateral, 2)) * (float) i / 3.0f;

        float endOfLineX, endOfLineY, endOfLineZ;
        endOfLineX = getPointAbscissa(quadrilateral, 1) - (getPointAbscissa(quadrilateral, 1) - getPointAbscissa(quadrilateral, 3)) * (float) i / 3.0f;
        endOfLineY = getPointOrdinate(quadrilateral, 1) - (getPointOrdinate(quadrilateral, 1) - getPointOrdinate(quadrilateral, 3)) * (float) i / 3.0f;
        endOfLineZ = getPointHeight(quadrilateral, 1) - (getPointHeight(quadrilateral, 1) - getPointHeight(quadrilateral, 3)) * (float) i / 3.0f;

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

void recursiveSubdivide(uint8_t depth, const vector<float> &parallelepiped, vector<float> &vertices,
                        vector<uint32_t> &indices, const vector<Faces>& apparentFaces){

    if (depth > 0){
        /* Subdivide the given parallelepiped into 27 smaller one */
        vector<float> subdivisionResult;
        subdivideParallelepiped(parallelepiped, subdivisionResult);

        /* Extract the first child parallelepiped */
        vector<float> childParallelepiped;
        addPointToVector(childParallelepiped, subdivisionResult, 0);
        addPointToVector(childParallelepiped, subdivisionResult, 1);
        addPointToVector(childParallelepiped, subdivisionResult, 4);
        addPointToVector(childParallelepiped, subdivisionResult, 5);
        addPointToVector(childParallelepiped, subdivisionResult, 16);
        addPointToVector(childParallelepiped, subdivisionResult, 17);
        addPointToVector(childParallelepiped, subdivisionResult, 20);
        addPointToVector(childParallelepiped, subdivisionResult, 21);

        /* Indicate which faces are worth drawing */
        vector<Faces> childApparentFaces;
        if (contains(apparentFaces, Front)){
            childApparentFaces.push_back(Front);
        }
        if (contains(apparentFaces, Left)){
            childApparentFaces.push_back(Left);
        }
        if (contains(apparentFaces, Top)){
            childApparentFaces.push_back(Top);
        }

        /* Subdivide the child parallelepiped */
        recursiveSubdivide(depth - 1, childParallelepiped, vertices, indices, childApparentFaces);
    } else {
        subdivideParallelepiped(parallelepiped, vertices);
        addFaces(vertices.size(), indices, apparentFaces);
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
    vector<Faces> apparentFaces = {Front, Top, Right, Bottom, Left, Back};
    recursiveSubdivide(depth, parallelepiped, vertices, indices, apparentFaces);
}

void computeSpongeNormals(const vector<float> &vertices, const vector<uint32_t> &indices, vector<float> &normals) {
    normals.resize(vertices.size());
    for (uint32_t i = 0; i < indices.size(); i += 6) {
        glm::vec3 a(
                vertices[3 * indices[i]],
                vertices[3 * indices[i] + 1],
                vertices[3 * indices[i] + 2]
        );
        glm::vec3 b(
                vertices[3 * indices[i + 1]],
                vertices[3 * indices[i + 1] + 1],
                vertices[3 * indices[i + 1] + 2]
        );
        glm::vec3 c(
                vertices[3 * indices[i + 2]],
                vertices[3 * indices[i + 2] + 1],
                vertices[3 * indices[i + 2] + 2]
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
    //uint32_t maxIndex = *max_element(indices.begin(), indices.end());

    map<uint32_t, uint8_t> count;
    for (uint32_t &index: indices) {
        if (count.find(index) == count.end()) count[index] = 1;
        else {
            ++count[index];
            newVertices.push_back(vertices[3 * index]);
            newVertices.push_back(vertices[3 * index + 1]);
            newVertices.push_back(vertices[3 * index + 2]);
            //index = ++maxIndex;
            index = (vertices.size() + newVertices.size()) / 3 - 1;
        }
    }
    vertices.insert(vertices.end(), newVertices.begin(), newVertices.end());
}
