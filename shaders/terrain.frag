
uniform float dirtMin, dirtMax;
uniform float grassMin, grassMax;
uniform float rockMin, rockMax;
uniform float snowMin, snowMax;
    
uniform sampler2D dirtTexture;
uniform sampler2D grassTexture;
uniform sampler2D rockTexture;
uniform sampler2D snowTexture;

varying float height;
varying float intensity;


float computeWeight(float regionMin,float regionMax)
{

    float regionRange = regionMax-regionMin;
    float regionWeight = (regionRange-abs(height - regionMax))/regionRange;
    return max(0.0,regionWeight);
}
vec4 sampleTextures()
{
    float dirtWeight = computeWeight(dirtMin, dirtMax);
    float grassWeight = computeWeight(grassMin, grassMax);
    float rockWeight = computeWeight(rockMin, rockMax);
    float snowWeight = computeWeight(snowMin, snowMax);
    
    vec4 dirtSample = texture2D(dirtTexture, gl_TexCoord[0].st);
    vec4 grassSample = texture2D(grassTexture, gl_TexCoord[0].st);
    vec4 rockSample = texture2D(rockTexture, gl_TexCoord[0].st);
    vec4 snowSample = texture2D(snowTexture, gl_TexCoord[0].st);
    return (dirtSample*dirtWeight + grassSample*grassWeight + rockSample*rockWeight + snowSample*snowWeight);
}

void main()
{   
    gl_FragColor = sampleTextures() * intensity;
}
