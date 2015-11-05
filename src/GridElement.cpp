#include <GridElement.h>
#include <algorithm>
#include "cinder/gl/gl.h"

using namespace ci;

GridElement::GridElement(float x, float z, float numCircles, float circleSpacing, float center)
    : x(x), z(z), height(0.0f)
{
    float radius = sqrt(x * x + z * z);
    // nearCircleIndex is the largest circle whose radius is less than
    // the radius above.
    for (size_t i = 0; i < numCircles; i++)
    {
        if (radius > i * circleSpacing)
        {
            this->nearCircleIndex = i;
            this->nearCircleRadius = i * circleSpacing;
            this->circleSpacing = circleSpacing;
            break;
        }
    }
}

void GridElement::update(float dt, std::vector<float> heights)
{
    auto radius = sqrt(x * x + z * z);
    auto slope =
        (heights[this->nearCircleIndex + 1] - heights[this->nearCircleIndex]) / this->circleSpacing;
    auto newHeight = heights[this->nearCircleIndex] + slope * (radius - this->nearCircleRadius);
    this->height = std::max(this->height, newHeight);

    const float dropRate = 0.50f; // Units per second
    this->height -= dropRate * dt;
}

void GridElement::draw(const float maxHeight, const float cubeSide) const
{
    for (float barHeight = 0; barHeight < maxHeight; barHeight += cubeSide)
    {
        float rel = barHeight / maxHeight;
        gl::color(Color(CM_HSV, rel, 1, 1));
        gl::drawCube(vec3(this->x, barHeight - (cubeSide / 2), this->z), vec3(cubeSide));
    }
}