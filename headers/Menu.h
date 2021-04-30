#ifndef FRACTALS_PLATONIC4D_MENU_H
#define FRACTALS_PLATONIC4D_MENU_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>
#include <cstdint>
#include <cstring>
#include <iostream>

#include "MenuProperties.h"
#include "font.h"

enum Gauges {
    TRANSPARENCY_OUTER = 0,
    TRANSPARENCY_INNER = 1,
    GAUGE_NUMBER = 2,
};

class ShapesDrawer {
public:
    static void drawDiamond(std::vector<float> &appearance, uint16_t abscissa, uint16_t ordinate,
                     uint16_t semiDiagonalLength, const float colors[4]);

    static void drawRectangle(std::vector<float> &appearance, uint16_t topLeftAbscissa, uint16_t topLeftOrdinate,
                              uint16_t bottomRightAbscissa, uint16_t bottomRightOrdinate, const float colors[4]);
};

class Cursor {
private:
    std::vector<float> &appearance;
    uint16_t abscissa;
    uint16_t ordinate;
    uint16_t semiDiagonalLength;
    uint16_t startAbscissaTrack;
    uint16_t endAbscissaTrack;

    void draw();

    void restoreBackground();

public:
    Cursor(std::vector<float> &appearance, uint16_t abscissa, uint16_t ordinate, uint16_t trackLength, uint16_t semiDiagonalLength, float defaultValue);

    void move(uint16_t newAbscissa);

    float getValue() const;

    bool contains(uint16_t x, uint16_t y) const;
};

class Menu {
public:
    bool isLeftTabUp;
private:
    std::vector<float> appearance;
    std::vector<Cursor> cursors;
    Cursor* selected = nullptr;
    uint16_t gaugeProperties[Gauges::GAUGE_NUMBER][5] {
            /* top left abscissa, top left ordinate, bottom right abscissa, bottom right ordinate, default cursor value in percents */
            { 10, 20, 144, 10, 70 },
            { 10, 50, 144, 10, 100 },
    };

public:
    Menu();

    std::vector<float>& getAppearance();

    float getGaugeValue(Gauges ID);

    bool isInputCaptured();

    void handleKeyPress(uint32_t action, uint16_t abscissa, uint16_t ordinate);

    void handleMouseMovement(uint16_t abscissa, uint16_t ordinate);
private:
    void drawGauges();

    void writeGaugeText();

    void createCursors();
};

#endif //FRACTALS_PLATONIC4D_MENU_H
