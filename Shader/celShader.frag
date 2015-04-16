//author Gleb Ostrowski
#version 120 
#define NUM_LIGHTS 6
uniform sampler2D texture0;
uniform sampler2D toonTex;
uniform float osg_FrameTime;
uniform bool tex;
varying vec3 normalModelView;
varying vec4 vertexModelView;

vec4 calculateLightFromLightSource(int lightIndex, bool front){
	vec3 lightDir;
	vec3 eye = normalize(-vertexModelView.xyz);
	vec4 curLightPos = gl_LightSource[lightIndex].position;
	//curLightPos.z = sin(10*osg_FrameTime)*4+curLightPos.z;
	lightDir = normalize(curLightPos.xyz - vertexModelView.xyz);

	float dist = distance( gl_LightSource[lightIndex].position, vertexModelView );
	float attenuation =  1.0 / (gl_LightSource[lightIndex].constantAttenuation
				 + gl_LightSource[lightIndex].linearAttenuation * dist 
				 + gl_LightSource[lightIndex].quadraticAttenuation * dist * dist);

	float z = length(vertexModelView);
	vec4 color = vec4(0.0);
	vec3 n = normalize(normalModelView);
	vec3 nBack = normalize(-normalModelView);
	float intensity = dot(n,lightDir); //NdotL, Lambert
	float intensityBack = dot(nBack,lightDir); //NdotL, Lambert
	//-Phong Modell
	vec3 reflected = normalize(reflect( -lightDir, n));
	float specular = pow(max(dot(reflected, eye), 0.0), gl_FrontMaterial.shininess);
	vec3 reflectedBack = normalize(reflect( -lightDir, nBack));
	float specularBack = pow(max(dot(reflectedBack, eye), 0.0), gl_BackMaterial.shininess);
	//Toon-Shading
	//2D Toon http://www.cs.rpi.edu/~cutler/classes/advancedgraphics/S12/final_projects/hutchins_kim.pdf		
	vec4 toonColor = texture2D(toonTex,vec2(intensity,specular));
	vec4 toonColorBack = texture2D(toonTex,vec2(intensityBack,specularBack));
	if(front){	
		color += gl_FrontMaterial.ambient * gl_LightSource[lightIndex].ambient;
		if(intensity > 0.0){
			color += gl_FrontMaterial.diffuse * gl_LightSource[lightIndex].diffuse * intensity * attenuation ;
			color += gl_FrontMaterial.specular * gl_LightSource[lightIndex].specular * specular *attenuation ;
		}
		return color  * toonColor;
	} else {//back	
		color += gl_BackMaterial.ambient * gl_LightSource[lightIndex].ambient;
		if(intensity > 0.0){
			color += gl_BackMaterial.diffuse * gl_LightSource[lightIndex].diffuse * intensityBack * attenuation ;
			color += gl_BackMaterial.specular * gl_LightSource[lightIndex].specular * specularBack *attenuation ;
		}
		return color  * toonColorBack;
	}	
}

void main(void) {
	vec4 color = vec4(0.0);
	bool front = true;
	//non-uniform-flow error correction
	//happens sometome (mostyl on NVIDIA)
	//see more here: http://www.opengl.org/wiki/GLSL_Sampler#Non-uniform_flow_control
	//and here: http://gamedev.stackexchange.com/questions/32543/glsl-if-else-statement-unexpected-behaviour
	vec4 texColor = texture2D(texture0,gl_TexCoord[0].xy);
	if(!gl_FrontFacing)
		front = false;
	for(int i = 0; i< NUM_LIGHTS; i++){
		color += calculateLightFromLightSource(i,front);
		}
	if(tex)	
		gl_FragColor =color * texColor;
	else
		gl_FragColor = color;
  }
