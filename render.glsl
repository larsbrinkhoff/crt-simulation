#version 130

const vec2 resolution = vec2(800.0, 600.0);

uniform float time;
uniform sampler2D phosphor;

void main(void)
{
    vec2 xy = vec2(gl_FragCoord.x/1024.0, gl_FragCoord.y/1024.0);
    vec2 i = texture2D(phosphor, xy).rg;
    gl_FragColor = vec4(i,0.0,1.0);
}
