#ifndef FRACTALS_PLATONIC4D_MENU_H
#define FRACTALS_PLATONIC4D_MENU_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>
#include <cstdint>
#include <cstring>
#include <iostream>

class MenuProperties {
public:
    static float backgroundColors[4];
    static float gaugeColors[4];
    static float cursorColors[4];
    static float tabBoundariesColors[4];
    const static uint16_t width = 640;
    const static uint16_t height = 360;

    static bool isConstrained(uint16_t abscissa, uint16_t ordinate) {
        return abscissa >= 0 && abscissa < width && ordinate >= 0 && ordinate < height;
    }
};

class ShapesDrawer {
public:
    static void drawDiamond(std::vector<float> &appearance, uint16_t abscissa, uint16_t ordinate,
                     uint16_t semiDiagonalLength, const float colors[4]){
        for (uint16_t i = 0; i <= semiDiagonalLength; ++i) {
            for (uint16_t j = 0; j <= semiDiagonalLength - i; ++j){
                memcpy(appearance.data() + ((ordinate + i) * MenuProperties::width + abscissa + j) * 4, colors, 4 * sizeof(float));
                memcpy(appearance.data() + ((ordinate + i) * MenuProperties::width + abscissa - j) * 4, colors, 4 * sizeof(float));
            }
        }
    }

    static void drawRectangle(std::vector<float> &appearance, uint16_t topLeftAbscissa, uint16_t topLeftOrdinate,
                              uint16_t bottomRightAbscissa, uint16_t bottomRightOrdinate, const float colors[4]){
        for (uint16_t i = topLeftOrdinate; i <= bottomRightOrdinate; ++i) {
            for (uint16_t j = topLeftAbscissa; j <= bottomRightAbscissa; ++j) {
                memcpy(appearance.data() + ((i) * MenuProperties::width + j) * 4, colors, 4 * sizeof(float));
            }
        }
    }
};

class Cursor {
private:
    std::vector<float> &appearance;
    uint16_t abscissa;
    uint16_t ordinate;
    uint16_t semiDiagonalLength;
    uint16_t startAbscissaTrack;
    uint16_t endAbscissaTrack;

    void draw() {
        ShapesDrawer::drawDiamond(appearance, abscissa, ordinate, semiDiagonalLength, MenuProperties::cursorColors);
    }

    void restoreBackground() {
        ShapesDrawer::drawDiamond(appearance, abscissa, ordinate, semiDiagonalLength, MenuProperties::backgroundColors);
        ShapesDrawer::drawRectangle(appearance, abscissa - semiDiagonalLength, ordinate, abscissa + semiDiagonalLength, ordinate + semiDiagonalLength, MenuProperties::gaugeColors);
    }

public:
    Cursor(std::vector<float> &appearance, uint16_t abscissa, uint16_t ordinate, uint16_t trackLength, uint16_t semiDiagonalLength)
            : appearance(appearance), abscissa(abscissa + semiDiagonalLength), ordinate(ordinate),
              startAbscissaTrack(abscissa + semiDiagonalLength), endAbscissaTrack(abscissa + trackLength - semiDiagonalLength),
              semiDiagonalLength(semiDiagonalLength) {
        draw();
    }

    void move(uint16_t newAbscissa) {
        restoreBackground();
        abscissa = glm::max(glm::min(newAbscissa, endAbscissaTrack), startAbscissaTrack);
        draw();
    }

    float getValue() {
        return (float) (abscissa - startAbscissaTrack) / (float) (endAbscissaTrack - startAbscissaTrack);
    }

    bool contains(uint16_t x, uint16_t y) {
        return glm::pow(x - abscissa, 2) + glm::pow(y - ordinate - semiDiagonalLength / 2, 2) <= glm::pow(semiDiagonalLength, 2);
    }
};

class Menu {
public:
    bool isLeftTabUp;
private:
    std::vector<float> appearance;
    std::vector<Cursor> cursors;
    Cursor* selected = nullptr;
    std::vector<std::vector<uint16_t>> gaugePositions = {
            { 10, 10, 60, 5 },
    };

    void drawGauges() {
        for (std::vector<uint16_t> gauge: gaugePositions) {
            ShapesDrawer::drawRectangle(
                    appearance,
                    gauge[0], gauge[1],
                    gauge[0] + gauge[2], gauge[1] + gauge[3],
                    MenuProperties::gaugeColors
            );
        }
    }

    void createCursors() {
        for (std::vector<uint16_t> gauge: gaugePositions) {
            cursors.emplace_back(appearance, gauge[0], gauge[1], gauge[2], gauge[3]);
        }
    }

public:
    Menu() : appearance(MenuProperties::width * MenuProperties::height * 4, 0.0f), cursors(),
             isLeftTabUp(false) {
        drawGauges();
        createCursors();
    }

    std::vector<float>& getAppearance() {
        return appearance;
    }

    bool isInputCaptured() {
        return selected != nullptr;
    }

    void handleKeyPress(uint32_t action, uint16_t abscissa, uint16_t ordinate) {
        if (action == GLFW_PRESS) {
            for (Cursor &c: cursors) {
                if (c.contains(abscissa, ordinate)) {
                    selected = &c;
                }
            }
        } else if (action == GLFW_RELEASE) {
            selected = nullptr;
        }
    }

    void handleMouseMovement(uint16_t abscissa, uint16_t ordinate) {
        if (isInputCaptured()) {
            if (MenuProperties::isConstrained(abscissa, 0)) {
                selected->move(abscissa);
                std::cout << selected->getValue() << std::endl;
            }
        }
    }
};

#endif //FRACTALS_PLATONIC4D_MENU_H
