#include <vector>
#include <random>

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder\gl\Shader.h"

using namespace ci;
using namespace ci::app;

class YAAVApp : public App {
public:
    void setup() override;
    void mouseDown(MouseEvent event) override;
    void update() override;
    void draw() override;

private:
    void drawVisualization(std::vector<float>);

    CameraPersp cam;
    gl::GlslProgRef shader;
    std::default_random_engine generator;
};

void YAAVApp::setup()
{
    gl::enableDepth();
    auto lambert = gl::ShaderDef().lambert().color();
    this->shader = gl::getStockShader(lambert);
    this->shader->bind();
}

void YAAVApp::mouseDown(MouseEvent event)
{
}

void YAAVApp::update()
{
    //Update the camera here
    cam.lookAt(vec3(100, 120, 100), vec3());
}

void YAAVApp::draw()
{
    gl::clear(Color(0, 0, 0));
    gl::setMatrices(cam);
    
    std::vector<float> randomSamples;
    std::uniform_real_distribution<float> distribution;
    for (size_t i = 0; i < 32; i++)
    {
        randomSamples.push_back(distribution(this->generator));
    }
    this->drawVisualization(randomSamples);
}

void YAAVApp::drawVisualization(std::vector<float> sampleParams)
{
    float t = static_cast<float>(getElapsedSeconds());
    const float pathScale = 50.0f;
    const int numRingSpheres = 8;
    const float maxRingRadius = 10.0f;
    const float sphereRadius = 1.0f;
    float du = static_cast<float>(2 * M_PI) / sampleParams.size();

    gl::pushModelMatrix();
    for (size_t i = 0; i < sampleParams.size(); i++)
    {
        float u = i * du;
        vec3 ringCenter(
            pathScale * cos(u),
            pathScale * cos(M_PI * t) * cos(u) * sin(u),
            pathScale * sin(u)
            );
        vec3 ringTangent = vec3(
            pathScale * cos(u + 0.01),
            pathScale * cos(M_PI * t) * cos(u + 0.01) * sin(u + 0.01),
            pathScale * sin(u + 0.01)
            ) - ringCenter;

        float r = maxRingRadius * (1 - exp(-2*sampleParams[i]));
        //float r = maxRingRadius;
        auto axisToRing = r * normalize(cross(ringTangent, vec3(1, 0, 0)));
        float dtheta = (2 * M_PI) / numRingSpheres;
        for (size_t j = 0; j < numRingSpheres; j++)
        {
            gl::pushModelMatrix();
            gl::translate(ringCenter);
            gl::rotate(dtheta * j + (M_PI / 4) * t, ringTangent);
            gl::translate(axisToRing);
            float rel = j / static_cast<float>(numRingSpheres);
            gl::color(Color(CM_HSV, rel, 1, 1));
            gl::drawSphere(vec3(), sphereRadius);
            gl::popModelMatrix();
        }
    }
    gl::popModelMatrix();
}

CINDER_APP(YAAVApp, RendererGl(RendererGl::Options().msaa(16)))
