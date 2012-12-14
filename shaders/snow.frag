const int MAX_KERNEL_SIZE = 128;
varying float intensity;

uniform vec4 color;
uniform int arraySize;
uniform vec2 offsets[MAX_KERNEL_SIZE];
uniform float kernel[MAX_KERNEL_SIZE];

uniform sampler2D snowDisplacement;
uniform sampler2D snowTexture;


float computeOffset(vec4 hVec)
{
    return float((hVec.x
            +hVec.y*255
            +hVec.z*255*255));
}
void main()
{
    vec4 col = color;
    vec4 snowSample = texture2D(snowTexture, gl_TexCoord[0].st);
    float actualHeight = float(snowSample.r)*128 + (float(snowSample.g)*128)*255 + (float(snowSample.b)*128)*255*255;

    float heightsSum = 0.0;
    float weightSum = 0.0;
    for( int i = 0; i < arraySize; i++ )
    {
        vec4 sample = texture2D(snowTexture, gl_TexCoord[0].st + offsets[i]);
        float r = sample.r * 128;
        float g = sample.g * 128;
        float b = sample.b * 128;
        float height = r + g * 255 + b * 255 * 255;
        heightsSum += kernel[i] * height;
        weightSum += kernel[i];
    }
    float blurredHeight = float(heightsSum) / float(weightSum);

    col = color + vec4(1.0, 1.0, 1.0, 0.0) * blurredHeight * 0.5;

    //col = color + vec4(1.0, 1.0, 1.0, 0.0) * actualHeight;

    gl_FragColor = col * intensity;
}
