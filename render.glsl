#version 130

const vec2 resolution = vec2(800.0, 600.0);

uniform float time;
uniform sampler2D phosphor;

void main(void)
{
    vec2 xy = vec2(gl_FragCoord.x/1024.0, gl_FragCoord.y/1024.0);
    float i1 = texture2D(phosphor, xy).r;
    float i2 = texture2D(phosphor, xy).g;
    vec3 c1 = i1 * vec3(0.3,0.4,0.0);
    vec3 c2 = i2 * vec3(0.3,0.4,1.0);
    gl_FragColor = vec4(c1+c2,1.0);
}
