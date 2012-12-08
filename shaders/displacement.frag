varying float intensity;

void main()
{	
    vec4 color = vec4(1, 0.5, 0.5, 1);
    gl_FragColor = color * intensity;
}
