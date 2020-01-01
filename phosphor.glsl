#version 130

precision mediump float;

uniform sampler2D phosphor;

void main(void)
{
  float r = 0.997 * texture2D(phosphor, gl_FragCoord.xy / 1024.0).r;
  float g = 0.83 * texture2D(phosphor, gl_FragCoord.xy / 1024.0).g;
  gl_FragColor = vec4(r,g,0.0,1.0);
}
