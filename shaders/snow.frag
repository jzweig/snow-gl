varying float intensity;

uniform vec4 color;

uniform sampler2D snowDisplacement;
uniform sampler2D snowTexture;
void main()
{	
    vec4 col = color;
    vec4 snowSample = texture2D(snowTexture, gl_TexCoord[0].st);
    if(snowSample.r != 0.0){
        col = snowSample+vec4(0.8,0.8,0.8,0.0);
    }

    gl_FragColor = col;
}
