#include "../headers/Menu.h"

float MenuProperties::backgroundColors[4] = {0.0f, 0.0f, 0.0f, 0.0f};
float MenuProperties::gaugeColors[4] = {0.9f, 0.9f, 0.9f, 1.0f};
float MenuProperties::cursorColors[4] = {0.1f, 0.1f, 0.1f, 1.0f};
float MenuProperties::buttonColors[4] = {0.5f, 0.5f, 0.5f, 1.0f};

/**
 * Draw every letter of a string using the font of font.h
 * @param texture
 * @param letters
 * @param abscissa
 * @param ordinate
 */
static void writeLetters(std::vector<float> &texture, const std::string& letters, uint16_t abscissa, uint16_t ordinate) {
    uint16_t letterAbscissa = abscissa;
    for (char letter: letters) {
        uint8_t *pixels = font[letter];
        for (uint8_t y = 0; y < 8; ++y) {
            for (uint8_t x = 0; x < 8; ++x) {
                if ((pixels[y] >> x) & 1) {
                    memcpy(texture.data() + ((ordinate + y) * MenuProperties::width + letterAbscissa + x) * 4, fontColor, 4 * sizeof(float));
                }
            }
        }
        letterAbscissa += 8;
    }
}

/**
 * Draw an equilateral triangle
 * @param texture
 * @param abscissa
 * @param ordinate
 * @param semiDiagonalLength
 * @param colors
 */
void ShapesDrawer::drawEquilateralTriangle(std::vector<float> &texture, uint16_t abscissa, uint16_t ordinate,
                                           uint16_t semiDiagonalLength, const float *colors) {
    for (uint16_t i = 0; i <= semiDiagonalLength; ++i) {
        for (uint16_t j = 0; j <= semiDiagonalLength - i; ++j){
            memcpy(texture.data() + ((ordinate + i) * MenuProperties::width + abscissa + j) * 4, colors, 4 * sizeof(float));
            memcpy(texture.data() + ((ordinate + i) * MenuProperties::width + abscissa - j) * 4, colors, 4 * sizeof(float));
        }
    }
}

/**
 * Draw a rectangle
 * @param texture
 * @param topLeftAbscissa
 * @param topLeftOrdinate
 * @param bottomRightAbscissa
 * @param bottomRightOrdinate
 * @param colors
 */
void ShapesDrawer::drawRectangle(std::vector<float> &texture, uint16_t topLeftAbscissa, uint16_t topLeftOrdinate,
                          uint16_t bottomRightAbscissa, uint16_t bottomRightOrdinate, const float colors[4]){
    for (uint16_t i = topLeftOrdinate; i <= bottomRightOrdinate; ++i) {
        for (uint16_t j = topLeftAbscissa; j <= bottomRightAbscissa; ++j) {
            memcpy(texture.data() + ((i) * MenuProperties::width + j) * 4, colors, 4 * sizeof(float));
        }
    }
}

/**
 * Draw to the texture
 */
void Cursor::draw() {
    ShapesDrawer::drawEquilateralTriangle(texture, abscissa, ordinate, semiDiagonalLength,
                                          MenuProperties::cursorColors);
}

/**
 * Restore the original colors behind the cursor
 */
void Cursor::restoreBackground() {
    ShapesDrawer::drawEquilateralTriangle(texture, abscissa, ordinate, semiDiagonalLength,
                                          MenuProperties::backgroundColors);
    ShapesDrawer::drawRectangle(
            texture, abscissa - semiDiagonalLength, ordinate, abscissa + semiDiagonalLength,
            ordinate + semiDiagonalLength, MenuProperties::gaugeColors
    );
}

Cursor::Cursor(std::vector<float> &texture, uint16_t abscissa, uint16_t ordinate, uint16_t trackLength, uint16_t semiDiagonalLength, float defaultValue, bool triggerVerticesReload)
        : texture(texture), abscissa(abscissa + semiDiagonalLength + (uint16_t) ((float) (trackLength - 2 * semiDiagonalLength) * defaultValue)), ordinate(ordinate),
          startAbscissaTrack(abscissa + semiDiagonalLength), endAbscissaTrack(abscissa + trackLength - semiDiagonalLength),
          semiDiagonalLength(semiDiagonalLength), triggerVerticesReload(triggerVerticesReload) {
    draw();
}

/**
 * Checks if a new position will trigger a movement
 * @param newAbscissa
 * @return
 */
bool Cursor::willMove(uint16_t newAbscissa) const {
    return newAbscissa != abscissa;
}

/**
 * Move the cursor to a new abscissa
 * @param newAbscissa
 */
void Cursor::move(uint16_t newAbscissa) {
    restoreBackground();
    abscissa = glm::max(glm::min(newAbscissa, endAbscissaTrack), startAbscissaTrack);
    draw();
}

/**
 * Compute the ratio between the current position and it's extrema
 * @return
 */
float Cursor::getValue() const {
    return (float) (abscissa - startAbscissaTrack) / (float) (endAbscissaTrack - startAbscissaTrack);
}

/**
 * A position is inside the cursor's hit-box
 * @param x
 * @param y
 * @return
 */
bool Cursor::contains(uint16_t x, uint16_t y) const {
    return glm::pow(x - abscissa, 2) + glm::pow(y - ordinate - semiDiagonalLength / 2, 2) <= glm::pow(semiDiagonalLength, 2);
}

/**
 * Draw all the gauges
 */
void Menu::drawGauges() {
    for (auto & gaugeProperty : gaugeProperties) {
        ShapesDrawer::drawRectangle(
                texture,
                gaugeProperty[0], gaugeProperty[1],
                gaugeProperty[0] + gaugeProperty[2], gaugeProperty[1] + gaugeProperty[3],
                MenuProperties::gaugeColors
        );
    }
}

/**
 * Write each gauges text above it
 */
void Menu::writeGaugeText() {
    for (uint16_t i = 0; i < Gauges::GAUGE_NUMBER; ++i) {
        writeLetters(texture, gaugeTexts[i],
                     gaugeProperties[i][0] + (gaugeProperties[i][2] - gaugeTexts[i].length() * 8) / 2,
                     gaugeProperties[i][1] - 10);
    }
}

/**
 * Initialize cursors objects
 */
void Menu::createCursors() {
    cursors.clear();
    for (auto & gaugeProperty : gaugeProperties) {
        cursors.emplace_back(
                texture, gaugeProperty[0], gaugeProperty[1],
                gaugeProperty[2], gaugeProperty[3],
                (float) gaugeProperty[4] / 100.0f,
                (bool) gaugeProperty[5]
        );
    }
}

/**
 * Draw the reset button
 */
void Menu::drawResetButton() {
    ShapesDrawer::drawRectangle(texture, resetButtonProperties[0], resetButtonProperties[1],
                                resetButtonProperties[0] + resetButtonProperties[2],
                                resetButtonProperties[1] + resetButtonProperties[3], MenuProperties::buttonColors);
    writeLetters(texture, "Reset", resetButtonProperties[0] + 6, resetButtonProperties[1] + 7);
}

void Menu::drawKeysTooltip() {
    for (uint16_t i = 0; i < (uint16_t) keysTooltipPositions.size(); ++i) {
        writeLetters(texture, keysTooltipTexts[i], keysTooltipPositions[i][0], keysTooltipPositions[i][1]);
    }
}

Menu::Menu() : texture(MenuProperties::width * MenuProperties::height * 4, 0.0f), cursors() {
    drawGauges();
    writeGaugeText();
    createCursors();
    drawResetButton();
    drawKeysTooltip();
}

/**
 * @return texture
 */
std::vector<float>& Menu::getTexture() {
    return texture;
}

/**
 * Get a gauge's normalized value
 * @param ID
 * @return
 */
float Menu::getGaugeValue(Gauges ID) {
    return cursors[ID].getValue();
}

/**
 * Return true if a cursor is currently selected
 * @return
 */
bool Menu::isInputCaptured() {
    return selected != nullptr;
}

/**
 * Handler for key presses
 * @param action
 * @param abscissa
 * @param ordinate
 */
void Menu::handleKeyPress(uint32_t action, uint16_t abscissa, uint16_t ordinate) {
    if (action == GLFW_PRESS) {
        for (Cursor &c: cursors) {
            if (c.contains(abscissa, ordinate)) {
                selected = &c;
            }
        }
        if (abscissa >= resetButtonProperties[0] && abscissa < resetButtonProperties[0] + resetButtonProperties[2] &&
            ordinate >= resetButtonProperties[1] && ordinate < resetButtonProperties[1] + resetButtonProperties[3]) {
            drawGauges();
            createCursors();
            rotationWasModified = true;
        }
    } else if (action == GLFW_RELEASE) {
        if (selected != nullptr) {
            selected = nullptr;
        }
    }
}

/**
 * Handler for mouse movements
 * @param abscissa
 * @param ordinate
 */
void Menu::handleMouseMovement(uint16_t abscissa, uint16_t ordinate) {
    if (isInputCaptured()) {
        if (MenuProperties::isConstrained(abscissa, 0) && selected->willMove(abscissa)) {
            selected->move(abscissa);
            if (selected->triggerVerticesReload) {
                rotationWasModified = true;
            }
        }
    }
}
