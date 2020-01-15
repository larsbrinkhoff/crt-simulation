#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/glut.h>
#include "util.h"

void draw(void);
void reshape(GLint, GLint);
void idle_handler(void);
void key_handler(unsigned char key, int x, int y);

int demo = 0;
float focus = 1.0;
float intensity = 2.0;
unsigned int prog_point;
unsigned int prog_phosphor;
unsigned int prog_render;
GLuint fbo;
GLuint tex[2];

int fd;
unsigned int buffer[1000];
int points = 0;

GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};

static void make_texture (int t)
{
	glBindTexture(GL_TEXTURE_2D, t);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F,
		     1024, 1024, 0, GL_RG, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

int main(int argc, char **argv) {
	/* initialize glut */
	glutInitWindowSize(1024, 1024);
	
	fprintf (stderr, "Pass -S to start a network server on port 3400.\n\n");
	fprintf (stderr, "Type space to cyle between modes:\n");
	fprintf (stderr, " - Radar.\n");
	fprintf (stderr, " - Rectangle thingy.\n");
	fprintf (stderr, " - Swirling lines.\n");
	fprintf (stderr, " - Munching squares.\n");
	fprintf (stderr, " - Network data.\n\n");
	fprintf (stderr, "Type q to quit.\n");
	fprintf (stderr, "Type i/I to change intensity.\n");
	fprintf (stderr, "Type f/F to change focus.\n");

	if (argc >= 2 && strcmp (argv[1], "-S") == 0) {
	  fprintf (stderr, "Waiting for connection.\n");
	  fd = serve(3400);
	  argc--;
	  argv++;
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("CRT");

	glutDisplayFunc(draw);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle_handler);
	glutKeyboardFunc(key_handler);

	glGenTextures(2, tex);
	make_texture(tex[0]);
	make_texture(tex[1]);
	
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			       GL_TEXTURE_2D, tex[1], 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return EXIT_FAILURE;

	if(!(prog_phosphor = setup_shader("phosphor.glsl"))) {
		return EXIT_FAILURE;
	}
	if(!(prog_point = setup_shader("point.glsl"))) {
		return EXIT_FAILURE;
	}
	if(!(prog_render = setup_shader("render.glsl"))) {
		return EXIT_FAILURE;
	}

	glutPostRedisplay();
	glutMainLoop();
	return 0;
}

void spot(float x, float y, float i)
{
  set_uniform2f(prog_point, "xy", x + .5, y + .5);
  set_uniform1f(prog_point, "focus", focus);
  set_uniform1f(prog_point, "intensity", i);

  x -= 512;
  y -= 512;
  x /= 512;
  y /= 512;

  glBegin(GL_QUADS);
  glTexCoord2f(0, 0);
  glVertex2f(x-.1, y-.1);
  glTexCoord2f(1, 0);
  glVertex2f(x+.1, y-.1);
  glTexCoord2f(1, 1);
  glVertex2f(x+.1, y+.1);
  glTexCoord2f(0, 1);
  glVertex2f(x-.1, y+.1);
  glEnd();
}

void blat1(float t, int tt) {
  int i;

  t = 0.01* tt;

  t *= 0.7;
  t += 3.0;

  for (i = -500; i < 500; i += 2) {
    float x = 512 + i*cos(t);
    float y = 512 + i*sin(t);
    if ((tt % 200) < 100)
      spot (x, y, intensity);
    else
      spot ((int)(x + .5), (int)(y + .5), intensity);
  }
}

void blat2(float t, int tt) {
  int x, i;

  x = (5*tt) % 2048;
  if (x >= 1024)
    x = 2048-x;

  for (i = 0; i < 1024; i++) {
    spot (x+i, i, intensity);
    spot (x-i, i, intensity);
    spot (1023-x-i, 1024-i, intensity);
    spot (1023-x+i, 1024-i, intensity);
  }
}

void blat3(float t, int tt)
{
  float x, r, u, v;
  int y;

  r = 30+20*sin(.03*tt);

  for (x = -10; x < 10; x++) {
    for (y = -200; y < 200; y +=3) {
      u = (r*x)*cos(.005*tt) + (y)*sin(.005*tt);
      v = (y)*cos(.005*tt) + (r*x)*sin(.005*tt);
      if ((tt % 400) < 200)
	spot (512+u, 512+v, intensity);
      else
	spot ((int)(512+u+.5), (int)(512+v+.5), intensity);
    }
  }
}

void blat4(float t, int tt)
{
  static long long x, y, z, v = 0;
  int i;

  for (i = 0; i < 200; i++) {
    x = 01001002LL;
    x += v;
    x &= 0777777777777LL;
    v = x;
    z = y & 0777777LL;
    y >>= 18;
    y |= (x & 0777777LL) << 18;
    x >>= 18;
    x |= z << 18;
    x ^= v;
    spot (x >> 26, y >> 26, intensity);
  }
}

void blat5(float t, int tt)
{
  int i;

  for (i = 0; i < points; i++) {
    int x, y;
    x = buffer[i] & 01777;
    y = (buffer[i] >> 10) & 01777;
    spot (x, y, intensity * 0.1 * ((buffer[i] >> 20) & 7));
  }
}

void (*blat[])(float, int) = { blat1, blat2, blat3, blat4, blat5 };

int t2 = 0;

void draw(void) {
	int tt;
	float t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
	t2++;

	glViewport(0, 0, 1024, 1024);
	gluOrtho2D(-1,1,-1,1);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			       GL_TEXTURE_2D, tex[1], 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[0]);
	glUseProgramObjectARB(prog_phosphor);

	/* Entire display. */
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(-1, -1);
	glTexCoord2f(1, 0);
	glVertex2f(1, -1);
	glTexCoord2f(1, 1);
	glVertex2f(1, 1);
	glTexCoord2f(0, 1);
	glVertex2f(-1, 1);
	glEnd();

	/* Individual points. */
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[1]);
	glUseProgramObjectARB(prog_point);

	blat[demo](t, t2);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgramObjectARB(prog_render);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[1]);
	set_uniform1f(prog_render, "time", glutGet(GLUT_ELAPSED_TIME)/10000.0);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(-1, -1);
	glTexCoord2f(1, 0);
	glVertex2f(1, -1);
	glTexCoord2f(1, 1);
	glVertex2f(1, 1);
	glTexCoord2f(0, 1);
	glVertex2f(-1, 1);
	glEnd();

	glutSwapBuffers();

	/* Swap textures. */
	tt = tex[0];
	tex[0] = tex[1];
	tex[1] = tt;

	int n = transfer (fd, buffer, sizeof buffer);
	if (n > 0) {
	  points = n/4;
	}
}

void idle_handler(void) {
  glutPostRedisplay();
}

void key_handler(unsigned char key, int x, int y) {
	switch(key) {
	case 27:
	case 'q':
	case 'Q':
		exit(0);
		break;
	case 'F':
	  focus *= 1.1;
	  break;
	case 'f':
	  focus /= 1.1;
	  break;
	case 'I':
	  intensity *= 1.1;
	  break;
	case 'i':
	  intensity /= 1.1;
	  break;
	case ' ':
	  demo = (demo + 1) % 5;
	  break;
	}
	glutPostRedisplay();
}

void reshape(GLint w, GLint h)
{
  glClearColor(0.15, 0.2, 0.15, 0.0);
  glClear(GL_COLOR_BUFFER_BIT);
}
