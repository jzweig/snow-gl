varying float intensity;

uniform float time;
//uniform vec4 vertex;
    
float computeScale(float t)
{
    return 10*(.7 +.3*sin(2.5*t));
}
    
void main()
{

     float scale = computeScale(time);
    vec4 vertex = gl_Vertex;


    // Flatten the normals to make the lighting a little more realistic
     vec3 normal = normalize(gl_NormalMatrix * gl_Normal * vec3(1.0, 1.0, 1.0 / scale));
     vec3 light = normalize(gl_LightSource[0].position - (gl_ModelViewMatrix * vertex)).xyz;
     intensity = max(0.0, dot(normal, light));

    vertex.y = vertex.y+scale*vertex.x;
    gl_Position = gl_ModelViewProjectionMatrix * vertex;

} 
