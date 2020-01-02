#version 130

precision mediump float;

uniform vec2 xy;
uniform float focus;
uniform float intensity;
uniform sampler2D phosphor;

float gaussian(void)
{
  float x2 = gl_FragCoord.x - xy.x;
  float y2 = gl_FragCoord.y - xy.y;
  x2 *= x2;
  y2 *= y2;
  return intensity*focus*exp(-focus*(x2+y2));
}

/* This models P7 phosphor.  The incident electron beam only excites
   the blue layer.  The yellow layer is mostly insensitive to
   electrons. */

void main(void)
{
  int i;
  vec2 rg = texture2D(phosphor, gl_FragCoord.xy / 1024.0).rg;
  rg.g += gaussian();
  gl_FragColor = vec4(rg,0.0,1.0);
}
