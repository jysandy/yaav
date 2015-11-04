#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class YAAVApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};

void YAAVApp::setup()
{
}

void YAAVApp::mouseDown( MouseEvent event )
{
}

void YAAVApp::update()
{
}

void YAAVApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( YAAVApp, RendererGl )
