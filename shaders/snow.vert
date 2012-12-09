//#version 110
varying float intensity;
uniform sampler2D displacementMap;

uniform float time;
//uniform vec4 vertex;
    
float computeScale(float t)
{
    return (0.0 +1.0*sin(2.5*t));
}
    
void main()
{

     float scale = computeScale(time);

     vec4 dv;
     //gl_TexCoord[0].xy = gl_MultiTexCoord0.xy;
    // dv =  texture2D( displacementMap, gl_MultiTexCoord0.xy );

     // Flatten the normals to make the lighting a little more realistic
     //vec3 normal = normalize(gl_NormalMatrix * gl_Normal * vec3(1.0, 1.0, 1.0 / scale));
     //vec3 light = normalize(gl_LightSource[0].position - (gl_ModelViewMatrix * vertex)).xyz;
     //intensity = max(0.0, dot(normal, light));
     //vertex.x = mod(time,2.0);//vertex.y+scale*vertex.x;
     //vertex.y = mod(time,2.0);//vertex.y+scale*vertex.x;
     //vertex.z = mod(time,1.0);//vertex.y+scale*vertex.x;

     //gl_Position = gl_ModelViewProjectionMatrix * vertex;

     //float vDisp = texture2D( displacementMap, gl_MultiTexCoord0.xy );

     vec4 v = vec4(gl_Vertex);
     v.y = v.y+v.x;//(dv.x+dv.y+dv.z)*10.0;//v.x/time;
     gl_Position = gl_ModelViewProjectionMatrix * v;
}
