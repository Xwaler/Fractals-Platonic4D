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
    TRANSPARENCY_PX = 0,
    TRANSPARENCY_NX = 1,
    TRANSPARENCY_PY = 2,
    TRANSPARENCY_NY = 3,
    TRANSPARENCY_PZ = 4,
    TRANSPARENCY_NZ = 5,
    TRANSPARENCY_PW = 6,
    TRANSPARENCY_NW = 7,
    ROTATION_XY = 8,
    ROTATION_YZ = 9,
    ROTATION_ZX = 10,
    ROTATION_XW = 11,
    ROTATION_YW = 12,
    ROTATION_ZW = 13,
    GAUGE_NUMBER = 14,
};

class ShapesDrawer {
public:
    static void drawDiamond(std::vector<float> &appearance, uint16_t abscissa, uint16_t ordinate,
                     uint16_t semiDiagonalLength, const float colors[4]);

    static void drawRectangle(std::vector<float> &appearance, uint16_t topLeftAbscissa, uint16_t topLeftOrdinate,
                              uint16_t bottomRightAbscissa, uint16_t bottomRightOrdinate, const float colors[4]);
};

class Cursor {
public:
    bool triggerVerticesReload;
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
    Cursor(std::vector<float> &appearance, uint16_t abscissa, uint16_t ordinate, uint16_t trackLength, uint16_t semiDiagonalLength, float defaultValue, bool triggerVerticesReload);

    bool willMove(uint16_t newAbscissa) const;

    void move(uint16_t newAbscissa);

    float getValue() const;

    bool contains(uint16_t x, uint16_t y) const;
};

class Menu {
public:
    bool isLeftTabUp;
    bool rotationWasModified = false;
private:
    std::vector<float> appearance;
    std::vector<Cursor> cursors;
    Cursor* selected = nullptr;
    uint16_t gaugeProperties[Gauges::GAUGE_NUMBER][6] {
            /* top left abscissa, top left ordinate, width, height, default cursor value in percents, bool must reload vertices */
            { 10, 20,  200, 10,  70, 0},
            { 10, 50,  200, 10,  70, 0 },
            { 10, 80,  200, 10,  70, 0 },
            { 10, 110, 200, 10,  70, 0 },
            { 10, 140, 200, 10,  70, 0 },
            { 10, 170, 200, 10,  70, 0 },
            { 10, 200, 200, 10,   0, 0 },
            { 10, 230, 200, 10,  70, 0 },

            { MenuProperties::width - 210, 20,  200, 10, 0, 1 },
            { MenuProperties::width - 210, 50,  200, 10, 0, 1 },
            { MenuProperties::width - 210, 80,  200, 10, 0, 1 },
            { MenuProperties::width - 210, 110, 200, 10, 0, 1 },
            { MenuProperties::width - 210, 140, 200, 10, 0, 1 },
            { MenuProperties::width - 210, 170, 200, 10, 0, 1 },
    };
    uint16_t resetButtonProperties[4] {
        MenuProperties::width - 70, MenuProperties::height - 40, 50, 20,
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

    void drawResetButton();
};

#endif //FRACTALS_PLATONIC4D_MENU_H
