//
//  led-strip.cpp
//  opticon-virtual
//
//  Created by Jason Levine on 2016-02-18.
//
//

#include "led-strip.h"


void ledStrip::setup(int num){
    for (int i = 0; i < num; i++){
        pixels.push_back(ofColor(0));
        pixelToLed.push_back(ofVec2f(0.5,0.5));
    }
}

//TODO: make mode where video is cropped and centered instead of stretched
void ledStrip::update(ofPixels &framePix, float maxBrightness){
    for (int i = 0; i < pixels.size(); i++){
        int x = pixelToLed[i].x * 1.3333 * framePix.getWidth() - 0.1666 * framePix.getWidth();
        int y = pixelToLed[i].y * framePix.getHeight();
        pixels[i] = framePix.getColor(x, y) * maxBrightness;
    }
    
    //cout << pixelToLed[0].x * framePix.getWidth()  << endl;
}


void ledStrip::draw(){
    
}


void ledStrip::setLed(int led, ofColor color){
    pixels[led] = color;
}

void ledStrip::blackout(){
    for (int i = 0; i < pixels.size(); i++){
        pixels[i].set(0);
    }
}

void ledStrip::setLeds(ofColor color){
    for (int i = 0; i < pixels.size(); i++){
        pixels[i].set(color);
    }
}

void ledStrip::calibrateLed(int led, ofVec2f pixel){
    pixelToLed[led] = pixel;
}
