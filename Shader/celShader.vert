//author Gleb Ostrowski
#version 120
varying vec3 normalModelView;
varying vec4 vertexModelView;
uniform bool zAnimation;
uniform bool xAnimation;
uniform bool yAnimation;
uniform float osg_FrameTime;
void main()
{	
	normalModelView = gl_NormalMatrix * gl_Normal;

	gl_TexCoord[0] = gl_MultiTexCoord0;

	vec4 vertexPos = gl_Vertex;
	if(zAnimation){
		vertexPos.z += sin(2.5*vertexPos.z + osg_FrameTime)*0.25;	
	}
	if(xAnimation){
		vertexPos.x += sin(vertexPos.z + osg_FrameTime);
		vertexPos.y += cos(vertexPos.z +osg_FrameTime);	
	}
	if(yAnimation){
		vertexPos.x += -sin(vertexPos.z + osg_FrameTime);
		vertexPos.y += -cos(vertexPos.z +osg_FrameTime);	
	}
	vertexModelView = gl_ModelViewMatrix * vertexPos;
	gl_Position = gl_ModelViewProjectionMatrix * vertexPos;		

}