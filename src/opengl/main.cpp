#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

float yAngle = 0.f;
float yAngleDiff = 0.0f;
int xOrigin = -1;

float zAngle = 60.f;
float zAngleDiff = 0.0f;
int yOrigin = -1;

void mouseButton(int button, int state, int x, int y) {
  // only start motion if the left button is pressed
  if (button == GLUT_LEFT_BUTTON) {

    // when the button is released
    if (state == GLUT_UP) {
      yAngle += yAngleDiff;
      yAngleDiff = 0;
      xOrigin = -1;

      zAngle += zAngleDiff;
      zAngleDiff = 0;
      yOrigin = -1;
    }
    else  {// state = GLUT_DOWN
      xOrigin = x;
      yOrigin = y;
    }
  }
}

void mouseMove(int x, int y) {
  // this will only be true when the left button is down
  if (xOrigin >= 0) {

    // update camera's direction
    yAngleDiff = (xOrigin-x);
  }
  if (yOrigin >= 0) {
    zAngleDiff = (y-yOrigin);
  }
  glutPostRedisplay();
}

void draw_mesh(void) {
  int i;

  glPushMatrix();
  /* Adjust cube position to be asthetic angle. */
  glTranslatef(0.0, 0.0, -1.0);
  glRotatef(zAngle+zAngleDiff, 1.0, 0.0, 0.0);
  glRotatef(yAngle+yAngleDiff, 0.0, 0.0, 1.0);

  glColor3f(1.f, 0.f, 0.f);
  glBegin(GL_QUADS);
  glVertex3f(1.f, 0.f, 1.f);
  glVertex3f(1.f, 0.f, -1.f);
  glVertex3f(-1.f, 0.f, -1.f);
  glVertex3f(-1.f, 0.f, 1.f);
  glEnd();

  glPopMatrix();
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  draw_mesh();
  glutSwapBuffers();
}

void init(void) {
  /* Use depth buffering for hidden surface elimination. */
  glEnable(GL_DEPTH_TEST);

  /* Setup the view of the cube. */
  glMatrixMode(GL_PROJECTION);
  gluPerspective( /* field of view in degree */ 40.0,
    /* aspect ratio */ 1.0,
    /* Z near */ 1.0, /* Z far */ 10.0);
  glMatrixMode(GL_MODELVIEW);
  gluLookAt(0.0, 0.0, 5.0,  /* eye is at (0,0,5) */
    0.0, 0.0, 0.0,      /* center is at (0,0,0) */
    0.0, 1.0, 0.);      /* up is in positive Y direction */
}

void *loop1 (void *vargp) {
  printf("In loop 1\n");
  int l = 0;
  while (1) {
    int x = 0;
    for (int j = 0; j < 500; j++) {
      for(long long i = 0; i < 1000000; i++) {
        x += 1;
      }
    }
    printf("Done with loop %d\n", l++);
  }

  return NULL;
}

int main(int argc, char **argv) {
  pthread_t tid;
  pthread_create(&tid, NULL, loop1, NULL);

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutCreateWindow("red 3D lighted cube");
  glutDisplayFunc(display);
  // glutIdleFunc(display);
  glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);
  init();
  glutMainLoop();
  pthread_join(tid, NULL);
  
  return 0;             /* ANSI C requires main to return int. */
}