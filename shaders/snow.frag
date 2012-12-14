varying float intensity;

uniform vec4 color;

uniform sampler2D snowDisplacement;
uniform sampler2D snowTexture;
void main()
{	
    vec4 col = color;
    vec4 snowSample = texture2D(snowTexture, gl_TexCoord[0].st);

    if(snowSample.r != 0 || snowSample.g != snowSample.b != 0){
        col = vec4(235, 235, 235, 255);
    }

    gl_FragColor = col;
}
