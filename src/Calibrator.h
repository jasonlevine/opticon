//
//  Calibrator.h
//  opticon
//
//  Created by Jason Levine on 2016-04-29.
//
//

#pragma once
#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxOpc.h"
#include "led-strip.h"

struct LEDtoPixel {
    int strip, led;
    float x, y;
};

class Calibrator {
public:
    void setup();
    void update();
    void draw();
    
    void start();
    bool calibrate();
    void turnOnLED();
    void contourToPixel();
    void transformPoints();
    
//private:
    int w; //cam width
    int h; //cam height
    
    ofVideoGrabber          grabber;
    ofxCvColorImage			colorImg;
    
    ofxCvGrayscaleImage 	grayImage;
    ofxCvGrayscaleImage 	grayBg;
    ofxCvGrayscaleImage 	grayDiff;
    
    ofxCvContourFinder 	contourFinder;

    vector<ledStrip> * strips;
    int calibratingLed, calibratingStrip;
    vector<LEDtoPixel> calibrationData;
    
    ofParameter<int> threshold;
    
    ofxOPC *opcClient;
};
