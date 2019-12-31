#version 130

uniform vec2 xy;
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
  float r = texture2D(phosphor, gl_FragCoord.xy / 1024.0).r;
  float g = texture2D(phosphor, gl_FragCoord.xy / 1024.0).g;
  float d = dist(xy.x, xy.y);
  gl_FragColor = vec4(r+d,g+d,0.0,1.0);
}
