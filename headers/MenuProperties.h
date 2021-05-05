#ifndef FRACTALS_PLATONIC4D_MENUPROPERTIES_H
#define FRACTALS_PLATONIC4D_MENUPROPERTIES_H

class MenuProperties {
public:
    static float backgroundColors[4];
    static float gaugeColors[4];
    static float cursorColors[4];
    static float buttonColors[4];
    const static uint16_t width = 1280;
    const static uint16_t height = 720;

    /**
     * Return true if a position is inside the menu (meaning the texture)
     * @param abscissa
     * @param ordinate
     * @return
     */
    static bool isConstrained(uint16_t abscissa, uint16_t ordinate) {
        return abscissa >= 0 && abscissa < width && ordinate >= 0 && ordinate < height;
    }
};

#endif //FRACTALS_PLATONIC4D_MENUPROPERTIES_H
