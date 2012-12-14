const int MAX_KERNEL_SIZE = 128;
varying float intensity;

uniform vec4 color;
uniform int arraySize;
uniform vec2 offsets[MAX_KERNEL_SIZE];
uniform float kernel[MAX_KERNEL_SIZE];

uniform sampler2D snowDisplacement;
uniform sampler2D snowTexture;
void main()
{
    vec4 col = color;
    vec4 snowSample = texture2D(snowTexture, gl_TexCoord[0].st);

    float heightsSum = 0;
    float weightSum = 0;
    for( int i = 0; i < arraySize; i++ )
    {
        vec4 sample = texture2D(snowTexture, gl_TexCoord[0].st + offsets[i]);
        int r = int(sample.r * 255);
        int g = int(sample.g * 255);
        int b = int(sample.b * 255);
        int height = r + g * 255 + b * 255 * 255;
        heightsSum += kernel[i] * height;
        weightSum += kernel[i];
    }
    float blurredHeight = float(heightsSum) / float(weightSum);

    col = color + vec4(1.0, 1.0, 1.0, 0.0) * blurredHeight * 100;

/*
    if(snowSample.r != 0 || snowSample.g  != 0 || snowSample.b != 0){
        col = color + vec4(0.2,0.2,0.2,0.0);
    }

    col = vec4(int(snowSample.r*255) > 1 ? 1 : 0, snowSample.g ? 1 : 0, snowSample.b ? 1 : 0, 1);
*/
    gl_FragColor = col;
}
