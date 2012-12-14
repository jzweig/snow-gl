varying float intensity;

uniform vec4 color;

uniform sampler2D snowDisplacement;
uniform sampler2D snowTexture;
void main()
{	
    vec4 col = color;
    vec4 snowSample = texture2D(snowTexture, gl_TexCoord[0].st);

    if(snowSample.r != 0 || snowSample.g  != 0 || snowSample.b != 0){
        col = color + vec4(0.2,0.2,0.2,0.0);
    }

    gl_FragColor = col;
}
