#ifndef FRACTALS_PLATONIC4D_MENUPROPERTIES_H
#define FRACTALS_PLATONIC4D_MENUPROPERTIES_H

class MenuProperties {
public:
    static float backgroundColors[4];
    static float gaugeColors[4];
    static float cursorColors[4];
    static float tabBoundariesColors[4];
    const static uint16_t width = 960;
    const static uint16_t height = 540;

    static bool isConstrained(uint16_t abscissa, uint16_t ordinate) {
        return abscissa >= 0 && abscissa < width && ordinate >= 0 && ordinate < height;
    }
};

#endif //FRACTALS_PLATONIC4D_MENUPROPERTIES_H
