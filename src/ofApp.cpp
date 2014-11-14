#include "ofApp.h"

#include "glmIO.h"
#include "glmGL.h"
#include "glmGeom.h"
#include "glmTesselation.h"
#include "glmIntersection.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    //  Setup general scene
    //
    ofEnableDepthTest();
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    ofSetFullscreen(true);
    
    light1.setPosition(ofPoint(300.*cos(0.1+PI),
                               300.*sin(0.1+PI),
                               300.));
    
    light2.setPosition(ofPoint(200.*cos(0.1),
                               200.*sin(0.1),
                               200));
    
    light1.setDiffuseColor(ofFloatColor(0.5,0.5,0.7));
    light2.setDiffuseColor(ofFloatColor(0.7,0.7,0.7));
    
    cam.setDistance(1000);
    cam.setOrientation(ofQuaternion(0.279266, 0.515565, 0.737963, 0.334088));
    cam.setPosition(1033.08, 785.632, 431.556);
    floorShader.load("floor");
    
    fade = 0;
    rotation = 90;
    bRotate = true;
    bWireframe = false;
    bShowLights = false;
    nScene = 0;
    
    //  PointCloud
    //
    loadPLY(pointcloud, ofToDataPath("pointcloud-l-s.ply") );
    pointcloud.setDrawMode(POINTS);
    cout << "PCL " << pointcloud.getVertices().size() << endl;
    
    //  Mesh
    //
    loadPLY( mesh, ofToDataPath("mesh-l-s.ply") );
    mesh.setDrawMode(TRIANGLES);
    cout << "Mesh " << mesh.getVertices().size() << endl;
    centroid = toOf( getCentroid(mesh.getVertices()) );
    threshold = 100;
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if(nScene == 3){
        
        //  Reconstruct the mesh in extrueded polygons
        //
        
        //  Make a new plane cut looking for the intersection of the mesh with a plane
        //
        glmPlane plane;
        plane.set(glm::vec3(0,0,offSet), glm::vec3(0,0,1));

        planecut.clear();
        vector<glm::ivec3> faces = mesh.getTriangles();
        for(int i=0;i<faces.size();i++){
            glmTriangle triangle;
            triangle.set(mesh.getVertices()[faces[i][0]],
                         mesh.getVertices()[faces[i][1]],
                         mesh.getVertices()[faces[i][2]]);
            
            IntersectionData id = PlaneTriangleIntersection(plane, triangle);
            if(id.isIntersection){
                planecut.add(id.pos);
                planecut.add(id.pos+id.dir);
            }
        }
        
        //  Move the plane up
        //
        offSet += 1;
        if(offSet>200){
            polygons.clear();
            building.clear();
            offSet = 0;
        }
        
        //  If the plane cut contour aera is significantly (threshold) different from the previus
        //      add it to the stuck
        //
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
    }
    
    if(bRotate){
        rotation += 0.2;
        ofQuaternion latRot,longRot;
        latRot.makeRotate( 70, 1, 0, 0);
        longRot.makeRotate( rotation , 0, 0, 1);
        
        cam.setPosition(latRot * longRot * ofPoint(0,0,1200));
        cam.setOrientation(latRot * longRot);
    }
    
    fade = ofLerp(fade, 1.0, 0.03);
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(255);
//    ofBackgroundGradient(ofFloatColor(0.9), ofFloatColor(1.));
    float scale = 7.;
    float res = 100;
    float dist = 50;

    //  Render
    //
    cam.begin();
    ofPushMatrix();
    ofPushStyle();
    
    //  Floor
    //
    ofPushMatrix();
    
    ofTranslate(-(res*dist)*0.5, -(res*dist)*0.5, -centroid.z*scale);
    floorShader.begin();
    for(int x = 0; x < res; x++){
        for(int y = 0; y < res; y++){
            drawCross(glm::vec3(float(x)*dist,float(y)*dist,0.0),5);
        }
    }
    floorShader.end();
    ofPopMatrix();
    
    //  Models
    //
    ofScale(scale, scale,scale);
    ofTranslate(-centroid);
    
    if(nScene == 0){
        float alpha = powf(fade,2.0);
        if(alpha>0.1){
            glBegin(GL_POINTS);
            for (int i = 0; i < pointcloud.getVertices().size()*alpha; i++) {
                glm::vec4 c = pointcloud.getColors()[i];
                c.a = alpha;
                glColor4fv(&c.r);
                glVertex3fv(&pointcloud.getVertices()[i].x);
            }
            glEnd();
        }
        
    } else if(nScene == 1){
        
        float alpha = powf(1.0-fade,2.0);
        if(alpha>0.1){
            glBegin(GL_POINTS);
            for (int i = 0; i < pointcloud.getVertices().size()*alpha; i++) {
                glm::vec4 c = pointcloud.getColors()[i];
                c.a = alpha;
                glColor4fv(&c.r);
                glVertex3fv(&pointcloud.getVertices()[i].x);
            }
            glEnd();
        }
        
        ofSetColor(0,fade*100);
        glBegin(GL_LINES);
        
        if (mesh.getDrawMode() == TRIANGLES) {
            std::vector<glm::ivec3> triangles = mesh.getTriangles();
            
            for (int i = 0; i < triangles.size()*fade; i++) {
                
                //  TODO: add normals and texcoords
                //
                glVertex3fv( &mesh.getVertices()[triangles[i].x].x );
                glVertex3fv( &mesh.getVertices()[triangles[i].y].x );
                
                glVertex3fv( &mesh.getVertices()[triangles[i].y].x );
                glVertex3fv( &mesh.getVertices()[triangles[i].z].x );
                
                glVertex3fv( &mesh.getVertices()[triangles[i].z].x );
                glVertex3fv( &mesh.getVertices()[triangles[i].x].x );
            }
        } else {
            drawMesh(mesh);
        }
        
        glEnd();

    } else if(nScene == 2){
        
        ofSetColor(0,(1.0-fade)*100);
        glBegin(GL_LINES);
        std::vector<glm::ivec3> triangles = mesh.getTriangles();
        
        for (int i = 0; i < triangles.size()*(1.0-fade); i++) {
            
            //  TODO: add normals and texcoords
            //
            glVertex3fv( &mesh.getVertices()[triangles[i].x].x );
            glVertex3fv( &mesh.getVertices()[triangles[i].y].x );
            
            glVertex3fv( &mesh.getVertices()[triangles[i].y].x );
            glVertex3fv( &mesh.getVertices()[triangles[i].z].x );
            
            glVertex3fv( &mesh.getVertices()[triangles[i].z].x );
            glVertex3fv( &mesh.getVertices()[triangles[i].x].x );
        }
        glEnd();
        
        ofEnableLighting();
        light1.enable();
        light2.enable();
        
        ofSetColor(255,255.*fade);
        drawMesh(mesh);
        
        light1.disable();
        light2.disable();
        ofDisableLighting();
        
    } else if(nScene == 3){
        ofPushMatrix();
        ofTranslate(50*fade, 0);
        
        //  Mesh wireframe
        //
        if(bWireframe){
            ofSetLineWidth(1);
            ofSetColor(0,100);
            drawWireMesh(mesh);
        }
        
        
        //  Current intersected outline
        //
        ofSetLineWidth(5);
        ofSetColor(0,200);
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
        
        if(!bWireframe){
            ofSetColor(255);
            drawMesh(mesh);
        }
        
        ofTranslate(-100*fade, 0);
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
        
        light1.disable();
        light2.disable();
        ofDisableLighting();
        ofPopMatrix();
    }
    ofPopStyle();
    ofPopMatrix();
    cam.end();
    
    ofSetColor(0);
    if(nScene == 0){
        ofDrawBitmapString("PointCloud", 15,15);
    } else if(nScene == 1){
        ofDrawBitmapString("Mesh", 15,15);
    } else if(nScene == 2){
        ofDrawBitmapString("Mesh", 15,15);
    } else if(nScene == 3){
        ofDrawBitmapString("Area threshold: "+ ofToString(threshold) + " (UP/DOWN)", 15,15);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == OF_KEY_UP){
        threshold+=10;
    } else if (key == OF_KEY_DOWN){
        threshold-=10;
    } else if (key == ' '){
        nScene++;
        fade = 0;
        if(nScene==4){
            nScene = 0;
        }
    } else if (key == 'w'){
        bWireframe = !bWireframe;
    } else if (key == 'l'){
        bShowLights = !bShowLights;
    } else if (key == 'r'){
       floorShader.load("floor");
    } else if (key == 'f'){
        ofToggleFullscreen();
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
    bRotate = false;
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
