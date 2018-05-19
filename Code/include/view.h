#include <bits/stdc++.h>
#include "GL/freeglut.h"
#include "GL/gl.h"
#include <GL/glu.h>
#include "controller.h"
#include <unistd.h>

using namespace std;

class View {
	public:
		static SceneGraph * model;
		/* billboardSelect stores whether user is in picking mode for frames
		 * or rendering mode 
		 */
		static int pick, path, menuid, val, billboardSelect;
		static GLuint tex[50];
		static Controller * controller;
		static void init(int, char**);
		/* Reading all frames as textures */
		static void initTex(int, char**);
		static void myinit(int, char**);
		static void menu(int);
		static void createMenu();
		static void drawAxis();
		static void display();
		static void keyboard(unsigned char, int, int);
		static void mouse(int, int, int, int);
		static void mouseMotion (int, int);
		static void reshape(GLsizei, GLsizei);
		static void specialKeys(int, int, int);
};
	
