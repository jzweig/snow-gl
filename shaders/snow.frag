const int MAX_KERNEL_SIZE = 128;

uniform int arraySize;
uniform vec2 offsets[MAX_KERNEL_SIZE];
uniform float kernel[MAX_KERNEL_SIZE];

uniform sampler2D snowDisplacement;
uniform sampler2D snowTexture;
uniform sampler2D localTexture;
uniform sampler2D snowSurfaceTexture;

uniform bool useLocalTexture;

varying vec3 vertex;		// The vector from the eye to the vertex
varying vec3 vertex_to_light;		// The normalized vector from the vertex to the light
varying vec3 eye;		// The normalized vector from the vertex to the eye
varying vec3 normal;		// The normal vector of the vertex, in eye space
varying vec3 origNormal;

uniform samplerCube cubeMap;	// The cube map containing the environment to reflect
uniform vec4 ambient;		// The ambient channel of the color to reflect
uniform vec4 diffuse;		// The diffuse channel of the color to reflect
uniform vec4 specular;		// The specular channel of the color to reflect

uniform float r0;		// The Fresnel reflectivity when the incident angle is 0
uniform float m;		// The roughness of the material

uniform int tesselationParam;           // The tesselation parameter for the shape being shaded

float computeOffset(vec4 hVec)
{
    return hVec.x + hVec.y*256.0 + hVec.z*256.0*256.0;
}
void main()
{
    // Light vectors
    vec3 n = normalize(normal);
    vec3 l = normalize(vertex_to_light);
    vec3 e = normalize(eye);
    vec3 i = normalize(vertex - eye);
    vec3 h = normalize(vertex_to_light+e);

    // Sample the snow height at this position. This isn't actually used.
    vec4 snowSample = texture2D(snowTexture, gl_TexCoord[0].st);
    float actualHeight = float(snowSample.r)*128.0 + (float(snowSample.g)*128.0)*255.0 + (float(snowSample.b)*128.0)*255.0*255.0;

    // Blur over the surrounding areas
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
    // Compute the actual blurred height
    float blurredHeight = float(heightsSum) / float(weightSum);

    // From the blurred height, determine the color contribution of the snow
    float snowIntensity = min(0.9, actualHeight)*dot(normalize(origNormal), vec3(0,1.0,0)) + min(0.1,actualHeight);
    vec4 snowColor = texture2D(snowSurfaceTexture, gl_TexCoord[0].st*(float(tesselationParam) / 10.0));

    // Calculate the diffuse coefficient
    float diffuseCoefficient = clamp(dot(n,vertex_to_light), 0.0, 1.0);

    // Compute the local color
    vec4 localColor;
    if( useLocalTexture ) {
        localColor = texture2D(localTexture, gl_TexCoord[0].st * (float(tesselationParam) / 10.0));
    } else {
        localColor = gl_Color;
    }

    // Compute the diffuse color from the object's natural color and the
    // snow's contribution
    vec4 diffuseColor = localColor * (1.0 - snowIntensity) + snowColor * snowIntensity;

    // Compute the final color
    vec4 ambientColor = vec4(0.2*diffuseColor.r, 0.2*diffuseColor.g, 0.2*diffuseColor.b, 0);
    vec4 finalColor = ambientColor + (diffuseColor * diffuseCoefficient);

    // Alpha values will have been reduced from multiplying by the diffuse coefficient,
    // so we need to bring them back up to the material's opacity.
    finalColor.w = gl_Color.w;

    // Set the final color
    gl_FragColor = finalColor;
    //gl_FragColor.rgb = normal;
    //gl_FragColor.a = 1.0;

    // Useful for debugging lighting:
    //
    //gl_FragColor = vec4(diffcof, diffcof, diffcof, 1);
    //gl_FragColor = vec4((n + vec4(1, 1, 1, 1))*0.5, 1);
    //gl_FragColor = vec4(vertex_to_light, 1);
}
