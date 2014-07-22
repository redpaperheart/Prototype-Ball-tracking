#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Serial.h"
#include "ciSpacebrew.h"
#include "cinder/Utilities.h"
#include "cinder/Json.h"
#include "cinder/gl/Texture.h"
#include "cinder/params/Params.h"
#include <boost/regex.hpp>
#include "cinder/Text.h"

#include "WebSocketClient.h"
#include "boost/algorithm/string.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

class ArduinoSpacebrewApp : public AppNative {
public:
    void prepareSettings( Settings *settings);
	void setup();
	void mouseDown( MouseEvent event );
	void update();
	void draw();
    bool serialInitiallized();
    
    bool		 bTextureComplete;

    
    //Serial to Arduino
    Serial serial;

    std::string lastString;
    gl::Texture	mTexture;
    char contact;
        
    // Spacebrew
    ci::Spacebrew::ConnectionRef mSpacebrew;
    std::string mSpacebrewHost = "10.0.1.8";
    std::string mSpacebrewName = "cinder-Arduino-App";
    std::string mSpacebrewDescription = "Kinect to ARDUINO";
    void onMessage( const ci::Spacebrew::Message &msg );

    
    
    
};

void ArduinoSpacebrewApp::prepareSettings(cinder::app::AppBasic::Settings *settings){
    settings->setWindowSize(640,480);
    settings->setFrameRate(60.f);
    
}

void ArduinoSpacebrewApp::setup()
{
    
	lastString = "";
    bTextureComplete = false;
    
    //SERIAL SETUP
    
    const vector<Serial::Device> &devices( Serial::getDevices() );
	for( vector<Serial::Device>::const_iterator deviceIt = devices.begin(); deviceIt != devices.end(); ++deviceIt ) {
		console() << "Device: " << deviceIt->getName() << endl;
	}
	
	try {
		Serial::Device dev = Serial::findDeviceByNameContains("tty.usbmodem");
		serial = Serial( dev, 9600);
	}
	catch( ... ) {
		console() << "There was an error initializing the serial device!" << std::endl;
		exit( -1 );
	}
    contact = 0;
	serial.flush();
    

    
    //SPACEBREW
    
    mSpacebrew = ci::Spacebrew::Connection::create( this, mSpacebrewHost, mSpacebrewName, mSpacebrewDescription );
//    mSpacebrew->addSubscribe( "splash",         ci::Spacebrew::TYPE_STRING);
//    mSpacebrew->addSubscribe( "opticalFlow",    ci::Spacebrew::TYPE_STRING);
//    mSpacebrew->addSubscribe( "motion",         ci::Spacebrew::TYPE_STRING);
//    mSpacebrew->addSubscribe( "style",          ci::Spacebrew::TYPE_STRING);
//    mSpacebrew->addSubscribe( "color",          ci::Spacebrew::TYPE_STRING);
    mSpacebrew->addSubscribe( "blobCenter",          ci::Spacebrew::TYPE_STRING);

    mSpacebrew->connect();
    mSpacebrew->addListener( &ArduinoSpacebrewApp::onMessage, this);
    
}

bool ArduinoSpacebrewApp::serialInitiallized(){
    //serial handshake with the arduino
    
	if (contact != '*'){
		contact = (char) serial.readByte();
        return false;
	}else{
        serial.writeByte('0');
        return true;
    }
    
}

void ArduinoSpacebrewApp::onMessage( const ci::Spacebrew::Message &msg ){
    
    
    if(msg.name =="blobCenter"){
        lastString = msg.valueString();
        console() <<"blobCenter: "  <<lastString<<endl;
    }
    
    
    
    //dont need the rest of these
    
    
//    if (msg.name == "splash") {
//        
//        //console() << "splash: \"" << msg.valueString() << "\"" << std::endl;
//         std::vector<std::string> data = explode(msg.valueString(), ',');
//        float xNormalized = ci::fromString<float>(data[0]);
//        float yNormalized = ci::fromString<float>(data[1]);
//
//        if( xNormalized < 0.1 || yNormalized < 0.1 || xNormalized > 0.9 || yNormalized > 0.9) return;
//
//        int x = int(640 * ci::fromString<float>(data[0]));
//        int y = int(480 * ci::fromString<float>(data[1]));
//        //splash(x,y);
//        // {"splash":["x","y"]}
//        
//    }else if (msg.name == "opticalFlow") {
//        console() << "opticalFlow: " << msg.valueString() << std::endl;
//        //console() << "opticalFlow. " << std::endl;
//        //{"grid":["column","rows"],"data"[["x","y"],["x","y"], ... ]}
//         unserializeFlow( msg.valueString() );
//    }else if (msg.name == "color") {
//        console() << "color: " << msg.valueString() << std::endl;
//        //{"color1":"0xFF00FF","color2":"0xFF00FF"}
//        std::vector<std::string> colors = explode(msg.valueString(), ',');
//        //console() << "color: " << colors[0] << colors[1] << std::endl;
//        //changeThemeColor( ci::fromString<int>(colors[0]), ci::fromString<int>(colors[1]));
//    }else if (msg.name == "style") {
//        console() << "style: " << msg.valueString() << std::endl;
//        //changeParticleStyle( ci::fromString<int>( msg.valueString() ));
//        //{"style":"0/1/2"}
//    }else if (msg.name == "motion") {
//        console() << "motion: " << msg.valueString() << std::endl;
//        //console() << "motion " << std::endl;
//        unserializeMotion( msg.valueString() );
//        //{"style":"0/1/2"}
//}


}

void ArduinoSpacebrewApp::mouseDown( MouseEvent event )
{
}

void ArduinoSpacebrewApp::update()
{
    if (serialInitiallized() && getElapsedFrames() % 2== 0){
        serial.writeString(lastString + "\n");
        TextLayout simple;
        simple.setFont( Font( "Arial Black", 54 ) );
        simple.setColor( Color( .7, .7, .2 ) );
        simple.addLine( lastString );
        simple.setLeadingOffset( 0 );
        mTexture = gl::Texture( simple.render( true, false ) );
        bTextureComplete = true;
        serial.flush();
    }

    
}

void ArduinoSpacebrewApp::draw()
{
    gl::clear(Color(0,0,0));
	gl::enableAlphaBlending( false );
	
	if(bTextureComplete){
		glColor3f( 1.0f, 1.0f, 1.0f );
		gl::draw( mTexture, Vec2f( 10, 10 ) );
	}
}


CINDER_APP_NATIVE( ArduinoSpacebrewApp, RendererGl )
