#ifndef FRACTALS_PLATONIC4D_VECTORTOOLS_H
#define FRACTALS_PLATONIC4D_VECTORTOOLS_H

#include <glm/glm.hpp>

#include <vector>

#include "Faces.h"

using namespace std;

bool contains(const vector<Faces> &vector, Faces element);

float getPointAbscissa(const vector<float> &sourceVector, uint32_t pointIndex);

float getPointOrdinate(const vector<float> &sourceVector, uint32_t pointIndex);

float getPointHeight(const vector<float> &sourceVector, uint32_t pointIndex);

float getCoordinateOneThirdOfTheWay(float closest, float furthest);

vector<float> &addPointOneThirdOfTheWayToVector(vector<float> &targetVector, const vector<float> &sourceVector,
                                                uint32_t closestPointIndex, uint32_t furthestPointIndex);

vector<float> &addPointToVector(vector<float> &targetVector, const vector<float> &sourceVector, uint32_t pointIndex);
#endif //FRACTALS_PLATONIC4D_VECTORTOOLS_H
