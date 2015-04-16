//author Gleb Ostrowski
#version 120
varying vec4 vertexModelView;
uniform sampler2D texture0;
uniform sampler2D deepth;
uniform vec3 fogColor;
uniform float zNear;
uniform float zFar;
uniform float osg_FrameTime;

float linearDepth(float z){
    return (2.0 * (zNear+zFar)) / ((zFar + zNear) - z * (zFar - zNear));// -1.0;	
}

void main(void){
	//Literature
	//http://www.ozone3d.net/tutorials/glsl_fog/p04.php and depth_of_field example OSG Cookbook
     //http://en.wikibooks.org/wiki/OpenGL_Programming/Post-Processing
	vec4 texCoord = gl_TexCoord[0];
    texCoord.x +=  sin(texCoord.y * 4*2*3.14159 + osg_FrameTime) / 100;
    vec2 deepthPoint = texCoord.xy;
	float z = texture2D(deepth, deepthPoint).x;
	//fogFactor = (end - z) / (end - start)
	z = linearDepth(z); 
	float fogFactor = (4000*4-z) / (4000*4 - 30*4);
	fogFactor = clamp(fogFactor, 0.0, 1.0);

	vec4 texColor = texture2D(texture0,texCoord.xy);
	gl_FragColor = mix(vec4(fogColor,1.0), texColor,fogFactor);

}

