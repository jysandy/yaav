#pragma once
#include <vector>

class GridElement
{
public:
    GridElement(float x, float y, float numCircles, float circleSpacing, float center);
    void update(float dt, std::vector<float> heights);
    void draw(const float maxHeight, const float cubeSide) const;

private:
    float x;
    float z;
    float height;
    int nearCircleIndex;
    float nearCircleRadius;
    float circleSpacing;
};