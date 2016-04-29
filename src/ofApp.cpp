#include "ofApp.h"
#define NUMLEDS 60
//#define NUMSTRIPS 24

//--------------------------------------------------------------
void ofApp::setup(){
    opcClient.setup("127.0.0.1", 7890);
    
    
    //setup grabber and cv
    w = 1280;
    h = 800;
    grabber.setup(w,h);

    colorImg.allocate(w, h);
    grayImage.allocate(w, h);
    grayBg.allocate(w, h);
    grayDiff.allocate(w, h);
    
    // start video
    video.load("renders/small movie 07.mp4");
    video.play();
    
    //setup strips
    for (int i = 0; i < 24; i++){
        ledStrip strip;
        strip.setup(NUMLEDS);
        strips.push_back(strip);
    }

    //load last calibration
//    settings.loadFile(ofToDataPath("settings.csv"));
    
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
//    for (int i = 0; i < strips.size() * NUMLEDS; i++){
//        int strip = settings.getInt(i, 0);
//        int led = settings.getInt(i, 1);
//        ofVec2f pixel = ofVec2f(settings.getFloat(i, 2), settings.getFloat(i,3));
//        
//        strips[strip].calibrateLed(led, pixel);
//    }
    
    //setup gui
    threshold.set("calib threshold", 200, 127, 254);
    maxLedBrightness.set("max LED brightness", 0.7, 0.1, 1.0);
    
    gui.setup();
    gui.add(threshold);
    gui.add(maxLedBrightness);
    gui.setPosition(0,0);
    
    ofSetWindowShape(w, h);
    ofSetWindowTitle("OPTICON");
}

//--------------------------------------------------------------
void ofApp::update(){
    grabber.update();
    colorImg.setFromPixels(grabber.getPixels());
    grayImage = colorImg;
    grayImage.threshold(threshold);
    
    if (calibrate){
        //turn on each led one at a time
        for (int i = 0; i < strips.size(); i++) strips[i].setLeds(ofColor(0));
        
        strips[calibratingStrip].setLed(calibratingLed, ofColor(200));
        
        for (int i = 0; i < strips.size(); i++) opcClient.writeChannel(i+1, strips[i].pixels);
        
        if (ofGetFrameNum() % 5 == 0){
            //get pixels
            colorImg.setFromPixels(grabber.getPixels());
            
            //find contours
            grayImage = colorImg;
    //        grayDiff.absDiff(grayBg, grayImage);
            grayImage.threshold(threshold);
            contourFinder.findContours(grayImage, 3, (w*h)/3, 10, false);
            
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
            if (calibratingLed == NUMLEDS){
                //next Strip
                calibratingLed = 0;
                calibratingStrip++;
                
                //done calibrating
                if (calibratingStrip == strips.size()) {
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
                        strips[calibrationData[i].strip].calibrateLed(
                            calibrationData[i].led,
                            ofVec2f(calibrationData[i].x, calibrationData[i].y));
                    }
                    // save settings
//                    for (int i = 0; i < calibrationData.size(); i++){
//                        int row = calibrationData[i].strip * NUMLEDS + calibrationData[i].led;
//                        settings.setInt(row, 0, calibrationData[i].strip);
//                        settings.setInt(row, 1, calibrationData[i].led);
//                        settings.setFloat(row, 2, calibrationData[i].x);
//                        settings.setFloat(row, 3, calibrationData[i].y);
//                    }
////
//                    settings.saveFile(ofToDataPath("settings.csv"));
                    
                    //done
                    calibrate = false;
                }
            }
        }
    
    }
    else {
        video.update();
        ofPixels vidPix = video.getPixels();
        
        if (vidPix.size()){
            for (int i = 0; i < strips.size(); i++){
                strips[i].update(vidPix, maxLedBrightness);
            }
        }
        
        for (int i = 0; i < strips.size(); i++) opcClient.writeChannel(i+1, strips[i].pixels);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){

    if (calibrate) {
        //colorImg.draw(0,0);
        ofBackground(25);
        grayImage.draw(0,0);
        contourFinder.draw(0, 0);
        ofDrawBitmapString(ofToString(calibratingStrip) + " - " + ofToString(calibratingLed), 20, 20);
    }
    else {
        grayImage.draw(0,0);
        for (int i = 0; i < strips.size(); i++){
            strips[i].draw();
        }
        video.draw(w - video.getWidth(), 0);
        gui.draw();
    }
}

//--------------------------------------------------------------
void ofApp::exit(){
    for (int i = 0; i < strips.size(); i++){
        strips[i].blackout();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if (key == 'c'){
        calibrate = true;
        calibratingLed = 0;
        calibratingStrip = 0;
        calibrationData.clear();
    }
    
    
    if (key == OF_KEY_LEFT){
        threshold--;
        if (threshold < 0) threshold = 0;
        cout << "thrsh " << threshold << endl;
    }
    
    if (key == OF_KEY_RIGHT){
        threshold++;
        if (threshold > 254) threshold = 254;
        cout << "thrsh " << threshold << endl;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
    video.load(dragInfo.files[0]);
    video.play();
}
