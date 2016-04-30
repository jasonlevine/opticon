//
//  led-strip.cpp
//  opticon-virtual
//
//  Created by Jason Levine on 2016-02-18.
//
//

#include "led-strip.h"


void ledStrip::setup(int num){
    for (unsigned int i = 0; i < num; i++){
        pixels.push_back(ofColor(0));
        pixelToLed.push_back(ofVec2f(0.5,0.5));
    }
}

//TODO: make mode where video is cropped and centered instead of stretched
void ledStrip::update(ofPixels &framePix, float maxBrightness){
    for (unsigned int i = 0; i < pixels.size(); i++){
        if (pixelToLed[i].x != -1) { //oh jesus its late and i just want it to work
            int x = pixelToLed[i].x * framePix.getWidth();
            int y = pixelToLed[i].y * framePix.getHeight();
            pixels[i] = framePix.getColor(x, y) * maxBrightness;
        }
        else {
            pixels[i] = ofColor(0);
        }
    }
    
    //cout << pixelToLed[0].x * framePix.getWidth()  << endl;
}


void ledStrip::draw(){
    
}


void ledStrip::setLed(int led, ofColor color){
    pixels[led] = color;
}

void ledStrip::blackout(){
    for (unsigned int i = 0; i < pixels.size(); i++){
        pixels[i].set(0);
    }
}

void ledStrip::setLeds(ofColor color){
    for (unsigned int i = 0; i < pixels.size(); i++){
        pixels[i].set(color);
    }
}

void ledStrip::calibrateLed(int led, ofVec2f pixel){
    pixelToLed[led] = pixel;
}
