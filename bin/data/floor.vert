#version 120

varying vec4 vertexPos;
varying vec3 vertexNormal;

void main() {
	gl_TexCoord[0] = gl_MultiTexCoord0;
	vertexPos = gl_Vertex - vec4(2500.,2500.,0,0);
	vertexNormal = normalize(gl_NormalMatrix * gl_Normal);
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
	gl_FrontColor = gl_Color;
}