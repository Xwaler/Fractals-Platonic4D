#include <algorithm>
#include "../headers/vectorTools.h"

using namespace std;

bool contains(const vector<Faces> &vector, Faces element){
    return any_of(vector.begin(), vector.end(), [&element](Faces face){
        return face == element;
    });
}

float getPointAbscissa(const vector<float> &sourceVector, uint32_t pointIndex) {
    return sourceVector[pointIndex * 3];
}

float getPointOrdinate(const vector<float> &sourceVector, uint32_t pointIndex) {
    return sourceVector[pointIndex * 3 + 1];
}

float getPointHeight(const vector<float> &sourceVector, uint32_t pointIndex) {
    return sourceVector[pointIndex * 3 + 2];
}

vector<float> &addPointToVector(vector<float> &targetVector, const vector<float> &sourceVector, uint32_t pointIndex) {
    targetVector.push_back(getPointAbscissa(sourceVector, pointIndex));
    targetVector.push_back(getPointOrdinate(sourceVector, pointIndex));
    targetVector.push_back(getPointHeight(sourceVector, pointIndex));
    return targetVector;
}