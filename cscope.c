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
void special_handler(int key, int x, int y);

int demo = 0;
float focus = .046;
float intensity = 2.85;
unsigned int prog_point;
unsigned int prog_phosphor;
unsigned int prog_render;
GLuint fbo;
GLuint tex[2];

int scale = 1;
int width = 512;
int height = 512;

int fd;
unsigned int buffer[1000];
int points = 0;

GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};

static void make_texture (int t)
{
	glBindTexture(GL_TEXTURE_2D, t);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,
		     width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

int main(int argc, char **argv) {
	scale = 2;
	focus /= scale;
	width *= scale;
	height *= scale;

	/* initialize glut */
	glutInitWindowSize(width, height);
	
	fprintf (stderr, "Pass -S to start a network server on port 4200.\n\n");

	if (argc >= 2 && strcmp (argv[1], "-S") == 0) {
	  fprintf (stderr, "Waiting for connection...");
	  fd = serve(4200);
	  fprintf (stderr, " OK\n");
	  demo = 1;
	  argc--;
	  argv++;
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("Color Scope");

	glutDisplayFunc(draw);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle_handler);
	glutKeyboardFunc(key_handler);
	glutSpecialFunc(special_handler);

	glGenTextures(2, tex);
	make_texture(tex[0]);
	make_texture(tex[1]);
	
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			       GL_TEXTURE_2D, tex[1], 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return EXIT_FAILURE;

	if(!(prog_phosphor = setup_shader("ntsc.glsl"))) {
		return EXIT_FAILURE;
	}
	if(!(prog_point = setup_shader("cscope.glsl"))) {
		return EXIT_FAILURE;
	}
	if(!(prog_render = setup_shader("render2.glsl"))) {
		return EXIT_FAILURE;
	}

	glutPostRedisplay();
	glutMainLoop();
	return 0;
}

void spot(float x, float y, float r, float g, float b)
{
  set_uniform1f(prog_point, "size", width);
  set_uniform2f(prog_point, "xy", scale*x + .5, scale*y + .5);
  set_uniform1f(prog_point, "focus", focus);
  set_uniform3f(prog_point, "intensity",
		intensity * r,  intensity * g, intensity * b);

  x -= 256;
  y -= 256;
  x /= 256;
  y /= 256;

  glUseProgramObjectARB(prog_point);
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

void blat1(float t, int tt)
{
  float x, y;
  t *= 1.0;
  for(tt = 0; tt < 10; tt++) {
    x = 256 + 200*cos(t + tt/100.0);
    y = 256 + 200*sin(t + tt/100.0);
    spot (x, y, 017, 017, 017);
  }
}

void blat6(float t, int tt)
{
  int i;

  for (i = 0; i < points; i++) {
    int x, y, r, g, b;
    x = buffer[i] & 0777;
    y = (buffer[i] >> 10) & 0777;
    r = (buffer[i] >> 28) & 017;
    g = (buffer[i] >> 24) & 017;
    b = (buffer[i] >> 20) & 017;
    //printf("%3o %3o %02o %02o %02o\n", x, y, r, g, b);
    spot (x, y, r, g, b);
  }
}

void (*blat[])(float, int) = { blat1, blat6 };

int t2 = 0;
float last_time = 0;
int frames = 0;

void draw(void) {
	int tt;
	float t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
	frames++;
	t2++;
	if (frames == 3*60) {
	  fprintf (stderr, "FPS = %.1f\n", frames / (t - last_time));
	  last_time = t;
	  frames=0;
	}

	glViewport(0, 0, width, height);
	gluOrtho2D(-1,1,-1,1);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			       GL_TEXTURE_2D, tex[1], 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[0]);
	glUseProgramObjectARB(prog_phosphor);
	set_uniform1f(prog_phosphor, "size", width);

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

	blat[demo](t, t2);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgramObjectARB(prog_render);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[1]);
	set_uniform1f(prog_phosphor, "size", width);

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

	if (fd == -1) {
	  fd = acc();
	  if (fd != -1)
	    fprintf(stderr, "New connection.\n");
	}

	if (fd != -1) {
	  int n = transfer (fd, buffer, sizeof buffer);
	  if (n > 0) {
	    points = n/4;
	  } else {
	    points = 0;
	    if (n == -1) {
	      close(fd);
	      fd = -1;
	      fprintf(stderr, "Lost connection.\n");
	    }
	  }
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
	  demo = (demo + 1) % 2;
	  break;
	}
	glutPostRedisplay();
}

static int wx = -1;
static int wy = -1;
static int ww = -1;
static int wh = -1;

void special_handler(int key, int x, int y) {
	switch(key) {
	case GLUT_KEY_F11:
	  if (wx == -1) {
	    wx = glutGet(GLUT_WINDOW_X);
	    wy = glutGet(GLUT_WINDOW_Y);
	    ww = glutGet(GLUT_WINDOW_WIDTH);
	    wh = glutGet(GLUT_WINDOW_HEIGHT);
	    glutFullScreen();
	  } else {
	    glutPositionWindow(wx, wy);
	    glutReshapeWindow(ww, wh);
	    wx = wy = ww = wh = -1;
	  }
	  break;
	}
	glutPostRedisplay();
}

void reshape(GLint w, GLint h)
{
  glClearColor(0.15, 0.2, 0.15, 0.0);
  glClear(GL_COLOR_BUFFER_BIT);
  glViewport(0, 0, w, h);
}
