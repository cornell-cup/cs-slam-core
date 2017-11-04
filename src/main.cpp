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
#include <unistd.h>

#include "vision_loop.h"

float yAngle = 0.f;
float yAngleDiff = 0.0f;
int xOrigin = -1;

float zAngle = 0.f;
float zAngleDiff = 0.0f;
int yOrigin = -1;

int windowWidth = 1080;
int windowHeight = 720;

VisionLoop visionLoop;

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
    else  {
      xOrigin = x;
      yOrigin = y;
    }
  }
}

void mouseMove(int x, int y) {
  // this will only be true when the left button is down
  if (xOrigin >= 0) {

    // update camera's direction
    yAngleDiff = (x-xOrigin);
  }
  if (yOrigin >= 0) {
    zAngleDiff = (y-yOrigin);
  }
  
  // TODO may not need
  // glutPostRedisplay();
}

void draw_mesh(void) {
  int i;

  glPushMatrix();
  /* Adjust cube position to be asthetic angle. */
  glTranslatef(0.0, 0.0, -3.0);
  glRotatef(zAngle+zAngleDiff, 1.0, 0.0, 0.0);
  glRotatef(yAngle+yAngleDiff, 0.0, 1.0, 0.0);

  // glColor3f(1.f, 0.f, 0.f);
  // glBegin(GL_QUADS);
  // glVertex3f(1.f, 1.f, 0.f);
  // glVertex3f(1.f, -1.f, 0.f);
  // glVertex3f(-1.f, -1.f, 0.f);
  // glVertex3f(-1.f, 1.f, 0.f);
  // glEnd();

  visionLoop.mesh_lock.lock();
  // printf("%lu\n", visionLoop.meshGenerator.getMeshes()->size());

  int h = 480;
  int w = 640;

  float scale = 5.0;
  int brighten = 50;
  float z_trans = 3.0;

  bool point_cloud = false;

  float projectMat[3];

  if (point_cloud){
    glPointSize(4);
    glBegin(GL_POINTS);
  }

  for (int r = 0; r < visionLoop.meshGenerator.getMeshes()->size(); r++) {
    
    int colorr = (char) (r*333);
    int colorg = (char) (r*651);
    int colorb = (char) (r*17);
    colorr = std::max(colorr, (colorr+brighten)%255);
    colorg = std::max(colorg, (colorg+brighten)%255);
    colorg = std::max(colorb, (colorb+brighten)%255);

    glColor3f(colorr/255.0, colorg/255.0, colorb/255.0);

    // point cloud
    if (point_cloud) {
      for (int i = 0; i < (*(visionLoop.meshGenerator.getMeshes()))[r].points.size(); i++) {
        int x = (*(visionLoop.meshGenerator.getMeshes()))[r].points[i].x;
        int y = (*(visionLoop.meshGenerator.getMeshes()))[r].points[i].y;
        int z = (*(visionLoop.meshGenerator.getMeshes()))[r].points[i].z;
        reproject_utils::reprojectArr(x, y, z, h, w, projectMat);
  
        glVertex3f(projectMat[0]*scale, projectMat[1]*scale, projectMat[2]*scale+z_trans);
      }
    } else {
      glColor3f(colorr/255.0, colorg/255.0, colorb/255.0);

      for (int i = 0; i < (*(visionLoop.meshGenerator.getMeshes()))[r].faces.size(); i++) {
        int p1 = (*(visionLoop.meshGenerator.getMeshes()))[r].faces[i].p1; 
        int p2 = (*(visionLoop.meshGenerator.getMeshes()))[r].faces[i].p2; 
        int p3 = (*(visionLoop.meshGenerator.getMeshes()))[r].faces[i].p3; 

        glBegin(GL_TRIANGLES);

        int x = (*(visionLoop.meshGenerator.getMeshes()))[r].points[p1].x;
        int y = (*(visionLoop.meshGenerator.getMeshes()))[r].points[p1].y;
        int z = (*(visionLoop.meshGenerator.getMeshes()))[r].points[p1].z;
        reproject_utils::reprojectArr(x, y, z, h, w, projectMat);
  
        glVertex3f(projectMat[0]*scale, projectMat[1]*scale, projectMat[2]*scale+z_trans);

        x = (*(visionLoop.meshGenerator.getMeshes()))[r].points[p2].x;
        y = (*(visionLoop.meshGenerator.getMeshes()))[r].points[p2].y;
        z = (*(visionLoop.meshGenerator.getMeshes()))[r].points[p2].z;
        reproject_utils::reprojectArr(x, y, z, h, w, projectMat);
  
        glVertex3f(projectMat[0]*scale, projectMat[1]*scale, projectMat[2]*scale+z_trans);

        x = (*(visionLoop.meshGenerator.getMeshes()))[r].points[p3].x;
        y = (*(visionLoop.meshGenerator.getMeshes()))[r].points[p3].y;
        z = (*(visionLoop.meshGenerator.getMeshes()))[r].points[p3].z;
        reproject_utils::reprojectArr(x, y, z, h, w, projectMat);
  
        glVertex3f(projectMat[0]*scale, projectMat[1]*scale, projectMat[2]*scale+z_trans);

        glEnd();
      }  
    }
  }
  if (point_cloud){
    glEnd();
  }

  visionLoop.mesh_lock.unlock();

  glPopMatrix();
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  draw_mesh();
  glutSwapBuffers();
}

void idle(){
  // printf("idleing\n");
  glutPostRedisplay();
  usleep(50*1000);
}

void init(void) {
  /* Use depth buffering for hidden surface elimination. */
  glEnable(GL_DEPTH_TEST);

  /* Setup the view of the cube. */
  glMatrixMode(GL_PROJECTION);
  gluPerspective( /* field of view in degree */ 40.0,
    /* aspect ratio */ 1.0,
		/* Z near */ 1.0, /* Z far */ 10.0);
	glLoadIdentity();
	glViewport(0, 0, windowWidth, windowHeight);
	gluPerspective(45,(1.0*windowWidth)/windowHeight,1,20.0);
  glMatrixMode(GL_MODELVIEW);
  gluLookAt(0.0, 0.0, 5.0,  /* eye is at (0,0,5) */
    0.0, 0.0, 0.0,      /* center is at (0,0,0) */
    0.0, 1.0, 0.);      /* up is in positive Y direction */
}

void *vision_loop(void *arg) {
	visionLoop.vision_loop();
}
 
// TODO test resize of camera feed, stereo mapping, and point projection
int main(int argc, char **argv) {
  pthread_t tid;
  pthread_create(&tid, NULL, vision_loop, NULL);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(windowWidth, windowHeight);
  glutCreateWindow("red 3D lighted cube");
	glutDisplayFunc(display);
	// reenable later
  glutIdleFunc(idle);
  glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);
  init();
  glutMainLoop();
  pthread_join(tid, NULL);
  
  return 0;
}
