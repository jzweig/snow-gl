//#version 110

uniform float time;
uniform bool useDisplacement;

uniform sampler2D snowDisplacement;
uniform sampler2D snowTexture;

varying vec3 vertex;	// The position of the vertex, in eye space
varying vec3 vertex_to_light;	// The normalized vector from the vertex to the light
varying vec3 eye;	// The normalized vector from the vertex to the eye
varying vec3 normal;	// The normal vector of the vertex, in eye space


float computeOffset(vec4 hVec)
{
    return (hVec.x+hVec.y*256.0+hVec.z*256.0*256.0) * 0.25;
}
    
void main()
{

     //displacement map unit
     //gl_TexCoord[0] = gl_MultiTexCoord0;
     gl_TexCoord[0].st = gl_MultiTexCoord0.st;
     //bump map unit
     //gl_TexCoord[1] = gl_MultiTexCoord1;
     gl_TexCoord[1].st = gl_MultiTexCoord1.st;

     vec4 modelviewVertex = gl_ModelViewMatrix * gl_Vertex;
     vertex = vec3(modelviewVertex.xyz);
     eye = -vertex;

     vertex_to_light = normalize(gl_LightSource[0].position.xyz-vertex);
     normal = normalize(gl_NormalMatrix * gl_Normal);

     gl_FrontColor = gl_Color;
     //gl_Position = gl_ModelViewProjectionMatrix * vertex;
     vec4 v = vec4(gl_Vertex);
     if(useDisplacement){
         vec4 dv =  texture2D( snowDisplacement, gl_MultiTexCoord0.st );
         v.y = v.y+computeOffset(dv);
         gl_Position = gl_ModelViewProjectionMatrix * v;
     } else {
         gl_Position = ftransform();
     }

}
