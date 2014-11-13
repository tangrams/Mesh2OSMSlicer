#include "ofApp.h"

#include "glmIO.h"
#include "glmGL.h"
#include "glmGeom.h"
#include "glmTesselation.h"
#include "glmIntersection.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofEnableDepthTest();
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    
    loadPLY( mesh, ofToDataPath("flatiron-l-s.ply") );
    cout << mesh.getNormals().size() << endl;
    centroid = toOf( getCentroid(mesh.getVertices()) );
    
    light1.setDiffuseColor(ofFloatColor(0.3,0.3,0.7));
    light2.setDiffuseColor(ofFloatColor(1.,0.7,0.7));
    
    threshold = 100;
    
    bWireframe = false;
    bShowLights = false;
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if(offSet>200){
        polygons.clear();
        building.clear();
        offSet = 0;
    }
    offSet += 1;
    
    if(planecut.size()>3){
        glmPolyline tmp = planecut.get2DConvexHull();
        
        float currentArea = tmp.getArea();
        
        if( abs(currentArea-prevArea) > threshold || polygons.size() == 0){
            polygons.push_back(tmp);
            
            if(polygons.size()>1){
                int last = polygons.size()-1;
                extrude(building, polygons[last], polygons[last-1][0].z, polygons[last][0].z);
            }
            
            prevArea = currentArea;
        }
        
        bOverModel = true;
    } else {
        bOverModel = false;
    }

    //  Compute new layer
    //
    glmPlane plane;
    plane.set(glm::vec3(0,0,offSet), glm::vec3(0,0,1));
    
    IntersectionData id;
    
    planecut.clear();
    vector<glm::ivec3> faces = mesh.getTriangles();
    for(int i=0;i<faces.size();i++){
        
        glmTriangle triangle;
        triangle.set(mesh.getVertices()[faces[i][0]],
                     mesh.getVertices()[faces[i][1]],
                     mesh.getVertices()[faces[i][2]]);
        
        id = PlaneTriangleIntersection(plane, triangle);
        if(id.isIntersection){
            planecut.add(id.pos);
            planecut.add(id.pos+id.dir);
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(255);
    
    //  Set scene
    //
    light1.setPosition(ofPoint(300.*cos(ofGetElapsedTimef()*0.5+PI),
                               300.*sin(ofGetElapsedTimef()*0.5+PI),
                               300.));
    
    light2.setPosition(ofPoint(200.*cos(ofGetElapsedTimef()*0.5),
                               200.*sin(ofGetElapsedTimef()*0.5),
                               200));
    float scale = 7.;
    
    
    //  Render
    //
    cam.begin();
    
    ofPushMatrix();
    ofPushStyle();
    ofScale(scale, scale,scale);
    ofTranslate(-centroid);
    ofTranslate(50, 0);
    
    //  Mesh wireframe
    //
    if(bWireframe){
        ofSetLineWidth(1);
        ofSetColor(0,100);
        drawWireMesh(mesh);
    }
    
    
    //  Current intersected outline
    //
    ofSetLineWidth(10);
    ofSetColor(255, 0, 0);
    ofNoFill();
    drawPolyline(planecut.get2DConvexHull());
    
    if(bShowLights){
        ofSetColor(light1.getDiffuseColor());
        light1.draw();
        ofSetColor(light2.getDiffuseColor());
        light2.draw();
    }
    
    //  Reconstruction
    //
    ofEnableLighting();
    light1.enable();
    light2.enable();
    ofPushMatrix();
    
    if(!bWireframe){
        ofSetColor(255);
        drawMesh(mesh);
    }
    
    ofTranslate(-100, 0);
    ofSetColor(255);
    ofNoFill();
    
    if(bWireframe){
        ofSetLineWidth(1);
        ofSetColor(0,100);
        drawWireMesh(building);
        
        if(bOverModel && polygons.size() > 0){
            drawWireMesh( getExtrude(polygons[polygons.size()-1], polygons[polygons.size()-1][0].z, offSet) );
        }
    } else {
        drawMesh(building);
        
        if(bOverModel && polygons.size() > 0){
            drawMesh( getExtrude(polygons[polygons.size()-1], polygons[polygons.size()-1][0].z, offSet) );
        }
    }
    
    
    ofPopMatrix();
    light1.disable();
    light2.disable();
    ofDisableLighting();
    
    ofPopStyle();
    ofPopMatrix();
    cam.end();
    
    ofSetColor(0);
    ofDrawBitmapString("Area threshold: "+ ofToString(threshold) + " (UP/DOWN)", 15,15);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == OF_KEY_UP){
        threshold+=10;
    } else if (key == OF_KEY_DOWN){
        threshold-=10;
    } else if (key == ' '){
        bWireframe = !bWireframe;
    } else if (key == 'l'){
        bShowLights = !bShowLights;
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
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
