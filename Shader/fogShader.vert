//author Gleb Ostrowski
#version 120
varying vec4 vertexModelView;
void main()
{	
  gl_Position = ftransform();		
  vertexModelView = gl_ModelViewMatrix * gl_Vertex;
  gl_TexCoord[0] = gl_MultiTexCoord0;
}