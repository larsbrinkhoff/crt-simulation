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
  int i;
  float r = 0.98 * texture2D(phosphor, gl_FragCoord.xy / 1024.0).r;
  float g = 0.79 * texture2D(phosphor, gl_FragCoord.xy / 1024.0).g;
  float d;
  for (i = 0; i < n; i++) {
    d = dist(points[3*i+0], points[3*i+1]);
    r += points[3*i+2] * d;
    g += points[3*i+2] * d;
  }
  r = min(r, 2.0);
  g = min(g, 2.0);
  gl_FragColor = vec4(r,g,0.0,1.0);
}
