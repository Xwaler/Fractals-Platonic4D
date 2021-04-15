#include <cstdint>
#include <cmath>
#include <glm/glm.hpp>
#include <vector>

#include "../headers/sponge.h"

using namespace std;

/**
 * Subdivide the given line into four equidistant points
 * @param line is a vector containing two points
 * @param result is an empty vector where the subdivision will be written to
 */
static void subdivideLine(const vector<float> &line, vector<float> &result) {
    /* Add the start of the line to the result */
    result.push_back(line[0]);
    result.push_back(line[1]);
    result.push_back(line[2]);

    /* Add a point one third of the way between the two points that describe the line */
    result.push_back(line[0] - (line[0] - line[3]) / 3.0f);
    result.push_back(line[1] - (line[1] - line[4]) / 3.0f);
    result.push_back(line[2] - (line[2] - line[5]) / 3.0f);

    /* Add a point two thirds of the way between the two points that describe the line */
    result.push_back(line[0] - (line[0] - line[3]) * 2.0f / 3.0f);
    result.push_back(line[1] - (line[1] - line[4]) * 2.0f / 3.0f);
    result.push_back(line[2] - (line[2] - line[5]) * 2.0f / 3.0f);

    /* Add the end of the line to the result vector */
    result.push_back(line[3]);
    result.push_back(line[4]);
    result.push_back(line[5]);
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
        startOfLineX = quadrilateral[0] - (quadrilateral[0] - quadrilateral[6]) * (float) i / 3.0f;
        startOfLineY = quadrilateral[1] - (quadrilateral[1] - quadrilateral[7]) * (float) i / 3.0f;
        startOfLineZ = quadrilateral[2] - (quadrilateral[2] - quadrilateral[8]) * (float) i / 3.0f;

        float endOfLineX, endOfLineY, endOfLineZ;
        endOfLineX = quadrilateral[3] - (quadrilateral[3] - quadrilateral[9]) * (float) i / 3.0f;
        endOfLineY = quadrilateral[4] - (quadrilateral[4] - quadrilateral[10]) * (float) i / 3.0f;
        endOfLineZ = quadrilateral[5] - (quadrilateral[5] - quadrilateral[11]) * (float) i / 3.0f;

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
        upLeftCornerX = parallelepiped[0] - (parallelepiped[0] - parallelepiped[12]) * (float) i / 3.0f;
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

    subdivideParallelepiped(parallelepiped, vertices);

    indices = {0, 3, 4,
               4, 3, 7,
               4, 5, 8,
               8, 5, 9,
               6, 7, 10,
               10, 7, 11,
               8, 11, 12,
               12, 11, 15,

               48, 51, 32,
               32, 51, 35,
               32, 33, 16,
               16, 33, 17,
               34, 35, 18,
               18, 35, 19,
               16, 19, 0,
               0, 19, 3,

               3, 51, 7,
               7, 51, 55,
               7, 23, 11,
               11, 23, 27,
               39, 55, 43,
               43, 55, 59,
               11, 59, 15,
               15, 59, 63,

               12, 15, 28,
               28, 15, 31,
               28, 29, 44,
               44, 29, 45,
               30, 31, 46,
               46, 31, 47,
               44, 47, 60,
               60, 47, 63,

               48, 0, 52,
               52, 0, 4,
               52, 36, 56,
               56, 36, 40,
               20, 4, 24,
               24, 4, 8,
               56, 8, 60,
               60, 8, 12,

               60, 63, 56,
               56, 63, 59,
               56, 57, 52,
               52, 57, 53,
               58, 59, 54,
               54, 59, 55,
               52, 55, 48,
               48, 55, 51,

               5, 21, 9,
               9, 21, 25,
               9, 25, 10,
               10, 25, 26,
               10, 26, 6,
               6, 26, 22,
               6, 22, 5,
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
               33, 38, 37,
    };
}

void getSpongeNormals(const vector<float> &vertices, const vector<uint32_t> &indices, vector<float> &normals) {
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
