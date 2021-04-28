#ifndef FRACTALS_PLATONIC4D_MENU_H
#define FRACTALS_PLATONIC4D_MENU_H


#include <vector>
#include <cstdint>
#include <cstring>

class Constants{
public:
    constexpr static float backgroundColors[4] = {0.6, 0.6, 0.6, 0.75};
    constexpr static float gaugeColors[4] = {0.9, 0.9, 0.9, 1};
    constexpr static float cursorColors[4] = {0.1, 0.1, 0.1, 1};
    constexpr static float tabBoundariesColors[4] = {0.1, 0.1, 0.1, 1};
    const static uint16_t width = 640;
    const static uint16_t height = 360;
};

class ShapesDrawer{
public:
    static void drawDiamond(std::vector<float> &appearance, uint16_t abscissa, uint16_t ordinate,
                     uint16_t semiDiagonalLength, const float colors[4]){
        for (uint16_t i = 0; i <= semiDiagonalLength; ++i) {
            for (uint16_t j = 0; j <= semiDiagonalLength - i; ++j){
                memcpy(appearance.data() + ((ordinate + i) * Constants::width + abscissa + j) * 4, colors, 4);
                memcpy(appearance.data() + ((ordinate + i) * Constants::width + abscissa - j) * 4, colors, 4);
            }
        }
    }

    static void drawRectangle(std::vector<float> &appearance, uint16_t topLeftAbscissa, uint16_t topLeftOrdinate,
                              uint16_t bottomRightAbscissa, uint16_t bottomRightOrdinate, const float colors[4]){
        for (uint16_t i = topLeftOrdinate; i <= bottomRightOrdinate; ++i) {
            for (uint16_t j = topLeftAbscissa; j <= bottomRightAbscissa; ++j) {
                memcpy(appearance.data() + ((i) * Constants::width + j) * 4, colors, 4);
            }
        }
    }
};

class Cursor {
    uint16_t abscissa;
    uint16_t ordinate;
    uint16_t semiDiagonalLength;
    std::vector<float> colors;

    Cursor(uint16_t abscissa, uint16_t ordinate, uint16_t semiDiagonalLength, float red, float green, float blue, float alpha)
            : abscissa(abscissa), ordinate(ordinate), semiDiagonalLength(semiDiagonalLength) {
        colors = {red, green, blue, alpha};
    }

    void move(std::vector<float> &appearance, uint16_t newAbscissa, uint16_t newOrdinate) {
        restoreBackground(appearance);
        ShapesDrawer::drawDiamond(appearance, newAbscissa, newOrdinate, semiDiagonalLength, Constants::cursorColors);
    }

    void restoreBackground(std::vector<float> &appearance){
        ShapesDrawer::drawDiamond(appearance, abscissa, ordinate, semiDiagonalLength, Constants::backgroundColors);
        ShapesDrawer::drawRectangle(appearance, abscissa - semiDiagonalLength, ordinate, abscissa + semiDiagonalLength, ordinate, Constants::gaugeColors);
    }
};

class Menu {
    std::vector<float> appearance;
    std::vector<Cursor> cursors;
    bool isLeftTabUp;

    Menu() : appearance(640 * 360 * 4), cursors(), isLeftTabUp(false){

    }


};


#endif //FRACTALS_PLATONIC4D_MENU_H
