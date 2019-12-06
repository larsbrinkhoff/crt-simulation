#version 130

uniform int n;
uniform float points[10000];
uniform sampler2D phosphor;

float dist (float x, float y)
{
  float x2 = gl_FragCoord.x - x;
  float y2 = gl_FragCoord.y - y;
  x2 *= x2;
  y2 *= y2;
  return 1.0 / (x2 + y2);
}

void main(void)
{
  float i = 0.9 * texture2D(phosphor, gl_FragCoord.xy / 1024.0).r;
  i += points[2] * dist(points[0], points[1]);
  gl_FragColor = vec4(i,0.0,0.0,1.0);
}
