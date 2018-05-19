#include <bits/stdc++.h>
#include "GL/freeglut.h"
#include "GL/gl.h"
#include <GL/glu.h>
#define bsize 25
#define yborder 13.0
#define xborder 9.5

using namespace std;

/* Class to store picked points
 * mx, my correspond to the background texture extents
 * ax and ay correspond the position coordinates
 */
class PickPoint {
	public:
		float mx, my, ax, ay;
		PickPoint() {
		}

		PickPoint(float a, float b, float c, float d) {
			mx = a;
			my = b;
			ax = c;
			ay = d;
		}
};

/* Store the set of points corresponding to one billboard
 * and depth at which to be rendered
 */
class TextureStrip {
	public:
		vector<PickPoint> points;
		float depth;
		TextureStrip(vector<PickPoint> p) {
			points = p;
			depth = 2;
		}
		TextureStrip(vector<PickPoint> p, float d) {
			points = p;
			depth = d;
		}
};

class Vertex {
	public:
		float x, y, z, r, s;
		Vertex() {
			x = y = z = r = s = 0;
		}
		Vertex(float a, float b, float c) {
			x = a;
			y = b;
			z = c;
			r = s = 0;
		}
};

class Face {	
	public:
		vector<Vertex*> vertices;
		float normal[3];
		Face() {
			normal[0] = normal[1] = normal[2] = 0;
		};
};

class Model {
	public:
		char text[20];
		vector<Face*> faces;	
		GLuint tex;
		bool nflag;
		int time, angle, speed, pathIndex;
		float left, right, top, bottom, near, far, size, scale, dist;
		float mx, my, mz;
		int width, height;
		unsigned char *image;
		void computeTexSphere();
		void readTexture(char*);
		Model();
		Model(char*);
		virtual void setDirection(GLfloat *pos);
		virtual void nextPosition(float *transm, float *rotm, float* zoom, vector<PickPoint>&);
		void read(char*);
		void print();
		void setNormals();
		void setTexture();
		virtual void render();	
		void drawBox();
};

class Light: public Model{
	public:
		Light();
		void render();
		void setDirection(GLfloat *pos);
		void nextPosition(float *transm, float *rotsm, float* scle, vector<PickPoint>&);
};

class SceneGraphNode {
	public:
		Model* object;
		vector <SceneGraphNode*> edges;
		float transMatrix[3];
		float rotMatrix[3];
		float scale;
		SceneGraphNode(Model*);
		SceneGraphNode(Model*, float*);
		SceneGraphNode* getNode(int i);
		void appendModelNode(SceneGraphNode* sm);
		void drawNodeList(vector<PickPoint>&);
		void addTexture(char *tex);
		void getPosition(GLfloat *pos);
		void drawModel();
		void setTransitions();
};

class SceneGraph {
	public:
		vector<int> lights;
		vector<SceneGraphNode*> graph;
		float eye[4];
		vector<PickPoint> cp;
		vector<PickPoint> path;
		/* Vector to store pick points for every frame */
		vector<TextureStrip> strips[bsize];
		float transmat[6];
		int oldIndex, newIndex, tind, count, texCount;
		bool changed, bounding, mode, object, changeCam;
		float X, Y, Z;
		/* backgroundCount stores index of the frame in use 
		 * maxc stores max number of frames */
		int time, backgroundCount, maxc;
		SceneGraph() {
			X = Y = Z = 0.0;
			eye[0] = eye[1] = eye[2] = 0;
			oldIndex = 0; newIndex = 0; tind = -1;
			changed = mode = changeCam = bounding = false;
			object = true;
			count = texCount = 0;
			backgroundCount = 0, maxc = 0;
			
		}
		void addModel(SceneGraphNode* sg);
		void drawBackground(GLuint*);
		void drawBillboard(GLuint*);
		void init(int, char**);
		void render();
		void sketchPath(float, float);
		void setCamera(float*);
		void drawScene();
		void updateScene();
};	