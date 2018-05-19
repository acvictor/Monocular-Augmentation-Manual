#include <bits/stdc++.h>
#include "GL/freeglut.h"
#include "GL/gl.h"
#include <GL/glu.h>
#include "../include/view.h"
#include "../include/SOIL.h"

using namespace std;


GLuint View::tex[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int View::val = 0, View::menuid = 0, View::billboardSelect = 1;
float mx, my;
GLdouble pos3Dx = 0, pos3Dy = 0, pos3Dz = 0;
float d;
GLdouble projection_matrix[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1} ;
GLdouble modelview_matrix[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1} ;
GLint viewport[4] = {0,0,0,0}; 

void View::initTex(int i, char** argv) {
	unsigned char* image;
	int width, height;
	glGenTextures(1, &tex[i-1]);
	glBindTexture(GL_TEXTURE_2D, tex[i-1]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 
    	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	image = SOIL_load_image(argv[i], &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
}

void View::myinit(int argc, char** argv) {
	GLsizei winX = glutGet(GLUT_WINDOW_WIDTH);
	GLsizei winY = glutGet(GLUT_WINDOW_HEIGHT);	
	glViewport(0, 0, winX, winY);
	glClearColor(0.0,0.0,0.0,1.0);
	glColor3f(1.0,0.0,0.0);
	for(int i = 1; i <= argc; i++) {
		initTex(i, argv);
	}	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE); 
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glMatrixMode(GL_PROJECTION);						
	glLoadIdentity();	
	gluPerspective(90.0, (float)winX / (float)winY, -6, 6.0);		
	glMatrixMode(GL_MODELVIEW);						
	glLoadIdentity();	
}


void View::drawAxis() {
	glPushMatrix();
	glBegin(GL_LINES);
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(-250.0, 0.0, -1.0f);
		glVertex3f(250.0, 0.0, -1.0f);		
	glEnd();
	glBegin( GL_LINES);
		glColor3f( 0.0, 0.0, 1.0);
		glVertex3f(0.0, -250.0, -1.0f);
		glVertex3f(0.0, 250.0, -1.0f);		
	glEnd();
	glPopMatrix();
}

void View::display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	//drawAxis();
	if(billboardSelect){
		controller->billboardDisplay(tex);
	}
	else {
		controller->display(tex);
		usleep(105000);
	}
	glutPostRedisplay();
}

void View::reshape(GLsizei wt, GLsizei ht) {  
	if (ht == 0)
		ht = 1;								// Update Window's Height to New Height
	glViewport(0,0, wt, ht);						// Set the Viewing Window
	glMatrixMode(GL_PROJECTION); 						//Switch to setting the camera perspective
	glLoadIdentity(); 							//Reset the camera
	gluPerspective(90.0, (float)wt / (float)ht, .1, 20.0);			// Camera - ViewAngle, Viewport, nearZ, farZ
	glMatrixMode(GL_MODELVIEW);						// Entering Viewing Mode
	glLoadIdentity();							// Reset View Matrix
}

void View::keyboard(unsigned char key, int x, int y) {
	
   	switch (key) {
      		case 27:     
        	 	exit(0);
        	 	break;
        	 	
        	case '+':
        		controller->zoomIn();
       		   	glutPostRedisplay();
        		break;
        		
        	case '-':
        		controller->zoomOut();
       		   	glutPostRedisplay();
        		break;
        		
        	case 'o':
        		controller->hideObject();
        		glutPostRedisplay();
        		break;

        	case 'c':
        		controller->changeCamTop();
        		glutPostRedisplay();
        		break;

        	case 'm':
        		controller->modePick();
        		glutPostRedisplay();
        		break;

        	case 'i':
        		cin >> d;
        		controller->setDepth(d);
        		glutPostRedisplay();
        		break;
        		
        	case 's':
        		billboardSelect = 0;
        		model->backgroundCount = 0;
        		controller->interpolate();
        		glutPostRedisplay();
        		break;

        	default:
        		break;       
   	}
}

void View::mouseMotion(int x, int y) {
	if (View::controller->mState == 0) {
		View::controller->rotate(View::controller->currentX, View::controller->currentY, x, y);
		glutPostRedisplay();
    	}
  	View::controller->currentX = x;
    	View::controller->currentY = y;
}

void coords(int x, int y) {
	mx = (float)x / 500;
	my = (float)y / 700;

}

void View::mouse(int button, int state, int x, int y) {
	double nx, ny, nz;
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		View::controller->mState = 0;				
        View::controller->currentX = x;
        View::controller->currentY = y;
    }	
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
       	View::controller->mState = 1;				
		View::controller->rotate(0, 0, 0, 0);		      
    	glutPostRedisplay();
	}
	else if(button == GLUT_RIGHT_BUTTON and state == GLUT_DOWN) {
		coords(x, y); // texture extents
		/*
		 * 700 - y because mouse y increases downwards
		 * 500 - x because camera is on -z pointing towards +z
		 */
		controller->textureCoordinates(mx, my, (float)(500 - x) / 500, (float)(700 - y) / 700); 
		glutPostRedisplay();
	}
}


void View::specialKeys(int key, int x, int y) {
	if(billboardSelect) {
		if(key == GLUT_KEY_LEFT) 
		controller->changeBillboard(-1);
		
	else if(key == GLUT_KEY_UP)
		controller->changeBillboard(1);
		
	else if(key == GLUT_KEY_RIGHT)
		controller->changeBillboard(1);
	
	else if(key == GLUT_KEY_DOWN)
		controller->changeBillboard(-1);
	}
	else {
		if(key == GLUT_KEY_LEFT) 
			controller->moveX(-0.1);
			
		else if(key == GLUT_KEY_UP)
			controller->moveY(0.1);
			
		else if(key == GLUT_KEY_RIGHT)
			controller->moveX(0.1);
		
		else if(key == GLUT_KEY_DOWN)
			controller->moveY(-0.1);
	}
	
	glutPostRedisplay();
}
void View::init(int argc, char** argv) {
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500,700);
	glutCreateWindow("3D");
	View::myinit(argc, argv);
	controller->run(argc, argv);
	glutDisplayFunc(View::display); 
	glutReshapeFunc(View::reshape);       
	glutKeyboardFunc(View::keyboard);
	glutSpecialFunc(View::specialKeys);
	glutMotionFunc(View::mouseMotion); 
	glutMouseFunc(View::mouse);
	glutMainLoop();
}
