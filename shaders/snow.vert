//#version 110

uniform float time;
uniform bool useDisplacement;

uniform sampler2D snowDisplacement;
uniform sampler2D snowTexture;

varying vec3 vertex;	// The position of the vertex, in eye space
varying vec3 vertex_to_light;	// The normalized vector from the vertex to the light
varying vec3 eye;	// The normalized vector from the vertex to the eye
varying vec3 normal;	// The normal vector of the vertex, in eye space
varying vec3 origNormal;

uniform int tesselationParam;     // The tesselation parameter for the shape being shaded
uniform vec3 scale;           // The scale parameter for the shape being shaded

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
     normal = origNormal = normalize(gl_NormalMatrix * gl_Normal);


     gl_FrontColor = gl_Color;
     //gl_Position = gl_ModelViewProjectionMatrix * vertex;
     vec4 v = vec4(gl_Vertex);
     if(useDisplacement){
         vec2 texCoords = gl_MultiTexCoord0.st;
         //if(texCoords.s>0.0 && gl_MultiTexCoord0.s <1.0 && gl_MultiTexCoord0.t >0.0 && gl_MultiTexCoord0.t < 1.0){
             float texOffset = 1.0/float(tesselationParam);
             float c1 = computeOffset(texture2D( snowDisplacement, vec2(texCoords.s-texOffset,texCoords.t-texOffset)));
             float c2 = computeOffset(texture2D( snowDisplacement, vec2(texCoords.s-texOffset,texCoords.t+texOffset)));
             float c3 = computeOffset(texture2D( snowDisplacement, vec2(texCoords.s+texOffset,texCoords.t+texOffset)));
             float c4 = computeOffset(texture2D( snowDisplacement, vec2(texCoords.s+texOffset,texCoords.t-texOffset)));
             float deltax = 1.0/(scale.x*float(tesselationParam));//0.001;//20.0/float(tesselationParam);
             float deltaz = 1.0/(scale.z*float(tesselationParam));//0.001;//20.0/float(tesselationParam);
             vec3 v1 = normalize(vec3(-deltax,c1,-deltaz));
             vec3 v2 = normalize(vec3(-deltax,c2,deltaz));
             vec3 v3 = normalize(vec3(deltax,c3,deltaz));
             vec3 v4 = normalize(vec3(deltax,c4,-deltaz));
             vec3 v12 = normalize(cross(v1,v2));
             if(v12.y < 0.0){ v12 = -v12;}
             vec3 v23 = normalize(cross(v2,v3));
             if(v23.y < 0.0){ v23 = -v23;}
             vec3 v34 = normalize(cross(v3,v4));
             if(v34.y < 0.0){ v34 = -v34;}
             vec3 v41 = normalize(cross(v4,v1));
             if(v41.y < 0.0){ v41 = -v41;}
             vec3 vnorm = normalize(v12+v23+v34+v41);
             normal = normalize(gl_NormalMatrix * vnorm);

         //}

         vec4 dv =  texture2D( snowDisplacement, gl_MultiTexCoord0.st );
         v.y = v.y+computeOffset(dv);
         gl_Position = gl_ModelViewProjectionMatrix * v;
     } else {
         gl_Position = ftransform();
     }

}
