//Jeff Chastine
#include <Windows.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <math.h>
#define PI 3.1415926535

using namespace std;

float px, py, pxDelta, pyDelta, pAngle; //player position

void drawPlayer() {
	glColor3f(1, 1, 0);
	glPointSize(8);
	glBegin(GL_POINTS);
	glVertex2i(px, py);
	glEnd();

	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2i(px, py);
	glVertex2i(px + pxDelta * 5, py + pyDelta * 5);
	glEnd();
}

int mapX = 8, mapY = 8, mapS = 64;
int map[] = {
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 1, 0, 0, 0, 0, 1,
	1, 0, 1, 0, 0, 0, 0, 1,
	1, 0, 1, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 1, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1
};

void drawMap2D() {
	int x, y, x0, y0;
	for (y = 0; y < mapY; y++) {
		for (x = 0; x < mapX; x++) {
			if (map[y * mapX + x] == 1) {
				glColor3f(1, 1, 1);
			}
			else {
				glColor3f(0, 0, 0);
			}
			x0 = x * mapS;
			y0 = y * mapS;
			glBegin(GL_QUADS);
			glVertex2i(x0 + 1, y0 + 1);
			glVertex2i(x0 + 1, y0 + mapS - 1);
			glVertex2i(x0 + mapS - 1, y0 + mapS - 1);
			glVertex2i(x0 + mapS - 1, y0 + 1);
			glEnd();
		}
	}
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawMap2D();
	drawPlayer();
	glutSwapBuffers();
}

void buttons(unsigned char key, int x, int y) {

	if (key == 'a') {
		pAngle -= 0.1;
		
		if (pAngle < 0) {
			pAngle += 2 * PI;
		}

		pxDelta = cos(pAngle) * 5;
		pyDelta = sin(pAngle) * 5;
	}

	if (key == 'd') {
		pAngle += 0.1;

		if (pAngle < 0) {
			pAngle -= 2 * PI;
		}

		pxDelta = cos(pAngle) * 5;
		pyDelta = sin(pAngle) * 5;
	}

	if (key == 'w') {
		px += pxDelta;
		py += pyDelta;
	}

	if (key == 's') {
		px -= pxDelta;
		py -= pyDelta;
	}

	glutPostRedisplay();
}

void init() {
	glClearColor(0.3, 0.3, 0.3, 0);
	gluOrtho2D(0, 1024, 512, 0);
	px = 300; 
	py = 300; 
	pxDelta = cos(pAngle) * 5; 
	pyDelta = sin(pAngle) * 5;
}

int main(int argc, char* argv[]) {
	
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(1024, 512);
	glutCreateWindow("Youtube - 3DSage");
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(buttons);
	glutMainLoop();
	return 0;
}