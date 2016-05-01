#include "ofApp.h"

#define NUMLEDS 60
#define NUMSTRIPS 24

//--------------------------------------------------------------
void ofApp::setup(){
    opcClient.setup("127.0.0.1", 7890);
    
    // start video
    video.load("renders/small movie 07.mp4");
    video.play();
    
    stripsVideo.allocate(video.getWidth(), video.getHeight(), OF_IMAGE_COLOR);
    for (unsigned int x = 0; x < stripsVideo.getWidth(); x++) {
        for (unsigned int y = 0; y < stripsVideo.getHeight(); y++){
            stripsVideo.setColor(x, y, ofColor(0));
        }
    }
    
    //setup strips
    for (unsigned int i = 0; i < NUMSTRIPS; i++){
        ledStrip strip;
        strip.setup(NUMLEDS);
        strips.push_back(strip);
    }
    
    loadSettings();

    calibrator.setup();
    calibrator.strips = &strips;
    calibrator.opcClient = &opcClient;
    bCalibrate = false;
    //setup gui
    calibrator.threshold.set("calib threshold", 200, 127, 254);
    maxLedBrightness.set("max LED brightness", 0.7, 0.1, 1.0);
    
    gui.setup();
    gui.add(calibrator.threshold);
    gui.add(maxLedBrightness);
    gui.setPosition(0,0);
    
//    ofSetWindowShape(calibrator.w, calibrator.h);
    ofSetWindowTitle("OPTICON");
}

//--------------------------------------------------------------
void ofApp::update(){
//    calibrator.update();
    
    if (bCalibrate){
//        bCalibrate = calibrator.calibrate();
    }
    else {
        video.update();
        ofPixels vidPix = video.getPixels();
        
        if (vidPix.size()){
            for (unsigned int i = 0; i < strips.size(); i++){
                strips[i].update(vidPix, maxLedBrightness);
                
                //awkward!
                for (int j = 0; j < strips[i].pixels.size(); j++){
                    stripsVideo.setColor(strips[i].pixelToLed[j].x * video.getWidth(),
                                         strips[i].pixelToLed[j].y * video.getHeight(),
                                         strips[i].pixels[j]);
                }
            }
            stripsVideo.update();
        }
        
        for (unsigned int i = 0; i < strips.size(); i++) opcClient.writeChannel(i+1, strips[i].pixels);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
//    calibrator.draw();
    if (bCalibrate) {
        //colorImg.draw(0,0);
    }
    else {
//        grayImage.draw(0,0);
        for (unsigned int i = 0; i < strips.size(); i++){
            strips[i].draw();
        }
        ofSetColor(255);
        video.draw(0, 0);
        stripsVideo.draw(0, video.getHeight());
        gui.draw();
    }
}

//--------------------------------------------------------------
void ofApp::exit(){
    saveSettings();
    
    for (unsigned int i = 0; i < strips.size(); i++){
        strips[i].blackout();
    }
}

//--------------------------------------------------------------
void ofApp::loadSettings(){
    ofXml settings;
    settings.load("settings.xml");
    settings.setTo("root");
    
    settings.setTo("strip[0]");
    for (int i = 0; i < strips.size(); i++){
        settings.setTo("led[0]");
        for (int j = 0; j < 60; j++){
            ofVec2f pixel;
            pixel.x = settings.getValue<float>("X");
            pixel.y = settings.getValue<float>("Y");
            cout << i << " " << j << " " << pixel << endl;
            strips[i].calibrateLed(j, pixel);
            
            settings.setToSibling();
        }
        settings.setToParent();
        settings.setToSibling();
    }
    
    
    
    //    ofBuffer csvBuffer = ofBufferFromFile("settings.csv");
//    while (! csvBuffer.isLastLine()) {
//        string line = csvBuffer.getNextLine();
//        vector<string> chunks = ofSplitString(line, ",");
//        int strip = ofToInt(chunks[0]);
//        int led = ofToInt(chunks[1]);
//        ofVec2f pixel = ofVec2f(ofToFloat(chunks[2]), ofToFloat(chunks[3]));
//        strips[strip].calibrateLed(led, pixel);
//    }
    
//    settings.loadFile(ofToDataPath("settings.csv"));
//    for (unsigned int i = 0; i < strips.size() * NUMLEDS; i++){
//        int strip = settings.getInt(i, 0);
//        int led = settings.getInt(i, 1);
//        ofVec2f pixel = ofVec2f(settings.getFloat(i, 2), settings.getFloat(i,3));
//        
//        strips[strip].calibrateLed(led, pixel);
//    }
}
 //--------------------------------------------------------------
void ofApp::saveSettings(){
//    for (int i = 0; i < calibrationData.size(); i++){
//        int row = calibrationData[i].strip * NUMLEDS + calibrationData[i].led;
//        settings.setInt(row, 0, calibrationData[i].strip);
//        settings.setInt(row, 1, calibrationData[i].led);
//        settings.setFloat(row, 2, calibrationData[i].x);
//        settings.setFloat(row, 3, calibrationData[i].y);
//    }
//    settings.saveFile(ofToDataPath("settings.csv"));
    
    
    ofXml settings;
    settings.addChild("root");
    settings.setTo("root");
    for (unsigned int i = 0; i < strips.size(); i++){
        ofXml strip;
        strip.addChild("strip");
        strip.setTo("strip");
        for (int j = 0; j < strips[i].pixelToLed.size(); j++){
            ofXml led;
            led.addChild("led");
            led.setTo("led");
            led.addValue("X", strips[i].pixelToLed[j].x);
            led.addValue("Y", strips[i].pixelToLed[j].y);
            strip.addXml(led);
        }
        settings.addXml(strip);
    }
    
    settings.save("settings.xml");
}



//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if (key == 'c'){
        bCalibrate = true;
        calibrator.start();
    }
    
    
//    if (key == OF_KEY_LEFT){
//        threshold--;
//        if (threshold < 0) threshold = 0;
//        cout << "thrsh " << threshold << endl;
//    }
//    
//    if (key == OF_KEY_RIGHT){
//        threshold++;
//        if (threshold > 254) threshold = 254;
//        cout << "thrsh " << threshold << endl;
//    }
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
    
    stripsVideo.allocate(video.getWidth(), video.getHeight(), OF_IMAGE_COLOR);
    for (int x = 0; x < stripsVideo.getWidth(); x++) {
        for (int y = 0; y < stripsVideo.getHeight(); y++){
            stripsVideo.setColor(x, y, ofColor(0));
        }
    }
}
