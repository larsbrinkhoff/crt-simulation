#version 130

precision mediump float;

uniform float size;
uniform sampler2D phosphor;

void main(void)
{
  float r = 0.7 * texture2D(phosphor, gl_FragCoord.xy / size).r;
  float g = 0.7 * texture2D(phosphor, gl_FragCoord.xy / size).g;
  float b = 0.7 * texture2D(phosphor, gl_FragCoord.xy / size).b;
  gl_FragColor = vec4(r,g,b,1.0);
}
