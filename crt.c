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
	glutInitWindowSize(800, 600);
	
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

void spot(float x, float y)
{
  set_uniform2f(prog_point, "xy", x, y);
  set_uniform1f(prog_point, "focus", focus);
  set_uniform1f(prog_point, "intensity", intensity);

  x -= 400;
  y -= 300;
  x /= 400;
  y /= 300;

  glBegin(GL_QUADS);
  glTexCoord2f(0, 0);
  glVertex2f(x-.01, y-.01);
  glTexCoord2f(1, 0);
  glVertex2f(x+.01, y-.01);
  glTexCoord2f(1, 1);
  glVertex2f(x+.01, y+.01);
  glTexCoord2f(0, 1);
  glVertex2f(x-.01, y+.01);
  glEnd();
}

void blat1(float t, int tt) {
  int i;

  t = 0.01* tt;

  t *= 0.7;
  t += 3.0;

  for (i = -300; i < 300; i += 2) {
    spot (400 + i*cos(t),
	  300 + i*sin(t));
  }
}

void blat2(float t, int tt) {
  int x, i;

  x = (5*tt) % 1600;
  if (x >= 800)
    x = 1600-x;

  for (i = 0; i < 800; i++) {
    spot (x+i, (3*i)/4);
    spot (x-i, (3*i)/4);
    spot (799-x-i, 599-(3*i)/4);
    spot (799-x+i, 599-(3*i)/4);
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
      spot (400+u, 300+v);
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
    spot (x >> 26, y >> 26);
  }
}

void (*blat[])(float, int) = { blat1, blat2, blat3, blat4 };

int t2 = 0;

void draw(void) {
	int tt;
	float t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
	t2++;

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
	  demo = (demo + 1) % 4;
	  break;
	}
	glutPostRedisplay();
}

void reshape(GLint w, GLint h)
{
  glClearColor(0.15, 0.2, 0.15, 0.0);
  glClear(GL_COLOR_BUFFER_BIT);
}
