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

std::string type2str(int type) {
  std::string r;

  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);

  switch ( depth ) {
    case CV_8U:  r = "8U"; break;
    case CV_8S:  r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default:     r = "User"; break;
  }

  r += "C";
  r += (chans+'0');

  return r;
}

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

void draw_disp_map() {
  // glColor3f(1.f, 0.f, 0.f);
  glBegin(GL_QUADS);
  float widthdisp = 2.f/640;
  float heightdisp = 1.5f/480;
  float minwidth = widthdisp*4;
  float minheight = heightdisp*4;
  float dispXStart = -3.3f;
  float dispYStart = 2.f;
  if (visionLoop.cameraPtr != NULL && visionLoop.cameraPtr->getDisparityNorm()->empty() == 0) {
    for(int r = 0; r < 480; r+=4) {
      for(int c = 0; c < 640; c+=4) {
        float grayscale = visionLoop.cameraPtr->getDispAtNorm(r,c)/255.0;
        glColor3f(grayscale, grayscale, grayscale);
        float addx = widthdisp*c;
        float addy = heightdisp*r;
        glVertex3f(dispXStart+addx, dispYStart-addy, 0.f);
        glVertex3f(dispXStart+addx, dispYStart-addy-minheight, 0.f);
        glVertex3f(dispXStart+addx+minwidth, dispYStart-addy-minheight, 0.f);
        glVertex3f(dispXStart+addx+minwidth, dispYStart-addy, 0.f);
      }
    }
  }

  glEnd();
}

void draw_overhead() {
  cv::Mat *map = visionLoop.map2d.getVisual();
  int rows = map->rows;
  int cols = map->cols;
  float widthdisp = 2.f/cols;
  float heightdisp = 2.f/rows;
  float minwidth = widthdisp*4;
  float minheight = heightdisp*4;
  float dispXStart = -3.3f;
  float dispYStart = 0.5f;

  glBegin(GL_QUADS);

  if (map->empty() == 0) {
    for(int r = 0; r < rows/2+16; r+=4) {
      for(int c = 0; c < cols; c+=4) {
        float grayscale = map->at<float>(r,c);
        glColor3f(grayscale, grayscale, grayscale);
        float addx = widthdisp*c;
        float addy = heightdisp*r;
        glVertex3f(dispXStart+addx, dispYStart-addy, 0.f);
        glVertex3f(dispXStart+addx, dispYStart-addy-minheight, 0.f);
        glVertex3f(dispXStart+addx+minwidth, dispYStart-addy-minheight, 0.f);
        glVertex3f(dispXStart+addx+minwidth, dispYStart-addy, 0.f);
      }
    }
  }

  glEnd();
}

void draw_optical_flow() {
  if (visionLoop.cameraPtr != NULL && visionLoop.cameraPtr->getLeftCamera()->getFrame()->empty() == 0) {
    cv::Mat vis_large = visionLoop.cameraPtr->getLeftCamera()->getFrame()->clone();
    cv::Mat vis;

    int rows_large = vis_large.rows;
    int cols_large = vis_large.cols;

    // the inverse scalar for the output image
    int scalar = 4;

    cv::resize(vis_large, vis, cv::Size(rows_large / scalar, cols_large / scalar));
    std::vector<cv::Point2f>* initFeatures = visionLoop.featureTracker.getInitFeatures();
    std::vector<cv::Point2f>* curFeatures = visionLoop.featureTracker.getCurFeatures();

    // draw tracking points on the left camera image
    for(int i = 0; i < initFeatures->size(); i++) {
      cv::Point2f curPoint = curFeatures->at(i);
      curPoint.x = curPoint.x / scalar; 
      curPoint.y = curPoint.y / scalar; 

      cv::Point2f startPoint = initFeatures->at(i);
      startPoint.x = startPoint.x / scalar; 
      startPoint.y = startPoint.y / scalar; 

      cv::line(vis, startPoint, curPoint, cv::Scalar(0, 128, 0));
      // cv::line(visDisp, startPoint, curPoint, cv::Scalar(0, 128, 0));
      cv::circle(vis, curPoint, 1, cv::Scalar(0,255,0), -1);
      // cv::circle(visDisp, curPoint, 2, cv::Scalar(0,255,0), -1);
    }

    int rows = vis.rows;
    int cols = vis.cols;

    float widthdisp = 2.f/cols;
    float heightdisp = 1.5f/rows;
    float minwidth = widthdisp;
    float minheight = heightdisp;
    float dispXStart = -3.3f;
    float dispYStart = -0.5f;

    glBegin(GL_QUADS);
    
    for(int r = 0; r < rows; r++) {
      for(int c = 0; c < cols; c++) {
        cv::Vec3b colors = vis.at<cv::Vec3b>(r, c);
        glColor3f(colors[0]/255.0, colors[1]/255.0, colors[2]/255.0);
        float addx = widthdisp*c;
        float addy = heightdisp*r;
        glVertex3f(dispXStart+addx, dispYStart-addy, 0.f);
        glVertex3f(dispXStart+addx, dispYStart-addy-minheight, 0.f);
        glVertex3f(dispXStart+addx+minwidth, dispYStart-addy-minheight, 0.f);
        glVertex3f(dispXStart+addx+minwidth, dispYStart-addy, 0.f);
      }
    }

    glEnd();
  }
}

void draw_mesh(void) {
  int i;

  draw_disp_map();

  draw_overhead();

  draw_optical_flow();

  glPushMatrix();
  glTranslatef(1.0, 0.0, -3.0);
  glRotatef(zAngle+zAngleDiff, 1.0, 0.0, 0.0);
  glRotatef(yAngle+yAngleDiff, 0.0, 1.0, 0.0);

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

    // point cloud
    if (point_cloud) {
      for (int i = 0; i < (*(visionLoop.meshGenerator.getMeshes()))[r].points.size(); i++) {
        glColor3f(colorr/255.0, colorg/255.0, colorb/255.0);

        int x = (*(visionLoop.meshGenerator.getMeshes()))[r].points[i].x;
        int y = (*(visionLoop.meshGenerator.getMeshes()))[r].points[i].y;
        int z = (*(visionLoop.meshGenerator.getMeshes()))[r].points[i].z;
        reproject_utils::reprojectArr(x, y, z, h, w, projectMat);
  
        glVertex3f(projectMat[0]*scale, projectMat[1]*scale, projectMat[2]*scale+z_trans);
      }
    } else {
      for (int i = 0; i < (*(visionLoop.meshGenerator.getMeshes()))[r].faces.size(); i++) {
        int p1 = (*(visionLoop.meshGenerator.getMeshes()))[r].faces[i].p1; 
        int p2 = (*(visionLoop.meshGenerator.getMeshes()))[r].faces[i].p2; 
        int p3 = (*(visionLoop.meshGenerator.getMeshes()))[r].faces[i].p3; 

        int x = (*(visionLoop.meshGenerator.getMeshes()))[r].points[p1].x;
        int y = (*(visionLoop.meshGenerator.getMeshes()))[r].points[p1].y;
        int z = (*(visionLoop.meshGenerator.getMeshes()))[r].points[p1].z;
        reproject_utils::reprojectArr(x, y, z, h, w, projectMat);

        glBegin(GL_TRIANGLES);
  
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

        if (visionLoop.cameraPtr != NULL && visionLoop.cameraPtr->getLeftCamera()->getFrame()->empty() == 0) {
          cv::Vec3b colors = visionLoop.cameraPtr->getLeftCamera()->getFrame()->at<cv::Vec3b>(y, x);

          if ((projectMat[2] < -0.87 || projectMat[1] > 0.2) && false) {
            float grayscale = (((int)colors[0]) + colors[1] + colors[2])/(7*255.0);
            glColor3f(grayscale, grayscale, grayscale);
          } else {
            glColor3f(colors[0]/255.0, colors[1]/255.0, colors[2]/255.0);
          }
        } else {
          glColor3f(colorr/255.0, colorg/255.0, colorb/255.0); 
        }
  
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
