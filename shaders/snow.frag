varying float intensity;

uniform vec4 color;

uniform sampler2D snowTexture;
void main()
{	
    //vec4 col = vec4(.4,.8,.4,1.0);
    vec4 snowSample = texture2D(snowTexture, gl_TexCoord[0].st);
    gl_FragColor = snowSample;//col;
}
