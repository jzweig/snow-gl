//#version 110
varying float intensity;

uniform float time;
uniform sampler2D snowTexture;
uniform bool useDisplacement;
    
float computeOffset(vec4 hVec)
{
    return (hVec.x+hVec.y+hVec.z)*10.0;
}
    
void main()
{


     gl_TexCoord[0].st = gl_MultiTexCoord0.st;

     // Flatten the normals to make the lighting a little more realistic
     //vec3 normal = normalize(gl_NormalMatrix * gl_Normal * vec3(1.0, 1.0, 1.0 / scale));
     //vec3 light = normalize(gl_LightSource[0].position - (gl_ModelViewMatrix * vertex)).xyz;
     //intensity = max(0.0, dot(normal, light));

     //gl_Position = gl_ModelViewProjectionMatrix * vertex;
     vec4 v = vec4(gl_Vertex);
     if(useDisplacement){
         vec4 dv =  texture2D( snowTexture, gl_MultiTexCoord0.xy );
         v.y = v.y+computeOffset(dv);
         gl_Position = gl_ModelViewProjectionMatrix * v;
     } else {
         gl_Position = ftransform();
     }
}
