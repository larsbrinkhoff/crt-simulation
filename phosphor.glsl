#version 130

precision mediump float;

uniform sampler2D phosphor;

/* This models P7 phosphor.  The yellow layer is excited by light from
   the blue layer. */

void main(void)
{
  float r = 0.99 * texture2D(phosphor, gl_FragCoord.xy / 1024.0).r;
  float g = 0.83 * texture2D(phosphor, gl_FragCoord.xy / 1024.0).g;
  r += 0.2 * g;
  gl_FragColor = vec4(r,g,0.0,1.0);
}
