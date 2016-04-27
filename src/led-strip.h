//
//  led-strip.h
//  opticon-virtual
//
//  Created by Jason Levine on 2016-02-18.
//
//

#pragma once
#include "ofMain.h"

#define HEIGHT 60 * 6 * 3
#define WIDTH 12

class ledStrip {
public:
    void setup(int num);
    void update(ofPixels &framePix, float maxBrightness);
    void draw();

    void setLed(int led, ofColor color);
    void setLeds(ofColor color);
    void calibrateLed(int led, ofVec2f pixel);
    void blackout();
    
//private:
    vector<ofColor> pixels;
    vector<ofVec2f> pixelToLed;
};