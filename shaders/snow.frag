const int MAX_KERNEL_SIZE = 128;
varying float intensity;

uniform vec4 color;
uniform int arraySize;
uniform vec2 offsets[MAX_KERNEL_SIZE];
uniform float kernel[MAX_KERNEL_SIZE];

uniform sampler2D snowDisplacement;
uniform sampler2D snowTexture;


varying vec3 vertex;		// The vector from the eye to the vertex
varying vec3 vertex_to_light;		// The normalized vector from the vertex to the light
varying vec3 eye;		// The normalized vector from the vertex to the eye
varying vec3 normal;		// The normal vector of the vertex, in eye space

uniform samplerCube cubeMap;	// The cube map containing the environment to reflect
uniform vec4 ambient;		// The ambient channel of the color to reflect
uniform vec4 diffuse;		// The diffuse channel of the color to reflect
uniform vec4 specular;		// The specular channel of the color to reflect

uniform float r0;		// The Fresnel reflectivity when the incident angle is 0
uniform float m;		// The roughness of the material
float computeOffset(vec4 hVec)
{
    return hVec.x + hVec.y*256.0 + hVec.z*256.0*256.0;
}
void main()
{
    vec4 col = color;
    vec4 snowSample = texture2D(snowTexture, gl_TexCoord[0].st);
    //float actualHeight = float(snowSample.r)*128 + (float(snowSample.g)*128)*255 + (float(snowSample.b)*128)*255*255;

    float heightsSum = 0.0;
    float weightSum = 0.0;
    for( int i = 0; i < arraySize; i++ )
    {
        vec4 sample = texture2D(snowTexture, gl_TexCoord[0].st + offsets[i]);
        sample.r = sample.r*128.0;
        sample.g = sample.g*128.0;
        sample.b = sample.b*128.0;
        float height = computeOffset(sample);
        heightsSum += kernel[i] * height;
        weightSum += kernel[i];
    }
    float blurredHeight = float(heightsSum) / float(weightSum);

    vec3 n = normalize(normal);
    vec3 l = normalize(vertex_to_light);

    vec3 e = normalize(eye);
    vec3 i = normalize(vertex - eye);
    vec3 h = normalize(vertex_to_light+e);

    float diffcof = clamp(dot(n,vertex_to_light), 0.0, 1.0);

    vec3 positive = vec3(1.0, 1.0, 1.0);

    vec4 color = gl_Color * diffcof;
    color.w = 1.0;

    gl_FragColor = color;
    //gl_FragColor = vec4(diffcof, diffcof, diffcof, 1);
    //gl_FragColor = vec4((n + positive)*0.5, 1);
    //gl_FragColor = vec4(vertex_to_light, 1);
}
