//Jeff Chastine
#include <Windows.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <math.h>
#define PI 3.1415926535

using namespace std;

//player position
float playerX, playerY, playerXDelta, playerYDelta, playerAngle;

void drawPlayer() {
	glColor3f(1, 1, 0);
	glPointSize(8);
	glBegin(GL_POINTS);
	glVertex2i(playerX, playerY);
	glEnd();

	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2i(playerX, playerY);
	glVertex2i(playerX + playerXDelta * 5, playerY + playerYDelta * 5);
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

void drawRays3d() {
	int radius, mx, my, mp, depthOfFeel;
	float radiusX, radiusY, radiusAngle, x0, y0;

	radiusAngle = playerAngle;

	for (radius = 0; radius < 1; radius++) {
		
		// ---Check Horizontal Lines
		depthOfFeel = 0;
		float aTan = -1 / tan(radiusAngle);
		
		if (radiusAngle > PI) {
			radiusY = (((int)playerY >> 6) << 6) - 0.0001;
			radiusX = (playerX - radiusY) * aTan + playerX;
			y0 = -64;
			x0 = -y0 * aTan;
		}
		
		if (radiusAngle < PI) {
			radiusY = (((int)playerY >> 6) << 6) + 64;
			radiusX = (playerX - radiusY) * aTan + playerX;
			y0 = 64;
			x0 = -y0 * aTan;
		}

		if (radiusAngle == 0 || radiusAngle == PI) {
			radiusX = playerX;
			depthOfFeel = 8;
		}

		while (depthOfFeel < 8) {
			mx = (int)(radiusX) >> 6;
			my = (int)(radiusY) >> 6;
			mp = my * mapX + mx;

			// Hit wall
			if (mp < mapX * mapY && map[mp] == 1) {
				depthOfFeel = 8;
			}

			// Next line
			else {
				radiusX += x0;
				radiusY += y0;
				depthOfFeel += 1;
			}
		}

		glColor3f(0, 1, 0);
		glLineWidth(1);
		glBegin(GL_LINES);
		glVertex2i(playerX, playerY);
		glVertex2i(radiusX, radiusY);
		glEnd();
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
		playerAngle -= 0.1;
		
		if (playerAngle < 0) {
			playerAngle += 2 * PI;
		}

		playerXDelta = cos(playerAngle) * 5;
		playerYDelta = sin(playerAngle) * 5;
	}

	if (key == 'd') {
		playerAngle += 0.1;

		if (playerAngle < 0) {
			playerAngle -= 2 * PI;
		}

		playerXDelta = cos(playerAngle) * 5;
		playerYDelta = sin(playerAngle) * 5;
	}

	if (key == 'w') {
		playerX += playerXDelta;
		playerY += playerYDelta;
	}

	if (key == 's') {
		playerX -= playerXDelta;
		playerY -= playerYDelta;
	}

	glutPostRedisplay();
}

void init() {
	glClearColor(0.3, 0.3, 0.3, 0);
	gluOrtho2D(0, 1024, 512, 0);
	playerX = 300; 
	playerY = 300;
	playerXDelta = cos(playerAngle) * 5;
	playerYDelta = sin(playerAngle) * 5;
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