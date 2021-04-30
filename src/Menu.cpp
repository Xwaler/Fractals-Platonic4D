#include "../headers/Menu.h"

float MenuProperties::backgroundColors[4] = {0.0f, 0.0f, 0.0f, 0.0f};
float MenuProperties::gaugeColors[4] = {0.9f, 0.9f, 0.9f, 1.0f};
float MenuProperties::cursorColors[4] = {0.1f, 0.1f, 0.1f, 1.0f};
float MenuProperties::tabBoundariesColors[4] = {0.1f, 0.1f, 0.1f, 1.0f};

static void writeLetters(std::vector<float> &appearance, const std::string& letters, uint16_t abscissa, uint16_t ordinate) {
    uint16_t letterAbscissa = abscissa;
    for (char letter: letters) {
        uint8_t *pixels = font[letter];
        for (uint8_t y = 0; y < 8; ++y) {
            for (uint8_t x = 0; x < 8; ++x) {
                if ((pixels[y] >> x) & 1) {
                    memcpy(appearance.data() + ((ordinate + y) * MenuProperties::width + letterAbscissa + x) * 4, fontColor, 4 * sizeof(float));
                }
            }
        }
        letterAbscissa += 8;
    }
}

void ShapesDrawer::drawDiamond(std::vector<float> &appearance, uint16_t abscissa, uint16_t ordinate,
                        uint16_t semiDiagonalLength, const float colors[4]){
    for (uint16_t i = 0; i <= semiDiagonalLength; ++i) {
        for (uint16_t j = 0; j <= semiDiagonalLength - i; ++j){
            memcpy(appearance.data() + ((ordinate + i) * MenuProperties::width + abscissa + j) * 4, colors, 4 * sizeof(float));
            memcpy(appearance.data() + ((ordinate + i) * MenuProperties::width + abscissa - j) * 4, colors, 4 * sizeof(float));
        }
    }
}

void ShapesDrawer::drawRectangle(std::vector<float> &appearance, uint16_t topLeftAbscissa, uint16_t topLeftOrdinate,
                          uint16_t bottomRightAbscissa, uint16_t bottomRightOrdinate, const float colors[4]){
    for (uint16_t i = topLeftOrdinate; i <= bottomRightOrdinate; ++i) {
        for (uint16_t j = topLeftAbscissa; j <= bottomRightAbscissa; ++j) {
            memcpy(appearance.data() + ((i) * MenuProperties::width + j) * 4, colors, 4 * sizeof(float));
        }
    }
}

void Cursor::draw() {
    ShapesDrawer::drawDiamond(appearance, abscissa, ordinate, semiDiagonalLength, MenuProperties::cursorColors);
}

void Cursor::restoreBackground() {
    ShapesDrawer::drawDiamond(appearance, abscissa, ordinate, semiDiagonalLength, MenuProperties::backgroundColors);
    ShapesDrawer::drawRectangle(
            appearance, abscissa - semiDiagonalLength, ordinate, abscissa + semiDiagonalLength,
            ordinate + semiDiagonalLength, MenuProperties::gaugeColors
    );
}

Cursor::Cursor(std::vector<float> &appearance, uint16_t abscissa, uint16_t ordinate, uint16_t trackLength, uint16_t semiDiagonalLength, float defaultValue)
        : appearance(appearance), abscissa(abscissa + semiDiagonalLength + (uint16_t) ((float) (trackLength - 2 * semiDiagonalLength) * defaultValue)), ordinate(ordinate),
          startAbscissaTrack(abscissa + semiDiagonalLength), endAbscissaTrack(abscissa + trackLength - semiDiagonalLength),
          semiDiagonalLength(semiDiagonalLength) {
    draw();
}

void Cursor::move(uint16_t newAbscissa) {
    restoreBackground();
    abscissa = glm::max(glm::min(newAbscissa, endAbscissaTrack), startAbscissaTrack);
    draw();
}

float Cursor::getValue() const {
    return (float) (abscissa - startAbscissaTrack) / (float) (endAbscissaTrack - startAbscissaTrack);
}

bool Cursor::contains(uint16_t x, uint16_t y) const {
    return glm::pow(x - abscissa, 2) + glm::pow(y - ordinate - semiDiagonalLength / 2, 2) <= glm::pow(semiDiagonalLength, 2);
}

void Menu::drawGauges() {
    for (auto & gaugeProperty : gaugeProperties) {
        ShapesDrawer::drawRectangle(
                appearance,
                gaugeProperty[0], gaugeProperty[1],
                gaugeProperty[0] + gaugeProperty[2], gaugeProperty[1] + gaugeProperty[3],
                MenuProperties::gaugeColors
        );
    }
}

void Menu::writeGaugeText() {
    for (uint16_t i = 0; i < Gauges::GAUGE_NUMBER; ++i) {
        std::string text;
        switch (i) {
            case Gauges::TRANSPARENCY_OUTER: text = "Outer transparency"; break;
            case Gauges::TRANSPARENCY_INNER: text = "Inner transparency"; break;
            default: break;
        }
        writeLetters(appearance, text, gaugeProperties[i][0], gaugeProperties[i][1] - 10);
    }
}

void Menu::createCursors() {
    for (auto & gaugeProperty : gaugeProperties) {
        cursors.emplace_back(
                appearance, gaugeProperty[0], gaugeProperty[1],
                gaugeProperty[2], gaugeProperty[3],
                (float) gaugeProperty[4] / 100.0f
        );
    }
}

Menu::Menu() : appearance(MenuProperties::width * MenuProperties::height * 4, 0.0f), cursors(),
         isLeftTabUp(false) {
    drawGauges();
    writeGaugeText();
    createCursors();
}

std::vector<float>& Menu::getAppearance() {
    return appearance;
}

float Menu::getGaugeValue(Gauges ID) {
    return cursors[ID].getValue();
}

bool Menu::isInputCaptured() {
    return selected != nullptr;
}

void Menu::handleKeyPress(uint32_t action, uint16_t abscissa, uint16_t ordinate) {
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

void Menu::handleMouseMovement(uint16_t abscissa, uint16_t ordinate) {
    if (isInputCaptured()) {
        if (MenuProperties::isConstrained(abscissa, 0)) {
            selected->move(abscissa);
        }
    }
}
