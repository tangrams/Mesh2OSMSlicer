#include "ofApp.h"

#include "glmGL.h"
#include "glmIntersection.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofEnableDepthTest();
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    
    mesh.load("LIDar-SfM.ply");
    centroid = mesh.getCentroid();
    
    light1.setDiffuseColor(ofFloatColor(0.7,0.7,1.));
    light2.setDiffuseColor(ofFloatColor(1.,0.7,0.7));

    threshold = 100;
    
    ofMeshFace a;
    ofPoint p;
    
    p.scale(1);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if(offSet>200){
        polygons.clear();
        offSet = 0;
    }
    offSet += 1;
    
    light1.setPosition(ofPoint(100.*cos(ofGetElapsedTimef()*0.5),
                              100.*sin(ofGetElapsedTimef()*0.5),
                              500));
    
    light2.setPosition(ofPoint(200.*sin(ofGetElapsedTimef()),
                               200.*cos(ofGetElapsedTimef()),
                               100));
    
    if(planecut.size()>2){
        glmPolyline tmp = planecut.get2DConvexHull();
        
        float currentArea = tmp.getArea();
        
        if( abs(currentArea-prevArea) > threshold ){
            polygons.push_back(tmp);
            prevArea = currentArea;
        }
        
        bScanning = true;
    } else {
        bScanning = false;
    }

    //  Compute new layer
    //
    glmPlane plane;
    plane.set(glm::vec3(0,0,offSet), glm::vec3(0,0,1));
    
    IntersectionData id;
    
    planecut.clear();
    for(int i=0;i<mesh.getUniqueFaces().size();i++){
        glmTriangle triangle;
        ofMeshFace face = mesh.getUniqueFaces().at(i);
        triangle.set(toGlm(face.getVertex(0)),toGlm(face.getVertex(1)),toGlm(face.getVertex(2)));
        
        id = PlaneTriangleIntersection(plane, triangle);
        if(id.isIntersection){
            planecut.add(id.pos);
            planecut.add(id.pos+id.dir);
        }
    }
}

glmMesh ofApp::extrude(const glmPolyline &_poly, float min, float max ){
    glmMesh mesh;
    
    glmPolyline top, buttom;
    
    for (int i = 0; i < _poly.size(); i++) {
        top.add(glm::vec3(_poly[i].x,_poly[i].y, max));
        buttom.add(glm::vec3(_poly[i].x,_poly[i].y, min));
    }
    
    mesh.add(top);
    mesh.add(buttom);
    
    uint16_t indexOffset = (uint16_t)mesh.getVertices().size();
    
    glm::vec3 tan, nor;
    
    for (int i = 0; i < _poly.size() - 1; i++) {
        
        //For each vertex in the polygon, make two triangles to form a quad
        //
        glm::vec3 ip0 = glm::vec3(_poly[i].x,_poly[i].y,0);
        glm::vec3 ip1 = glm::vec3(_poly[i+1].x,_poly[i+1].y,0);
        
        tan = ip1 - ip0;
        nor = glm::cross(UP_NORMAL, tan);
        nor = nor;
        
        mesh.addTexCoord(glm::vec2(1.,0.));
        mesh.addVertex(ip0 + glm::vec3(0,0,min));
        mesh.addNormal(nor);
        mesh.addTexCoord(glm::vec2(0.,0.));
        mesh.addVertex(ip1 + glm::vec3(0,0, min));
        mesh.addNormal(nor);
        
        mesh.addTexCoord(glm::vec2(1.,1.));
        mesh.addVertex(ip0 + glm::vec3(0,0, max));
        mesh.addNormal(nor);
        mesh.addTexCoord(glm::vec2(0.,1.));
        mesh.addVertex(ip1 + glm::vec3(0,0, max));
        mesh.addNormal(nor);
        
        mesh.addIndex(indexOffset);
        mesh.addIndex(indexOffset + 2);
        mesh.addIndex(indexOffset + 1);
        
        mesh.addIndex(indexOffset + 1);
        mesh.addIndex(indexOffset + 2);
        mesh.addIndex(indexOffset + 3);
        indexOffset += 4;
    }
    
    return mesh;
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    
    float scale = 7.;
    
    cam.begin();
    
    ofPushMatrix();
    ofPushStyle();
    ofScale(scale, scale,scale);
    ofTranslate(-centroid);
    
    ofSetLineWidth(1);
    ofSetColor(255,100);
    mesh.drawWireframe();
    
//    ofSetColor(0,255,255,30);
//    plane.draw();

    ofSetLineWidth(10);
    ofSetColor(255, 0, 0);
    ofNoFill();
    drawPolyline(planecut.get2DConvexHull());
    
    
    ofEnableLighting();
    light1.enable();
    light2.enable();
    ofPushMatrix();
    ofTranslate(-50, 0);
    ofSetColor(255);
    ofNoFill();
    for (int i = 1; i < cuts.size(); i++) {
        drawMesh(extrude(cuts[i], cuts[i-1][0].z, cuts[i][0].z));
    }
    
    if(bScanning && cuts.size() > 0){
        drawMesh(extrude(cuts[cuts.size()-1], cuts[cuts.size()-1][0].z, offSet));
    }
    
    ofPopMatrix();
    light1.disable();
    light2.disable();
    ofDisableLighting();
    
    ofPopStyle();
    ofPopMatrix();
    cam.end();
    
    ofDrawBitmapString("Area threshold: "+ ofToString(threshold), 15,15);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == OF_KEY_UP){
        threshold+=10;
    } else if (key == OF_KEY_DOWN){
        threshold-=10;
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
