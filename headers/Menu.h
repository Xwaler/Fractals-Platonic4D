#ifndef FRACTALS_PLATONIC4D_MENU_H
#define FRACTALS_PLATONIC4D_MENU_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>
#include <cstdint>
#include <cstring>
#include <iostream>

class MenuConstants {
public:
    static float backgroundColors[4];
    static float gaugeColors[4];
    static float cursorColors[4];
    static float tabBoundariesColors[4];
    const static uint16_t width = 640;
    const static uint16_t height = 360;
};

class ShapesDrawer {
public:
    static void drawDiamond(std::vector<float> &appearance, uint16_t abscissa, uint16_t ordinate,
                     uint16_t semiDiagonalLength, const float colors[4]){
        for (uint16_t i = 0; i <= semiDiagonalLength; ++i) {
            for (uint16_t j = 0; j <= semiDiagonalLength - i; ++j){
                memcpy(appearance.data() + ((ordinate + i) * MenuConstants::width + abscissa + j) * 4, colors, 4 * sizeof(float));
                memcpy(appearance.data() + ((ordinate + i) * MenuConstants::width + abscissa - j) * 4, colors, 4 * sizeof(float));
            }
        }
    }

    static void drawRectangle(std::vector<float> &appearance, uint16_t topLeftAbscissa, uint16_t topLeftOrdinate,
                              uint16_t bottomRightAbscissa, uint16_t bottomRightOrdinate, const float colors[4]){
        for (uint16_t i = topLeftOrdinate; i <= bottomRightOrdinate; ++i) {
            for (uint16_t j = topLeftAbscissa; j <= bottomRightAbscissa; ++j) {
                memcpy(appearance.data() + ((i) * MenuConstants::width + j) * 4, colors, 4 * sizeof(float));
            }
        }
    }
};

class Cursor {
    uint16_t abscissa;
    uint16_t ordinate;
    uint16_t semiDiagonalLength;

public:
    Cursor(std::vector<float> &appearance, uint16_t abscissa, uint16_t ordinate, uint16_t semiDiagonalLength)
            : abscissa(abscissa), ordinate(ordinate), semiDiagonalLength(semiDiagonalLength) {
        draw(appearance);
    }

    void draw(std::vector<float> &appearance) {
        ShapesDrawer::drawDiamond(appearance, abscissa, ordinate, semiDiagonalLength, MenuConstants::cursorColors);
    }

    void move(std::vector<float> &appearance, uint16_t newAbscissa, uint16_t newOrdinate) {
        restoreBackground(appearance);
        abscissa = newAbscissa; ordinate = newOrdinate;
        draw(appearance);
    }

    void restoreBackground(std::vector<float> &appearance) {
        ShapesDrawer::drawDiamond(appearance, abscissa, ordinate, semiDiagonalLength, MenuConstants::backgroundColors);
        ShapesDrawer::drawRectangle(appearance, abscissa - semiDiagonalLength, ordinate, abscissa + semiDiagonalLength, ordinate, MenuConstants::gaugeColors);
    }

    bool contains(uint16_t x, uint16_t y) {
        return glm::pow(x - abscissa, 2) + glm::pow(y - ordinate - semiDiagonalLength / 2, 2) <= glm::pow(semiDiagonalLength, 2);
    }
};

class Menu {
    std::vector<Cursor> cursors;
    bool isLeftTabUp;

public:
    std::vector<float> appearance;
    bool isInputCaptured;

    Menu() : appearance(MenuConstants::width * MenuConstants::height * 4, 0.0f), cursors(), isLeftTabUp(false), isInputCaptured(false) {
        cursors.emplace_back(appearance, 100, 40, 5);
    }

    void handleKeyPress(uint32_t action, uint16_t abscissa, uint16_t ordinate) {
        if (action == GLFW_PRESS) {
            for (Cursor c: cursors) {
                if (c.contains(abscissa, ordinate)) {
                    isInputCaptured = true;
                }
            }
        } else if (action == GLFW_RELEASE) {
            isInputCaptured = false;
        }
    }

    void handleMouseMovement(uint16_t abscissa, uint16_t ordinate) {
        if (isInputCaptured) {
            std::cout << "Mouse press captured by overlay" << std::endl;
        }
    }
};

#endif //FRACTALS_PLATONIC4D_MENU_H
