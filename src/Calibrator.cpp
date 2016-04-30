//
//  Calibrator.cpp
//  opticon
//
//  Created by Jason Levine on 2016-04-29.
//
//

#include "Calibrator.h"
void Calibrator::setup(){
     //setup grabber and cv
    w = 1280;
    h = 800;
    grabber.setup(w,h);

    colorImg.allocate(w, h);
    grayImage.allocate(w, h);
    grayBg.allocate(w, h);
    grayDiff.allocate(w, h);
}

void Calibrator::update(){
    grabber.update();
    colorImg.setFromPixels(grabber.getPixels());
    grayImage = colorImg;
    grayImage.threshold(threshold);
}

void Calibrator::draw(){
    ofBackground(25);
    grayImage.draw(0,0);
//        contourFinder.draw(0, 0);
    if (contourFinder.blobs.size()){
        ofSetColor(0,255,0);
        ofDrawCircle(contourFinder.blobs[0].centroid, 10);
        ofSetColor(255);
    }
    
    ofDrawBitmapString(ofToString(calibratingStrip) + " - " + ofToString(calibratingLed), 20, 20);
}

void Calibrator::start(){
    calibratingLed = 0;
    calibratingStrip = 0;
    calibrationData.clear();
}

//--------------------------------------------------------------
bool Calibrator::calibrate(){
    //turn on each led one at a time
    for (int i = 0; i < strips->size(); i++) strips->at(i).setLeds(ofColor(0));

    strips->at(calibratingStrip).setLed(calibratingLed, ofColor(250));

    for (int i = 0; i < strips->size(); i++) opcClient->writeChannel(i+1, strips->at(i).pixels);

    if (ofGetFrameNum() % 5 == 0){
        //get pixels
        colorImg.setFromPixels(grabber.getPixels());
        
        //find contours
        grayImage = colorImg;
    //        grayDiff.absDiff(grayBg, grayImage);
        grayImage.threshold(threshold);
        contourFinder.findContours(grayImage, 9, (w*h)/3, 10, false);
        
        ofPoint pixel = ofPoint(-1, -1);
        
        if (contourFinder.blobs.size()){
            
            //map centroid to movie dims
            ofPoint centroid = contourFinder.blobs[0].centroid;
            
            pixel.x = centroid.x;
            pixel.y = centroid.y;
            
            //assign pixel pos to strip calibration
    //                strips[calibratingStrip].calibrateLed(calibratingLed, pixel);
            
            cout << "MAPPED " << calibratingStrip << " - " << calibratingLed <<  " to " << pixel << endl;
        }
        
        //save data in temp data structure
        LEDtoPixel temp;
        temp.strip = calibratingStrip;
        temp.led = calibratingLed;
        temp.x = pixel.x;
        temp.y = pixel.y;
        
        calibrationData.push_back(temp);
        
        //next LED
        calibratingLed++;
        if (calibratingLed == 60){
            //next Strip
            calibratingLed = 0;
            calibratingStrip++;
            
            //done calibrating
            if (calibratingStrip == 24) {
                // find min/max
                int maxX = 0;
                int minX = w;
                int maxY = 0;
                int minY = h;
                
                for (int i = 0; i < calibrationData.size(); i++){
                    if (calibrationData[i].x != -1){
                        if (calibrationData[i].x > maxX) maxX = calibrationData[i].x;
                        else if (calibrationData[i].x < minX) minX = calibrationData[i].x;
                        if (calibrationData[i].y > maxY) maxY = calibrationData[i].y;
                        else if (calibrationData[i].y < minY) minY = calibrationData[i].y;
                    }
                }
                
                //the opticon is taller than it is wide. We want it to take pixels formt the center of the image.
                
                
                int opticonHeight = maxY - minY;
                int opticonWidth = maxX - minX;
                float theoreticalWidth = float(w) / float(h) * opticonHeight;
                float padding = (theoreticalWidth - opticonWidth)*0.5;
                
                cout << "x: " << minX << " " << maxX << endl
                     << "y: " << minY << " " << maxY << endl
                     << "w: " << opticonWidth << " h: " << opticonHeight << "H: " << theoreticalWidth << endl;
                
                
                
                // scale and translate points
                for (int i = 0; i < calibrationData.size(); i++){
                    if (calibrationData[i].x != -1){
                        calibrationData[i].x -= minX;
                        
                        calibrationData[i].x += padding;
                        calibrationData[i].x /= opticonHeight;
    //                            calibrationData[i].x /= theoreticalWidth;
                        calibrationData[i].y -= minY;
                        calibrationData[i].y /= opticonHeight;
                    }
                }
                
                for (int i = 0; i < calibrationData.size(); i++){
                    strips->at(calibrationData[i].strip).calibrateLed(
                        calibrationData[i].led,
                        ofVec2f(calibrationData[i].x, calibrationData[i].y));
                }
                // save settings
//                saveSettings();
                //done
                return false;
            }
        }
    }
    return true;
}






  //load last calibration
    
//    ofVec2f uncalibrated = ofVec2f(0.5,0.5);
//    int start = 0;
//    int end = 0;
//    ofVec2f lastPixel = ofVec2f(0.4, 0.4); // late night programming hack......
//    for (int i = 0; i < strips.size() * NUMLEDS; i++){
//        ofVec2f pixel = ofVec2f(settings.getFloat(i, 2), settings.getFloat(i,3));
//        if (pixel == uncalibrated && lastPixel != uncalibrated) start = i;
//        else if (pixel != uncalibrated && lastPixel == uncalibrated) end = i;
//        
//        if (start != 0 && end != 0) {
//            float preX = settings.getFloat(start-1, 2);
//            float preY = settings.getFloat(start-1, 3);
//            float postX = settings.getFloat(end+1, 2);
//            float postY = settings.getFloat(end+1, 3);
//
//            int range = end - start;
//            for (float j = 1; j <= range; j++){
//                float x = preX * j/range + postX * (1.0 - j/range);
//                float y = preY * j/range + postY * (1.0 - j/range);
//                settings.setFloat(start + j - 1, 2, x);
//                settings.setFloat(start + j - 1, 3, y);
//            }
//            
//            start = 0; end = 0;
//        }
//    }
//