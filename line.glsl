#version 130

precision mediump float;

uniform vec2 xy1;
uniform vec2 xy2;
uniform float focus;
uniform float intensity;
uniform sampler2D phosphor;

float gaussian(void)
{
  vec2 delta = xy2 - xy1;
  float d2;

  /*
  float x2 = gl_FragCoord.x - xy1.x;
  float y2 = gl_FragCoord.y - xy1.y;
  x2 *= x2;
  y2 *= y2;
  */

  d2 = delta.y * gl_FragCoord.x -
       delta.x * gl_FragCoord.y +
       xy2.x * xy1.y - xy2.y * xy1.x;
  d2 *= d2;
  d2 /= dot(delta, delta);

  //d2 = max (d2, x2+y2);
  return intensity*focus*exp(-focus*d2);
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
