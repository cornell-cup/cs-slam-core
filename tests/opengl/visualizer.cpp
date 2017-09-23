#include "visualizer.h"

Visualizer* Visualizer::_instance;

void Visualizer::initialize(int res) {
    _instance = new Visualizer(res);
}

Visualizer* Visualizer::instance() {
    if(!_instance) {
      _instance = new Visualizer(4);
    }
    return _instance;
}

Visualizer::Visualizer(int res) {
  _resolution = res;

  char *myargv[0];
  int myargc=0;
  glutInit(&myargc, myargv);

	glutInitWindowPosition(50, 50);
	glutInitWindowSize(500, 500);

  // glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutCreateWindow("point cloud");

  glutDisplayFunc(_display);

	glutMouseFunc(_mouse);
	glutMotionFunc(_mouseMove);

  _init();

  glutMainLoop();
}

Visualizer::visualizePoints(cv::Mat* mat) {

  glutPostRedisplay();
}

Visualizer::~Visualizer() {}

void Visualizer::_display() {
	glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(0.0, 0.0, 5.0,  /* eye is at (0,0,5) */
    0.0, 0.0, 0.0,      /* center is at (0,0,0) */
    0.0, 1.0, 0.);      /* up is in positive Y direction */

	glRotatef(_instance->_xrot, 1.0f, 0.0f, 0.0f);
	glRotatef(_instance->_yrot, 0.0f, 1.0f, 0.0f);

	glBegin(GL_POINTS);
  for (int i = 0; i < _instance->_numPoints; i+=_instance->_resolution){
			// glColor3f(_instance->_points[i][3], points[i][4], points[i][5]);
      glVertex3f(_instance->_pointMat[i][0], _instance->_pointMat[i][1], _instance->_pointMat[i][2]);
  }
  glEnd();

	glFlush();
  glutSwapBuffers();
}

void Visualizer::_init() {
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

void Visualizer::_mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		_instance->_mouseDown = true;

		_instance->_xdiff = x - _instance->_yrot;
		_instance->_ydiff = -y + _instance->_xrot;
	}
	else
		_instance->_mouseDown = false;
}

void Visualizer::_mouseMove(int x, int y) {
	if (_instance->_mouseDown) {
		_instance->_yrot = x - _instance->_xdiff;
		_instance->_xrot = y + _instance->_ydiff;
	}
}