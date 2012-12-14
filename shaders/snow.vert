//#version 110
varying float intensity;

uniform float time;
uniform bool useDisplacement;

uniform sampler2D snowDisplacement;
uniform sampler2D snowTexture;
    
float computeOffset(vec4 hVec)
{
    return float((hVec.z
            +hVec.y*255
            +hVec.x*255*255))*0.00001;
}
    
void main()
{

     //displacement map unit
     //gl_TexCoord[0] = gl_MultiTexCoord0;
     gl_TexCoord[0].st = gl_MultiTexCoord0.st;
     //bump map unit
     //gl_TexCoord[1] = gl_MultiTexCoord1;
     gl_TexCoord[1].st = gl_MultiTexCoord1.st;

     // Flatten the normals to make the lighting a little more realistic
     //vec3 normal = normalize(gl_NormalMatrix * gl_Normal * vec3(1.0, 1.0, 1.0 / scale));
     //vec3 light = normalize(gl_LightSource[0].position - (gl_ModelViewMatrix * vertex)).xyz;
     //intensity = max(0.0, dot(normal, light));
     intensity = 1.0;

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
