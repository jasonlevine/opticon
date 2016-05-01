#pragma once

#include "ofMain.h"


#include "ofxGui.h"
#include "ofxCsv.h"
//#include "led-strip.h"
#include "Calibrator.h"


class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    void exit();
    
    void loadSettings();
    void saveSettings();
    
    void calibrate();

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
    ofParameter<float> maxLedBrightness;
    
    Calibrator calibrator;
    bool bCalibrate;
    
    ofxCsv settings;
    //video
    ofVideoPlayer video;
    ofImage stripsVideo;
    
    //gui
    ofxPanel gui;
};
