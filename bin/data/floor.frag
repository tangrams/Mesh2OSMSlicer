#version 120

varying vec4 vertexPos;
varying vec3 vertexNormal;

void main() {
	vec4 color = gl_Color;
	color.rgb = vec3(0.0);
	color.a = 1.-pow((length(vertexPos.xy)/1000.0),0.2);

	gl_FragColor = color;
}
