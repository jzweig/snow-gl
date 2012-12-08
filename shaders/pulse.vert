varying float intensity;

uniform float time;
//uniform vec4 vertex;
    
float computeScale(float t)
{
    return (.7 +.3*sin(2.5*t));
}
    
void main()
{

     float scale = computeScale(time);
    vec4 vertex = gl_Vertex;


    // Flatten the normals to make the lighting a little more realistic
     vec3 normal = normalize(gl_NormalMatrix * gl_Normal * vec3(1.0, 1.0, 1.0 / scale));
     vec3 light = normalize(gl_LightSource[0].position - (gl_ModelViewMatrix * vertex)).xyz;
     intensity = max(0.0, dot(normal, light));
     //vertex.x = mod(time,2.0);//vertex.y+scale*vertex.x;
     vertex.y = mod(time,2.0);//vertex.y+scale*vertex.x;
     //vertex.z = mod(time,2.0);//vertex.y+scale*vertex.x;

    //gl_Position = gl_ModelViewProjectionMatrix * vertex;

                vec4 v = vec4(gl_Vertex);
                v.z = sin(5.0*v.x )*0.25;
                gl_Position = gl_ModelViewProjectionMatrix * v;
} 
