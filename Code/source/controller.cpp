#include "../include/controller.h"
#include "../include/SOIL.h"

#define pi 3.142857

using namespace std;

void Controller::interpolate() {
	for(int i = 1; i < pointIndex.size(); i++) { 							 /* No of explicitly picked points */
		int start = pointIndex[i - 1], stop = pointIndex[i], interval = stop - start;				
		for(int j = 0; j < model->strips[start].size(); j++) {				 /* No of billboards */
			int count = 0;
			for(int l = start+1; l < stop; l++) {						 	 /* No of frames without points */
				count++;
				for(int k = 0; k < model->strips[start][j].points.size(); k++) { /* No of points */
					float mxdiff = (model->strips[stop][j].points[k].mx - model->strips[start][j].points[k].mx) / interval;
					float mydiff = (model->strips[stop][j].points[k].my - model->strips[start][j].points[k].my) / interval;
					float axdiff = (model->strips[stop][j].points[k].ax - model->strips[start][j].points[k].ax) / interval;
					float aydiff = (model->strips[stop][j].points[k].ay - model->strips[start][j].points[k].ay) / interval;				
					float x = model->strips[start][j].points[k].mx + (count * mxdiff);
					float y = model->strips[start][j].points[k].my + (count * mydiff);
					float a = model->strips[start][j].points[k].ax + (count * axdiff);
					float b = model->strips[start][j].points[k].ay + (count * aydiff);
					PickPoint po(x, y, a, b); 
					model->cp.push_back(po);
				}
				float depthDiff = (model->strips[stop][j].depth - model->strips[start][j].depth) / interval;
				TextureStrip t(model->cp, model->strips[start][j].depth + (count * depthDiff));	
				model->strips[l].push_back(t);
				model->cp.clear();	
			}
		}
	}
	for (int i = 0; i < 25; i++) {
		for (int j = 0; j < model->strips[i].size(); j++) {
			for (int k = 0; k < model->strips[i][j].points.size(); k++) {
			}
		}
		cout<<endl;
	}
}

void Controller::modePick() {
	if(pMode == false)
		pMode = true;
	else {
		pMode = false;
		if(pointIndex.size() == 0)
			pointIndex.push_back(model->backgroundCount);
		else if(pointIndex.back() != model->backgroundCount)
			pointIndex.push_back(model->backgroundCount);
		TextureStrip t(model->cp);	
		model->strips[model->backgroundCount].push_back(t);
		model->count = 0;
		model->cp.clear();	
		}
}

void Controller::setDepth(float d) {
	int ind = model->strips[model->backgroundCount].size();
	model->strips[model->backgroundCount][ind-1].depth = d;
	cout << model->strips[model->backgroundCount][ind-1].depth<<endl;
}


void Controller::changeBillboard(int i) {
	model->backgroundCount += i;
	model->backgroundCount = max(0, model->backgroundCount);
	model->backgroundCount = min(24, model->backgroundCount);
}

void Controller::textureCoordinates(float x, float y, float a, float b) {
	if(model->changeCam == true) {
		model->sketchPath(a, b);
	}
	else {
		if(pMode == true) {
			/*
			 * a, b are values between 0 and 1
			 * now mapping a to -9.5 and 9.5
			 * b to -13 and 13
			 */
			a = (a - .5) * 2 * xborder; 
			b = (b - .5) * 2 * yborder;
			cout<<a<<" "<<b<<endl;
			PickPoint po(x, y, a, b); 
			model->cp.push_back(po);
			model->count++;
		}
	}
}

void Controller::changeCamTop() {
	if(model->changeCam == true)
		model->changeCam = false;
	else
		model->changeCam = true;
}

void Controller::hideObject() {
	if(model-> object == true) model->object = false;
	else model->object = true;
}			

void Controller::moveX(float x) {
	model->X += x;
}
  		   	
void Controller::moveY(float y) {
	model->Y += y;
}

void Controller::zoomIn() {
        model->Z -= .05f;
}

void Controller::zoomOut() {
        model->Z += .05f;
}

void Controller::rotate(float currentX, float currentY, float x, float y) {
	int winX = glutGet(GLUT_WINDOW_WIDTH);
	int winY = glutGet(GLUT_WINDOW_HEIGHT);
	trackball.rotate((2.0*currentX - winX)/(float)winX, (winY - 2.0*currentY)/(float)winY, (2.0*x - winX)/(float)winX, (winY - 2.0*y)/(float)winY);
}

void Controller::billboardDisplay(GLuint* tex) {
	glLoadIdentity();
	glPushMatrix();							
		static float rotation_transform[4][4];					
		trackball.rotationMatrix(rotation_transform);	
		model->setCamera(&rotation_transform[0][0]);
		model->drawBackground(tex);
	glPopMatrix();
	glutSwapBuffers();
}

void Controller::display(GLuint* tex) {
	glLoadIdentity();
	glPushMatrix();							
		static float rotation_transform[4][4];					
		trackball.rotationMatrix(rotation_transform);	
		model->setCamera(&rotation_transform[0][0]);
		model->drawBackground(tex);
		model->drawScene();	
	glPopMatrix();
	glutSwapBuffers();
}

void Controller::run(int argc, char** argv) {
	model->init(argc, argv);
}
