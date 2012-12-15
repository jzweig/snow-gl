const int MAX_KERNEL_SIZE = 128;
varying float intensity;

uniform vec4 color;
uniform int arraySize;
uniform vec2 offsets[MAX_KERNEL_SIZE];
uniform float kernel[MAX_KERNEL_SIZE];

uniform sampler2D snowDisplacement;
uniform sampler2D snowTexture;


varying vec3 vertex;		// The vector from the eye to the vertex
varying vec3 light;		// The normalized vector from the vertex to the light
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

    col = color + vec4(1.0, 1.0, 1.0, 0.0) * blurredHeight * 0.5;

    //col = color + vec4(1.0, 1.0, 1.0, 0.0) * actualHeight;

    //gl_FragColor = col * intensity;








    vec3 n = normalize(normal);
    vec3 l = normalize(light);

    vec3 e = normalize(eye);
    vec3 i = normalize(vertex - eye);
    vec3 h = normalize(l+e);
/*
    float ms = m*m;

    vec3 ve = -reflect(e,n);
    vec4 p = (gl_ModelViewMatrixInverse*vec4(ve.xyz,0));
    vec4 tc = textureCube(cubeMap, p.xyz);


    float costheta =dot(n,l);

    float f = r0 +(1.0-r0)*pow(1.0-cos(costheta),5);


    float nh = dot(n,h);
    float eh = dot(e,h);
    float en = dot(e,n);

    float alpha = acos(nh);
    float d = exp(-(tan(alpha)*tan(alpha))/ms)/(4.0*ms*pow(cos(alpha),4));
    float g1 = (2*nh*en)/eh;
    float g2 = (2*nh*dot(l,n))/eh;
    float g = min(1,min(g1,g2));

    float speccof = (max((d*f*g)/en,0));*/
    col = col*gl_LightSource[0].diffuse;
    float diffcof = max(dot(n,l),0.0);

    gl_FragColor = gl_LightSource[0].ambient +diffcof*col;//+speccof*specular);//(1.0-f)*col+(f)*tc;





}
