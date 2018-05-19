#include <bits/stdc++.h>
#include "GL/freeglut.h"
#include "GL/gl.h"
#include <unistd.h>
#include <GL/glu.h>
#include "../include/model.h"
#include "../include/SOIL.h"

SceneGraphNode *temp;

using namespace std;

float start[3], peak[3], stop[3], interval, ys, vs, cowHeight, cowDepth, restart = false;
bool box = false, greater = false;
Vertex direction;

float findModulus(float x, float y, float z) {
	return sqrt(pow(x, 2) + pow(y, 2) + pow(z,2));
}

Model::Model() {
	size = 0;
	scale = 1;
	angle = 0;
	speed = 1;
	nflag = false;
	pathIndex = 0;
	tex = 0;
}

Model::Model(char *fname) {
	size = 0;
	scale = 1;
	angle = 0;
	speed = 1;
	nflag = false;
	tex = 0;
	pathIndex = 0;
	read(fname);
}

void Model::setNormals() {
	float a[3], b[3], c[3], length;
	for(int i=0; i<faces.size(); i++) {
		a[0] = faces[i]->vertices[1]->x - faces[i]->vertices[0]->x;
		a[1] = faces[i]->vertices[1]->y - faces[i]->vertices[0]->y;
		a[2] = faces[i]->vertices[1]->z - faces[i]->vertices[0]->z;
		
		b[0] = faces[i]->vertices[2]->x - faces[i]->vertices[0]->x;
		b[1] = faces[i]->vertices[2]->y - faces[i]->vertices[0]->y;
		b[2] = faces[i]->vertices[2]->z - faces[i]->vertices[0]->z;
		
		// Cross Product
		c[0] = a[1] * b[2] - b[1] * a[2]; 
		c[1] = a[2] * b[0] - b[2] * a[0];
		c[2] = a[0] * b[1] - b[0] * a[1];
		
		// Normalization Factor
		length = sqrt((c[0] * c[0]) + (c[1] * c[1]) + (c[2] * c[2]));
		
		faces[i]->normal[0] = c[0] / length;
		faces[i]->normal[1] = c[1] / length;
		faces[i]->normal[2] = c[2] / length;
		
	}
}

void Model::computeTexSphere() {
	for(int i=0; i<faces.size(); i++) {
		for(int j=0; j<faces[i]->vertices.size(); j++) {
			faces[i]->vertices[j]->r = (atan2f(faces[i]->vertices[j]->z, faces[i]->vertices[j]->x)  + 3.14)/(2*3.14);
			faces[i]->vertices[j]->s = (atan2f(faces[i]->vertices[j]->z, faces[i]->vertices[j]->y*sin(2*3.14*faces[i]->vertices[j]->r)))/(2*3.14);
		}
	}
}

void Model::setDirection(GLfloat *pos) {	
}

void Model::read(char *argv) {
	vector<Vertex*> v;
	float minx, maxx, miny, maxy, minz, maxz;
	minx = miny = minz = 10000;
	maxx = maxy = maxz = -10000;
	FILE *file = fopen(argv,"r");
	int noOfv, noOfFaces;
	char buffer[100];
	if(file) {
		fgets(buffer, 100, file);
		while(strncmp("element vertex", buffer, strlen("element vertex")) != 0) {
			fgets(buffer, 100, file);
		}
		strcpy(buffer, buffer+strlen("element vertex"));
		sscanf(buffer, "%d", &noOfv);
					
		while(strncmp("element face", buffer, strlen("element face")) != 0) {
			fgets(buffer, 100, file);
		}
		strcpy(buffer, buffer+strlen("element face"));
		sscanf(buffer, "%d", &noOfFaces);
		
		while(strncmp("end_header", buffer, strlen("end_header")) != 0  ) {
			fgets(buffer, 100, file);			
		}
		
		for(int i=0; i<noOfv; i++) {
			fgets(buffer, 100, file);
			float a, b, c;
			sscanf(buffer, "%f %f %f", &a, &b, &c);
			minx = min(minx, a);
			miny = min(miny, b);
			minz = min(minz, c);
			maxx = max(maxx, a);
			maxy = max(maxy, b);
			maxz = max(maxz, c);
			Vertex* newV;
			newV = new Vertex(a,b,c);
			v.push_back(newV);
		}
		if(size < maxx-minx)
		    	size = maxx-minx;
		if(size < maxy-miny)
			size = maxy-miny;
		if(size < maxz-minz)
		    	size = maxz-minz;
		scale = 2.0 / size;
		for (int i=0; i<noOfv; i++) {
		    	v[i]->x = scale * (v[i]->x - minx) - 1.0;
		    	v[i]->y = scale * (v[i]->y - miny) - 1.0;
			v[i]->z = scale * (v[i]->z - minz) - 1.0;
		}
		minx = miny = minz = 10000;
		maxx = maxy = maxz = -10000;
		for (int i=0; i<noOfv; i++) {
		    	minx = min(minx, v[i]->x);
		    	maxx = max(maxx, v[i]->x);
		    	miny = min(miny, v[i]->y);
		    	maxy = max(maxy, v[i]->y);
		    	minz = min(minz, v[i]->z);
			maxz = max(maxz, v[i]->z);
		}
		left = minx; right = maxx;
		top = maxy; bottom = miny;
		near = minz; far = maxz;
		for(int i=0; i<noOfFaces; i++) {
			int listSize;
			fscanf(file, "%d", &listSize);
			Face* newf = new Face();
			for(int j=0; j<listSize; j++) {
				int a;
				fscanf(file, "%d", &a);
				newf->vertices.push_back(v[a]);
			}
			faces.push_back(newf);
		}
		fclose(file);
		mx = -(maxx+minx)/2;
		my = -(maxy+miny)/2;
		mx = -(maxz+minz)/2;
		
	}
	else {
		printf("File cannot be opened\n");
	}
}

void Model::readTexture(char* texname) {
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image = SOIL_load_image(texname, &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
   	SOIL_free_image_data(image);
}

void Model::drawBox() {
	glPushMatrix();
		glColor3f(.75, .75, 0.0);
		glBegin( GL_LINES);
		float minx, maxx, miny, maxy, minz, maxz;
		minx = left;
		maxx = right;
		miny = bottom;
		minz = near;
		maxy = top;
		maxz = far;
	
		glVertex3f(minx, miny, minz);			
		glVertex3f(maxx, miny, minz);				
		glVertex3f(minx, maxy, minz);			
		glVertex3f(maxx, maxy, minz);			
		glVertex3f(minx, miny, minz);			
		glVertex3f(minx, maxy, minz);			
		glVertex3f(maxx, miny, minz);			
		glVertex3f(maxx, maxy, minz);			
		
		glVertex3f(minx, miny, maxz);			
		glVertex3f(maxx, miny, maxz);				
		glVertex3f(minx, maxy, maxz);			
		glVertex3f(maxx, maxy, maxz);			
		glVertex3f(minx, miny, maxz);			
		glVertex3f(minx, maxy, maxz);			
		glVertex3f(maxx, miny, maxz);			
		glVertex3f(maxx, maxy, maxz);			
		
		glVertex3f(minx, miny, minz);			
		glVertex3f(minx, miny, maxz);			
		glVertex3f(minx, maxy, minz);			
		glVertex3f(minx, maxy, maxz);			

		glVertex3f(maxx, miny, minz);			
		glVertex3f(maxx, miny, maxz);				
		glVertex3f(maxx, maxy, minz);			
		glVertex3f(maxx, maxy, maxz); 
		
		glEnd();
		
	glPopMatrix();
	glutPostRedisplay();
}

void Model::setTexture() {
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, tex);
}

void Model::render() {
	for(int n=0; n<faces.size(); n++) {	
		glBegin(GL_POLYGON);		
		glColor3f(.75, .75, 1);
		for(int i=0; i<faces[n]->vertices.size(); i++) {	
			glNormal3f(faces[n]->normal[0], faces[n]->normal[1], faces[n]->normal[2]);
			glTexCoord2f(faces[n]->vertices[i]->r, faces[n]->vertices[i]->s);		
			glVertex3f(faces[n]->vertices[i]->x, faces[n]->vertices[i]->y, faces[n]->vertices[i]->z); 
		}
		glEnd();
	}
}

void Model::print() {
	for(int n=0; n<faces.size(); n++) {	
		for(int i=0; i<faces[n]->vertices.size(); i++) {	
			cout<<faces[n]->vertices[i]->x<<" "<<faces[n]->vertices[i]->y<<" "<<faces[n]->vertices[i]->z<<endl;
		}
	}
}

void Model::nextPosition(float *transm, float *rotsm, float* scle, vector<PickPoint>& path) {
	if(restart == true) {
		time = 0;
		transm[0] = transm[1] = 2;
		restart = false;
	}
	if(time < 25) {
		transm[0] +=.1;
		transm[1] += .01;
		transm[2] = cowDepth;
	}
	/* Path skething
	if(pathIndex < path.size()) {
		transm[0] = path[pathIndex].ax;
		transm[2] = path[pathIndex].ay;
		pathIndex++;
	}
	if(time == 0) {
		transm[0] = path[pathIndex].ax;
		transm[2] = path[pathIndex].ay;
		if(pathIndex + 1 < path.size()) {
			direction.x = (path[pathIndex + 1].ax - path[pathIndex].ax);
			direction.z = (path[pathIndex + 1].ay - path[pathIndex].ay);
			float m = findModulus(direction.x, direction.z, 0);
			direction.x /= m;
			direction.z /= m;
			cout<<m<<" D "<<direction.x<<" "<<direction.z<<endl;
			if(path[0] > path[1]) greater = false;
			else greater = true;
			pathIndex += 1;
		}
	}
	else {
		if(greater) {
			if((transm[0] + (direction.x * .1)) >= path[pathIndex].ax 
				&& (transm[2] + (direction.z * .1) < path[pathIndex].ay)) {
					transm[0] += (direction.x * .1);
					transm[2] += (direction.z *.1);
			}
			else {
				transm[0] = path[pathIndex].ax;
				transm[2] = path[pathIndex].ay;
				if(pathIndex + 1 < path.size()) {
					direction.x = (path[pathIndex + 1].ax - path[pathIndex].ax);
					direction.z = (path[pathIndex + 1].ay - path[pathIndex].ay);
					float m = findModulus(direction.x, direction.z, 0);
					direction.x /= m;
					direction.z /= m;
					cout<<"D "<<direction.x<<" "<<direction.z<<endl;
					pathIndex += 1;
					if(path[pathIndex-1] > pathIndex[pathIndex]) greater = false;
					else greater = true;
				}
			}
		}
		else {

		}
	}*/
	time++;
}

Light::Light() : Model() {
	GLfloat ambientColor[] = {.5f, 0.5f, 0.5f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
	GLfloat lightColor0[] = {0.5f, 0.5f, 0.5f, 1.0f};
	GLfloat lightPos0[] = {-2.5f, .6f, -2, 1.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0); 
}

void Light::render() {
}

void Light::setDirection(GLfloat *pos) {
}

void Light::nextPosition(float *transm, float *rotsm, float* scle, vector<PickPoint>& path) { 
}

SceneGraphNode::SceneGraphNode(Model* o) {
	object = o;
	transMatrix[0] = 0;
	transMatrix[1] = 0;
	transMatrix[2] = 0;
	rotMatrix[0] = 0;
	rotMatrix[1] = 0;
	rotMatrix[2] = 0;
	scale = 1;
}  

SceneGraphNode::SceneGraphNode(Model* o, float* mat) {
	object = o;
	transMatrix[0] = mat[0];
	transMatrix[1] = mat[1];
	transMatrix[2] = mat[2];
	rotMatrix[0] = mat[3];
	rotMatrix[1] = mat[4];
	rotMatrix[2] = mat[5];
	scale = mat[6];
}

void SceneGraphNode::addTexture(char *tex) {
	object->readTexture(tex);
}

void SceneGraphNode::appendModelNode(SceneGraphNode* sm) {
	edges.push_back(sm);
}

void SceneGraphNode::drawModel() {
	if (box)
		object->drawBox();
	glEnable(GL_TEXTURE_2D);
	object->setTexture();
	object->render();
	glDisable(GL_TEXTURE_2D);
}

void SceneGraphNode::setTransitions(){
	glTranslatef(transMatrix[0], transMatrix[1], transMatrix[2]);
	glRotatef(rotMatrix[0], 1, 0, 0);
	glRotatef(rotMatrix[1], 0, 1, 0);
	glRotatef(rotMatrix[2], 0, 0, 1);
	glScalef(scale, scale, scale);
}

void SceneGraphNode::drawNodeList(vector<PickPoint>& path) {
	glPushMatrix();
	setTransitions();
	drawModel();
	int i=0;
	for (i=0; i<edges.size(); i++) 
		edges[i]->drawNodeList(path);
	glPopMatrix();
	object->nextPosition(transMatrix, rotMatrix, &scale, path);
}

SceneGraphNode* SceneGraphNode::getNode(int i) {
	return edges[i];
}

void SceneGraphNode::getPosition(GLfloat *pos) {
	pos[0] = transMatrix[0];
	pos[1] = transMatrix[1];
	pos[2] = transMatrix[2];
}

void SceneGraph::drawBillboard(GLuint* tex) {

}

void SceneGraph::drawBackground(GLuint* tex) {
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	int ind = backgroundCount;
	glBindTexture(GL_TEXTURE_2D, tex[ind]);
	glColor3f(1,1,1);
	// Main Background
	glBegin(GL_QUADS);
		glNormal3f(0.0, 0.0f, 1.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(xborder, yborder, 7.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-xborder, yborder, 7.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(-xborder, -yborder, 7.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(xborder, -yborder, 7.0f);    
	glEnd();

	// New billboard based on user select
	for(int i = 0; i < strips[ind].size(); i++) {
		glBegin(GL_POLYGON);
		for(int j = 0; j < strips[ind][i].points.size(); j++) {
			glNormal3f(0.0, 0.0f, 1.0f);
			glTexCoord2f(strips[ind][i].points[j].mx, strips[ind][i].points[j].my);
			/* 
			 * using similarity of triangles
			 * see views.png in helper folder
			 */
			glVertex3f(strips[ind][i].points[j].ax * (abs(-6 - strips[ind][i].depth)  - Z) / (7 - eye[2]), 
				   	   strips[ind][i].points[j].ay * (abs(-6 - strips[ind][i].depth)  - Z) / (7 - eye[2]), 
				       strips[ind][i].depth);
		}
		glEnd();
	}
    glDisable(GL_TEXTURE_2D);
}

void SceneGraph::sketchPath(float x, float y) {
	x = (x - .5) * 2 * xborder; 
	y = (y - .5) * 2 * -6.;
	PickPoint po(0, 0, -x, y);
	path.push_back(po);
}

void SceneGraph::drawScene() {
	cowDepth = strips[backgroundCount][0].depth + 2;
	if(backgroundCount == 0) 
		restart = true;
	glEnable(GL_LIGHT0);
	/* Condition for sketching path has been commented out to demonstrate 
	 * the hard coded path
	 * if(changeCam == false && path.size() != 0) { */
		for(int i=0; i<graph.size(); i++) 
			graph[i]->drawNodeList(path);
	glDisable(GL_LIGHT0);
	backgroundCount = min(backgroundCount + 1, maxc - 2);
}

float distance(float x1, float x2, float y1, float y2, float z1, float z2) {
	return sqrt(pow(x2-x1, 2) + pow(y2-y1, 2) + pow(z2-z1, 2));
}

void SceneGraph::init(int argc, char** argv) {
	maxc = argc;
	char text[30];
	strcpy(text, "data/large.jpg");
	float geomProps[7] = {0, 2, 4, -9, 0, 0, 1.5};
	Model *cow = new Model("data/cow.ply"); 
	cow->computeTexSphere();
	cow->setNormals();
	cow->time = 0;
	SceneGraphNode *a = new SceneGraphNode(cow, &geomProps[0]);
	a->addTexture(text);
	addModel(a);
	Light *light = new Light();
	SceneGraphNode *b = new SceneGraphNode(light);
	addModel(b);
}

void SceneGraph::addModel(SceneGraphNode* sg) {
	graph.push_back(sg);
}

void multMatrix(float *e, float *rot) {
	int i, j;
	float c[4];
	for (i=0; i<4; i++) {
		c[i] = 0.0;
		for (j=0; j<4; j++) {
			c[i] += e[j]*rot[4*j+i];
		}
	}
	for (i=0; i<3; i++)  {
		e[i] = c[i];
	}
}

void SceneGraph::setCamera(float *rotmat) {
	eye[0] = 0 + X; 
	eye[1] = 0 + Y;
	eye[2] = -6.0 + Z;
	eye[3] = 1;
	if(changeCam == true) {
		eye[0] = 0 + X; 
		eye[1] = 8 + Y;
		eye[2] = 6.0 + Z;
		eye[3] = 1;
	}
	multMatrix(&eye[0], rotmat);
	gluLookAt(eye[0], eye[1], eye[2], X, Y, Z, 0, 1, 0);
}