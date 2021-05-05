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

/**
 * ID of the gauges
 */
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

/**
 * Draw shapes to a texture
 */
class ShapesDrawer {
public:
    /**
     * Draw an equilateral triangle
     * @param texture
     * @param abscissa
     * @param ordinate
     * @param semiDiagonalLength
     * @param colors
     */
    static void drawEquilateralTriangle(std::vector<float> &texture, uint16_t abscissa, uint16_t ordinate,
                                        uint16_t semiDiagonalLength, const float *colors);

    /**
     * Draw a rectangle
     * @param texture
     * @param topLeftAbscissa
     * @param topLeftOrdinate
     * @param bottomRightAbscissa
     * @param bottomRightOrdinate
     * @param colors
     */
    static void drawRectangle(std::vector<float> &texture, uint16_t topLeftAbscissa, uint16_t topLeftOrdinate,
                              uint16_t bottomRightAbscissa, uint16_t bottomRightOrdinate, const float colors[4]);
};

/**
 * Moving cursor
 */
class Cursor {
public:
    bool triggerVerticesReload;
private:
    std::vector<float> &texture;
    uint16_t abscissa;
    uint16_t ordinate;
    uint16_t semiDiagonalLength;
    uint16_t startAbscissaTrack;
    uint16_t endAbscissaTrack;

    /**
     * Draw to the texture
     */
    void draw();

    /**
     * Restore the original colors behind the cursor
     */
    void restoreBackground();

public:
    Cursor(std::vector<float> &texture, uint16_t abscissa, uint16_t ordinate, uint16_t trackLength, uint16_t semiDiagonalLength, float defaultValue, bool triggerVerticesReload);

    /**
     * Checks if a new position will trigger a movement
     * @param newAbscissa
     * @return
     */
    bool willMove(uint16_t newAbscissa) const;

    /**
     * Move the cursor to a new abscissa
     * @param newAbscissa
     */
    void move(uint16_t newAbscissa);

    /**
     * Compute the ratio between the current position and it's extrema
     * @return
     */
    float getValue() const;

    /**
     * A position is inside the cursor's hit-box
     * @param x
     * @param y
     * @return
     */
    bool contains(uint16_t x, uint16_t y) const;
};

class Menu {
public:
    bool rotationWasModified = false;
private:
    std::vector<float> texture;
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
    std::string gaugeTexts[Gauges::GAUGE_NUMBER] {
            "PX transparency",
            "NX transparency",
            "PY transparency",
            "NY transparency",
            "PZ transparency",
            "NZ transparency",
            "PW transparency",
            "NW transparency",

            "XY rotation",
            "YZ rotation",
            "ZX rotation",
            "XW rotation",
            "YW rotation",
            "ZW rotation",
    };
    std::vector<uint16_t> resetButtonProperties {
            MenuProperties::width - 60, MenuProperties::height - 30, 50, 20,
    };
    std::vector<std::vector<uint16_t>> keysTooltipPositions {
            {10, MenuProperties::height - 18},
    };
    std::vector<std::string> keysTooltipTexts {
            "Press Space to toggle wire mesh mode",
    };

public:
    Menu();

    /**
     * @return texture
     */
    std::vector<float>& getTexture();

    /**
     * Get a gauge's normalized value
     * @param ID
     * @return
     */
    float getGaugeValue(Gauges ID);

    /**
     * Return true if a cursor is currently selected
     * @return
     */
    bool isInputCaptured();

    /**
     * Handler for key presses
     * @param action
     * @param abscissa
     * @param ordinate
     */
    void handleKeyPress(uint32_t action, uint16_t abscissa, uint16_t ordinate);

    /**
     * Handler for mouse movements
     * @param abscissa
     * @param ordinate
     */
    void handleMouseMovement(uint16_t abscissa, uint16_t ordinate);
private:
    /**
     * Draw all the gauges
     */
    void drawGauges();

    /**
     * Write each gauges text above it
     */
    void writeGaugeText();

    /**
     * Initialize cursors objects
     */
    void createCursors();

    /**
     * Draw the reset button
     */
    void drawResetButton();

    /**
     * Draw the key helper
     */
    void drawKeysTooltip();
};

#endif //FRACTALS_PLATONIC4D_MENU_H
