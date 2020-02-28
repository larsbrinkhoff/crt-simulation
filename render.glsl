#version 130

precision mediump float;

const vec2 resolution = vec2(800.0, 600.0);

uniform float time;
uniform sampler2D phosphor;

void main(void)
{
    vec2 xy = gl_FragCoord.xy/1024.0;
    float i1 = texture2D(phosphor, xy).r;
    float i2 = texture2D(phosphor, xy).g;
    vec3 c1 = 0. * i1 * vec3(0.3,0.4,0.0);
    vec3 c2 = i2 * vec3(0.3,0.9,0.0);
    vec3 c3 = vec3(0.15, 0.2, 0.15);
    gl_FragColor = vec4(c1+c2+c3,1.0);
}
