#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <unistd.h>

GLfloat points[4][6] = {
	{0,0,0,0,0,0},
	{0,1,0,1,0,0},
	{1,0,0,1,1,0},
	{0,0,1,0,1,0}
};
int numPoints = 4;

bool fullscreen = false;
bool mouseDown = false;

float xrot = 0.0f;
float yrot = 0.0f;

float xdiff = 0.0f;
float ydiff = 0.0f;

void drawPoints() {
	glBegin(GL_POINTS);
  for ( int i = 0; i < numPoints; ++i ){
			glColor3f(points[i][3], points[i][4], points[i][5]);
      glVertex3f(points[i][0], points[i][1], points[i][2]);
  }
  glEnd();
}

void display() {
	glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(0.0, 0.0, 5.0,  /* eye is at (0,0,5) */
    0.0, 0.0, 0.0,      /* center is at (0,0,0) */
    0.0, 1.0, 0.);      /* up is in positive Y direction */

	glRotatef(xrot, 1.0f, 0.0f, 0.0f);
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);

  drawPoints();

	glFlush();
  glutSwapBuffers();
}

void init() {
	glEnable(GL_POINT_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glPointSize(6.0);

  /* Use depth buffering for hidden surface elimination. */
  // glEnable(GL_DEPTH_TEST);

  /* Setup the view of the cube. */
  glMatrixMode(GL_PROJECTION);
  gluPerspective( /* field of view in degree */ 40.0,
    /* aspect ratio */ 1.0,
    /* Z near */ 1.0, /* Z far */ 10.0);
  glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		mouseDown = true;

		xdiff = x - yrot;
		ydiff = -y + xrot;
	}
	else
		mouseDown = false;
}

void mouseMotion(int x, int y) {
	if (mouseDown) {
		yrot = x - xdiff;
		xrot = y + ydiff;

		glutPostRedisplay();
	}
}

void idleFunc() {
	std::cout << "idle" << std::endl;
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);

	glutInitWindowPosition(50, 50);
	glutInitWindowSize(500, 500);

  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutCreateWindow("point cloud");

  glutDisplayFunc(display);

	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);

  init();

	glutIdleFunc(idleFunc);

	glutMainLoop();

  return 0;
}
