#version 130

precision mediump float;

uniform float size;
uniform sampler2D phosphor;

void main(void)
{
    vec2 xy = gl_FragCoord.xy/size;
    vec3 rgb = texture2D(phosphor, xy).rgb;
    gl_FragColor = vec4(rgb,1.0);
}
