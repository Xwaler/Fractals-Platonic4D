#ifndef FRACTALS_PLATONIC4D_HYPERCUBE_H
#define FRACTALS_PLATONIC4D_HYPERCUBE_H

#include <vector>

using namespace std;

/**
 * Interface providing hypercube points and rotation methods
 */
class Hypercube {
protected:
    const vector<glm::vec4> baseHypercubePoints = {
            glm::vec4(-1.0f, -1.0f, -1.0f, -1.0f), glm::vec4(1.0f, -1.0f, -1.0f, -1.0f),
            glm::vec4(-1.0f, 1.0f, -1.0f, -1.0f), glm::vec4(1.0f, 1.0f, -1.0f, -1.0f),
            glm::vec4(-1.0f, -1.0f, 1.0f, -1.0f), glm::vec4(1.0f, -1.0f, 1.0f, -1.0f),
            glm::vec4(-1.0f, 1.0f, 1.0f, -1.0f), glm::vec4(1.0f, 1.0f, 1.0f, -1.0f),

            glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(1.0f, -1.0f, -1.0f, 1.0f),
            glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, -1.0f, 1.0f),
            glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f), glm::vec4(1.0f, -1.0f, 1.0f, 1.0f),
            glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
    };
    vector<glm::vec4> hypercubePoints = baseHypercubePoints;
    vector<glm::vec3> projectedHypercubePoints;

    /**
     * Resets the hypercube point buffer
     */
    void init4DRotations() {
        hypercubePoints = baseHypercubePoints;
    }

    /**
     * Rotate hypercube points around XY axis
     * @param angle
     */
    void rotateXY(float angle) {
        const glm::mat4 rot({
            glm::cos(angle), glm::sin(angle), 0, 0,
            -glm::sin(angle), glm::cos(angle), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1,
        });
        for (glm::vec4 &p: hypercubePoints) {
            p = rot * p;
        }
    }

    /**
     * Rotate hypercube points around YZ axis
     * @param angle
     */
    void rotateYZ(float angle) {
        const glm::mat4 rot({
                1, 0, 0, 0,
                0, glm::cos(angle), glm::sin(angle), 0,
                0, -glm::sin(angle), glm::cos(angle), 0,
                0, 0, 0, 1,
        });
        for (glm::vec4 &p: hypercubePoints) {
            p = rot * p;
        }
    }

    /**
     * Rotate hypercube points around ZX axis
     * @param angle
     */
    void rotateZX(float angle) {
        const glm::mat4 rot({
                glm::cos(angle), 0, -glm::sin(angle), 0,
                0, 1, 0, 0,
                glm::sin(angle), 0, glm::cos(angle), 0,
                0, 0, 0, 1,
        });
        for (glm::vec4 &p: hypercubePoints) {
            p = rot * p;
        }
    }

    /**
     * Rotate hypercube points around XW axis
     * @param angle
     */
    void rotateXW(float angle) {
        const glm::mat4 rot({
                glm::cos(angle), 0, 0, glm::sin(angle),
                0, 1, 0, 0,
                0, 0, 1, 0,
                -glm::sin(angle), 0, 0, glm::cos(angle),
        });
        for (glm::vec4 &p: hypercubePoints) {
            p = rot * p;
        }
    }

    /**
     * Rotate hypercube points around YW axis
     * @param angle
     */
    void rotateYW(float angle) {
        const glm::mat4 rot({
                1, 0, 0, 0,
                0, glm::cos(angle), 0, -glm::sin(angle),
                0, 0, 1, 0,
                0, glm::sin(angle), 0, glm::cos(angle),
        });
        for (glm::vec4 &p: hypercubePoints) {
            p = rot * p;
        }
    }

    /**
     * Rotate hypercube points around ZW axis
     * @param angle
     */
    void rotateZW(float angle) {
        const glm::mat4 rot({
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, glm::cos(angle), -glm::sin(angle),
                0, 0, glm::sin(angle), glm::cos(angle),
        });
        for (glm::vec4 &p: hypercubePoints) {
            p = rot * p;
        }
    }
};

#endif //FRACTALS_PLATONIC4D_HYPERCUBE_H
