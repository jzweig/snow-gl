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

    float heightsSum = 0.0;
    float weightSum = 0.0;
    for( int i = 0; i < arraySize; i++ )
    {
        vec4 sample = texture2D(snowTexture, gl_TexCoord[0].st + offsets[i]);
        float height = computeOffset(sample);//sample.r + sample.g * 255 + sample.b * 255 * 255;
        heightsSum += kernel[i] * height;
        weightSum += kernel[i];
    }
    float blurredHeight = float(heightsSum) / float(weightSum);

/*
    if( blurredHeight >= 0.1 )
        col = color + vec4(0.2,0.2,0.2,0.0);
*/

    if(snowSample.r != 0.0 || snowSample.g  != 0.0 || snowSample.b != 0.0){
        col = snowSample + vec4(0.2,0.2,0.2,0.0);
    }

  //  col = vec4(snowSample.r > 1 ? 1 : 0, snowSample.g ? 1 : 0, snowSample.b ? 1 : 0, 1);

    gl_FragColor = col;
}
