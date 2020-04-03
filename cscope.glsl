#version 130

precision mediump float;

uniform float size;
uniform vec2 xy;
uniform float focus;
uniform vec3 intensity;
uniform sampler2D phosphor;

float gaussian(void)
{
  float x2 = gl_FragCoord.x - xy.x;
  float y2 = gl_FragCoord.y - xy.y;
  x2 *= x2;
  y2 *= y2;
  return focus*exp(-focus*(x2+y2));
}

void main(void)
{
  int i;
  vec3 rgb = texture2D(phosphor, gl_FragCoord.xy / size).rgb;
  rgb += intensity*gaussian();
  gl_FragColor = vec4(rgb,1.0);
}
