#ifndef FRACTALS_PLATONIC4D_SPONGE_H
#define FRACTALS_PLATONIC4D_SPONGE_H

#include <cstdint>
#include <cmath>
#include <glm/glm.hpp>
#include <vector>

static void addLine(const std::vector<float>& line, std::vector<float>& result){

    result.push_back(line[0]);
    result.push_back(line[1]);
    result.push_back(line[2]);

    result.push_back(line[0] + glm::abs(line[0] - line[3]) / 3.0f);
    result.push_back(line[1] + glm::abs(line[1] - line[4]) / 3.0f);
    result.push_back(line[2] + glm::abs(line[2] - line[5]) / 3.0f);

    result.push_back(line[0] + glm::abs(line[0] - line[3]) * 2.0f / 3.0f);
    result.push_back(line[1] + glm::abs(line[1] - line[4]) * 2.0f / 3.0f);
    result.push_back(line[2] + glm::abs(line[2] - line[5]) * 2.0f / 3.0f);

    result.push_back(line[3]);
    result.push_back(line[4]);
    result.push_back(line[5]);
}

static void addSquare(const std::vector<float>& square, std::vector<float>& result) {
    {
        std::vector<float> line(square.begin(), square.begin() + 6);
        addLine(line, result);
    }

    for (uint8_t i = 1; i < 3; ++i) {
        float startOfLineX, startOfLineY, startOfLineZ;
        startOfLineX = square[0] + glm::abs(square[0] - square[6]) * (float) i / 3.0f;
        startOfLineY = square[1] + glm::abs(square[1] - square[7]) * (float) i / 3.0f;
        startOfLineZ = square[2] + glm::abs(square[2] - square[8]) * (float) i / 3.0f;

        float endOfLineX, endOfLineY, endOfLineZ;
        endOfLineX = square[3] + glm::abs(square[3] - square[9]) * (float) i / 3.0f;
        endOfLineY = square[4] + glm::abs(square[4] - square[10]) * (float) i / 3.0f;
        endOfLineZ = square[5] + glm::abs(square[5] - square[11]) * (float) i / 3.0f;

        std::vector<float> line;
        line.push_back(startOfLineX);
        line.push_back(startOfLineY);
        line.push_back(startOfLineZ);
        line.push_back(endOfLineX);
        line.push_back(endOfLineY);
        line.push_back(endOfLineZ);
        addLine(line, result);
    }

    {
        std::vector<float> line(square.begin() + 6, square.begin() + 12);
        addLine(line, result);
    }
}

/**
 * Subdivide the given cube in a Mender Sponge, stopping at the given rank
 * @param rank the rank when to stop subdivision. 0 is a basic cube
 * @param cube cube's coordinates (first face, then second face (the first vertex of the second face must be
 * adjacent to the first vertex of the first face and so one)
 * @return the array of vertices describing the Mender Sponge demanded
 */
void subdivide(uint8_t rank, const std::vector<float>& cube, std::vector<float> &vertices, std::vector<unsigned int> &indices){
    {
        std::vector<float> square(cube.begin(), cube.begin() + 12);
        addSquare(square, vertices);
    }

    for (uint8_t i = 1; i < 3; ++i) {
        float upLeftCornerX, upLeftCornerY, upLeftCornerZ;
        upLeftCornerX = cube[0] + glm::abs(cube[0] - cube[12]) * (float) i / 3.0f;
        upLeftCornerY = cube[1] + glm::abs(cube[1] - cube[13]) * (float) i / 3.0f;
        upLeftCornerZ = cube[2] + glm::abs(cube[2] - cube[14]) * (float) i / 3.0f;

        float upRightCornerX, upRightCornerY, upRightCornerZ;
        upRightCornerX = cube[3] + glm::abs(cube[3] - cube[15]) * (float) i / 3.0f;
        upRightCornerY = cube[4] + glm::abs(cube[4] - cube[16]) * (float) i / 3.0f;
        upRightCornerZ = cube[5] + glm::abs(cube[5] - cube[17]) * (float) i / 3.0f;

        float downLeftCornerX, downLeftCornerY, downLeftCornerZ;
        downLeftCornerX = cube[6] + glm::abs(cube[6] - cube[18]) * (float) i / 3.0f;
        downLeftCornerY = cube[7] + glm::abs(cube[7] - cube[19]) * (float) i / 3.0f;
        downLeftCornerZ = cube[8] + glm::abs(cube[8] - cube[20]) * (float) i / 3.0f;

        float downRightCornerX, downRightCornerY, downRightCornerZ;
        downRightCornerX = cube[9] + glm::abs(cube[9] -cube[21]) * (float) i / 3.0f;
        downRightCornerY = cube[10] + glm::abs(cube[10] - cube[22]) * (float) i / 3.0f;
        downRightCornerZ = cube[11] + glm::abs(cube[11] - cube[23]) * (float) i / 3.0f;

        std::vector<float> square;
        square.push_back(upLeftCornerX);
        square.push_back(upLeftCornerY);
        square.push_back(upLeftCornerZ);
        square.push_back(upRightCornerX);
        square.push_back(upRightCornerY);
        square.push_back(upRightCornerZ);
        square.push_back(downLeftCornerX);
        square.push_back(downLeftCornerY);
        square.push_back(downLeftCornerZ);
        square.push_back(downRightCornerX);
        square.push_back(downRightCornerY);
        square.push_back(downRightCornerZ);
        addSquare(square, vertices);
    }

    {
        std::vector<float> square(cube.begin() + 12, cube.begin() + 24);
        addSquare(square, vertices);
    }

    indices = {0,3,4,
                                4,3,7,
                                4,5,8,
                                8,5,9,
                                6,7,10,
                                10,7,11,
                                8,11,12,
                                12,11,15,

                                48,51,32,
                                32,51,35,
                                32,33,16,
                                16,33,17,
                                34,35,18,
                                18,35,19,
                                16,19,0,
                                0,19,3,

                                3,51,7,
                                7,51,55,
                                7,23,11,
                                11,23,27,
                                39,55,43,
                                43,55,59,
                                11,59,15,
                                15,59,63,

                                12,15,28,
                                28,15,31,
                                28,29,44,
                                44,29,45,
                                30,31,46,
                                46,31,47,
                                44,47,60,
                                60,47,63,

                                48,0,52,
                                52,0,4,
                                52,36,56,
                                56,36,40,
                                20,4,24,
                                24,4,8,
                                56,8,60,
                                60,8,12,

                                60,63,56,
                                56,63,59,
                                56,57,52,
                                52,57,53,
                                58,59,54,
                                54,59,55,
                                52,55,48,
                                48,55,51,

                                5,21,9,
                                9,21,25,
                                9,25,10,
                                10,25,26,
                                10,26,6,
                                6,26,22,
                                6,22,5,
                                5,22,21,

                                23,22,27,
                                27,22,26,
                                27,26,43,
                                43,26,42,
                                43,42,39,
                                39,42,38,
                                39,38,23,
                                23,38,22,

                                29,25,45,
                                45,25,41,
                                45,41,46,
                                46,41,42,
                                46,42,30,
                                30,42,26,
                                30,26,29,
                                29,26,25,

                                57,41,53,
                                53,41,37,
                                53,37,54,
                                54,37,42,
                                54,42,58,
                                58,42,42,
                                58,42,57,
                                57,42,41,

                                36,37,40,
                                40,37,41,
                                40,41,24,
                                24,41,25,
                                24,25,20,
                                20,25,21,
                                20,21,36,
                                36,21,37,

                                33,37,17,
                                17,37,21,
                                17,21,18,
                                18,21,22,
                                18,22,34,
                                34,22,38,
                                34,38,33,
                                33,38,37,
    };
}

static uint64_t getNumberOfCubes(int8_t rank){
    if (rank == - 1){
        return 0;
    } else {
        return pow(20, rank);
    }
}

static uint64_t getNumberOfVertices(uint8_t rank){
    return getNumberOfCubes(rank) * 32 + 8;
}

#endif //FRACTALS_PLATONIC4D_SPONGE_H
