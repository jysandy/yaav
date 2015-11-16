#include <vector>
#include <random>
#include <algorithm>
#include <string>

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder\gl\Shader.h"

#include "cinder/audio/Context.h"
#include "cinder/audio/Voice.h"
#include "cinder/audio/MonitorNode.h"

#include "cinder\params\Params.h"

#include <GridElement.h>

using namespace ci;
using namespace ci::app;

const static float cubeSide = 1.0f;
const static float maxHeight = 20.0f * cubeSide;

//Width and height in number of cubes
const static int gridWidth = 35;
const static int gridHeight = 35;

const static int numCircles = 8;
const static float circleSpacing = 4.0f;


class YAAVApp : public App {
public:
    void setup() override;
    void mouseDown(MouseEvent event) override;
    void update() override;
    void draw() override;
    void resize() override;

private:
    void drawVisualization(std::vector<float>);
    void startTrack(int trackNum);

    CameraPersp cam;
    params::InterfaceGlRef paramGui;
    gl::GlslProgRef shader;
    std::default_random_engine generator;
    std::vector<GridElement> gridElements;
    std::vector<float> heights;
    std::vector<std::string> enumNames;
    int enumSelection;
    float dt;
    float prevTime;

	//audio vars 
	audio::VoiceRef mVoice;
	audio::MonitorSpectralNodeRef mMonitorSpectralNode;

};

void YAAVApp::startTrack(int trackNum)
{
    if (mVoice && mVoice->isPlaying())
    {
        mVoice->stop();
    }
    
    std::string sourceFile;
    switch (this->enumSelection)
    {
    case 0:
        sourceFile = "some.mp3";
        break;
    case 1:
        sourceFile = "some1.mp3";
        break;
    case 2:
        sourceFile = "some2.mp3";
        break;
    case 3:
        sourceFile = "some3.mp3";
        break;
    case 4:
        sourceFile = "some4.mp3";
        break;
    default:
        sourceFile = "phail";
        break;
    }

    auto source = audio::load(loadAsset(sourceFile));
    auto options = audio::Voice::Options().connectToMaster(false);
    mVoice = audio::Voice::create(source, options);

    //configuring the sprectral node 
    auto monitorFormat = audio::MonitorSpectralNode::Format().fftSize(16).windowSize(numCircles);
    mMonitorSpectralNode = audio::master()->makeNode(new audio::MonitorSpectralNode(monitorFormat));

    //piping the node flow 
    mVoice->getOutputNode() >> mMonitorSpectralNode >> audio::master()->getOutput();

    //starting the audio playback 
    mVoice->start();
}

void YAAVApp::setup()
{
    gl::enableDepth();
    auto lambert = gl::ShaderDef().lambert().color();
    this->shader = gl::getStockShader(lambert);
    this->shader->bind();
    prevTime = getElapsedSeconds();
    dt = 0;
    //Build the grid here.
    
    float startX = -(gridWidth - 0.5) * cubeSide;
    float startZ = -(gridHeight - 0.5) * cubeSide;

    for (size_t i = 0; i < gridHeight; i++)
    {
        for (size_t j = 0; j < gridWidth; j++)
        {
            float x = startX + cubeSide * j * 2;
            float z = startZ + cubeSide * i * 2;
            gridElements.emplace_back(x, z, numCircles, circleSpacing);
        }
    }

    this->paramGui = params::InterfaceGl::create(getWindow(), "Music Parameters", toPixels(ivec2(200, 300)));

    this->enumSelection = 0;
    this->enumNames = { "Track 1", "Track 2", "Track 3", "Track 4", "Track 5" };
    this->paramGui->addParam("Select track", enumNames, &enumSelection)
        .updateFn([this]{this->startTrack(enumSelection); });

    this->paramGui->addSeparator();
    this->paramGui->addButton("Play", [this]{
        if (mVoice && !(mVoice->isPlaying()))
        {
            mVoice->start();
        }
    });
    this->paramGui->addButton("Pause", [this]{
        if (mVoice && mVoice->isPlaying())
        {
            mVoice->pause();
        }
    });
    this->paramGui->addButton("Stop", [this]{
        if (mVoice)
        {
            mVoice->stop();
        }
    });

	//loading the audio files 
    startTrack(0);
}

void YAAVApp::mouseDown(MouseEvent event)
{
}

void YAAVApp::update()
{
    this->dt = getElapsedSeconds() - prevTime;
    this->prevTime = getElapsedSeconds();
    
	//updating the heights 
	auto raw_heights = std::vector<float>(mMonitorSpectralNode->getMagSpectrum());
    assert(heights.size() == numCircles);
	//adjusting the heights accordingly 
	for (int i = 0; i < heights.size(); i++){
		raw_heights[i] = maxHeight * (1 - exp(-70 * raw_heights[i]));
		app::console() << heights[i] << "\n";
	}
	
    std::sort(raw_heights.begin(), raw_heights.end());

    heights.clear();
    for (auto x : { 7, 0, 6, 1, 5, 2, 4, 3 })
    {
        heights.push_back(raw_heights[x]);
    }
		
	//Update the camera here
    cam.lookAt(vec3(60 * cos((M_PI / 16) * prevTime), 80, 60 * sin((M_PI / 16) * prevTime)), vec3());

    for (auto& gridElement : gridElements)
    {
        gridElement.update(dt, heights);
    }
}

void YAAVApp::draw()
{
    gl::clear(Color(0, 0, 0));
    gl::setMatrices(cam);

    drawVisualization(heights);
    this->paramGui->draw();
}

void YAAVApp::drawVisualization(std::vector<float> sampleParams)
{
    gl::pushModelMatrix();
    for (auto& gridElement : gridElements)
    {
        gridElement.draw(maxHeight, cubeSide);
    }
    gl::popModelMatrix();
}

void YAAVApp::resize()
{
    this->cam.setAspectRatio(getWindowAspectRatio());
}

CINDER_APP(YAAVApp, RendererGl(RendererGl::Options().msaa(2)))
