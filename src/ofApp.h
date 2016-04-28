#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "led-strip.h"
#include "ofxOPC.h"
#include "ofxGui.h"
#include "ofxCsv.h"

struct LEDtoPixel {
    int strip, led, x, y;
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

    ofxOPC opcClient;
    
    vector<ledStrip> strips;
    vector<LEDtoPixel> calibrationData;
    //calibration
    int w; //cam width
    int h; //cam height
    
    ofVideoGrabber grabber;
    ofxCvColorImage			colorImg;
    
    ofxCvGrayscaleImage 	grayImage;
    ofxCvGrayscaleImage 	grayBg;
    ofxCvGrayscaleImage 	grayDiff;
    
    ofxCvContourFinder 	contourFinder;

    bool calibrate;
    int calibratingLed, calibratingStrip;
    
    ofParameter<int> threshold;
    ofParameter<float> maxLedBrightness;
    
    ofxCsv settings;
    
    
    //video
    ofVideoPlayer video;
    
    //gui
    ofxPanel gui;
    
    

    
    //
};
