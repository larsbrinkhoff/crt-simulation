#version 130

const vec2 resolution = vec2(800.0, 600.0);

uniform float time;
uniform sampler2D phosphor;

void main(void)
{
    vec3 color = vec3(0.0,0.0,0.0);
    vec2 xy = vec2(gl_FragCoord.x/1024.0, gl_FragCoord.y/1024.0);
    float i = texture2D(phosphor, xy).r;
    color += vec3(i, i, i);
    gl_FragColor = vec4(color,1.0);
}
