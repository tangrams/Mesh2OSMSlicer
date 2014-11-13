#pragma once

#include "ofMain.h"

#include "ofxGlm.h"
#include "ofxGlmTools.h"

class ofApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    glmMesh extrude(const glmPolyline &_poly, float min, float max );
    
    ofEasyCam   cam;
    ofLight     light1,light2;
    
    ofMesh      mesh;
    ofPoint     centroid;
    
    glmPolyline planecut;
    vector<glmPolyline> cuts;
    
    int     threshold;
    
    float   offSet;
    float   prevArea;
    
    bool    bScanning;
};
