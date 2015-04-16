//author Gleb Ostrowski
#version 120
varying vec4 vertexModelView;
uniform sampler2D texture0;
uniform sampler2D deepth;
uniform vec3 fogColor;
uniform float zNear;
uniform float zFar;

float linearDepth(float z){
    return (2.0 * (zNear+zFar)) / ((zFar + zNear) - z * (zFar - zNear)) -1.0;	
}

void main(void){
	//Literature
	//http://www.ozone3d.net/tutorials/glsl_fog/p04.php and depth_of_field example OSG Cookbook
	vec2 deepthPoint = gl_TexCoord[0].xy;
	float z = texture2D(deepth, deepthPoint).x;
	//fogFactor = (end - z) / (end - start)
	z = linearDepth(z); 
	float fogFactor = (4000*4-z) / (4000*4 - 30*4);
	fogFactor = clamp(fogFactor, 0.0, 1.0);

	vec4 texColor = texture2D(texture0,gl_TexCoord[0].xy);

	//SEPIA  http://wiki.delphigl.com/index.php/shader_sepia
	vec4 Sepia1 = vec4( 0.2, 0.05, 0.0, 1.0 );    
	vec4 Sepia2 = vec4( 1.0, 0.9, 0.5, 1.0 );
	float SepiaMix = dot(vec3(0.3, 0.59, 0.11), vec3(texColor));
	texColor = mix(texColor, vec4(SepiaMix), vec4(0.5));
	vec4 Sepia = mix(Sepia1, Sepia2, SepiaMix);
	texColor = mix(texColor, Sepia, 1.0);

	gl_FragColor = mix(vec4(fogColor,1.0), texColor,fogFactor) ;

}

