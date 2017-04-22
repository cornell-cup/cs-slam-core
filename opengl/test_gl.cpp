#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <unistd.h>


#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

boost::interprocess::mapped_region shared_region;

bool fullscreen = false;
bool mouseDown = false;

float xrot = 0.0f;
float yrot = 0.0f;

float xdiff = 0.0f;
float ydiff = 0.0f;

void drawPoints() {
	glBegin(GL_POINTS);
	int size = static_cast<int>(shared_region.get_size());
	size = size/4;
	float *points = static_cast<float*>(shared_region.get_address());
  for (int i = 0; i < size; i+=4 ){
			glVertex3f(points[i], points[i+1], points[i+2]);
			int c = ((int) points[i+3]);
      glColor3f(
				(c & 0xff)/255.f,
				((c >> 8) & 0xff)/255.f,
				((c >> 16) & 0xff)/255.f
			);
  }
  glEnd();
}

void display() {
	// std::cout << "display" << std::endl;

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
  glPointSize(1.0);

  /* Use depth buffering for hidden surface elimination. */
  // glEnable(GL_DEPTH_TEST);

  /* Setup the view of the cube. */
  glMatrixMode(GL_PROJECTION);
  gluPerspective( /* field of view in degree */ 40.0,
    /* aspect ratio */ 1.0,
    /* Z near */ 1.0, /* Z far */ 10.0);
  glMatrixMode(GL_MODELVIEW);

	try {
		boost::interprocess::shared_memory_object shared_points_object = boost::interprocess::shared_memory_object(
			boost::interprocess::open_only,
			"SharedPoints",
			boost::interprocess::read_only
		);

		//Map the whole shared memory in this process
		shared_region = boost::interprocess::mapped_region(shared_points_object, boost::interprocess::read_only);

		std::cout << "initialized point memory" << std::endl;
	} catch (const std::exception&) {
		std::cout << "Points DNE" << std::endl;
	}
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

		// glutPostRedisplay();
	}
}


void glutTimer(int value){
	glutPostRedisplay();
	glutTimerFunc(1, glutTimer, 1);
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

	// glutIdleFunc(idleFunc);
	glutTimerFunc(10, glutTimer, 1);

	glutMainLoop();

  return 0;
}
