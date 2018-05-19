#include <bits/stdc++.h>
#include "GL/freeglut.h"
#include "GL/gl.h"
#include <GL/glu.h>
#include "model.h"
#include "FastTrackball.h"

using namespace std;

class Controller {
	public:
		int currentX, currentY, mState, change, count, texCount; 
		float Position[3];
		/* Vector to store which frame indices that have explicitly picked points */
		vector<int> pointIndex;
		float data[3];
		bool headl, s1, s2, pMode;
		Trackball trackball;
		SceneGraph *model;
		void billboardDisplay(GLuint*);
		void bindTextures();
		void bounding();
		void changeBillboard(int);
		void changeCamTop();
		void display(GLuint*);
		void hideObject();
		/* Function to interpolate billboard positions for those that haven't been explicitly selected */
		void interpolate();
		void modePick();
		void mode();
		void moveX(float);
		void moveY(float);
		void render(int);
		void rotate(float, float, float, float);
		void run(int, char**);	
		void setDepth(float);
		void textureCoordinates(float, float, float, float);
		void zoomIn();
		void zoomOut();
		Controller() {
			currentX = currentY = mState=-1;
			headl = s1 = s2 = false;
			pMode = false;
			change = 0;
			count = texCount = 0;
		}
		
};
