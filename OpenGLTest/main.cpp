#include <Windows.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <math.h>
#define PI 3.1415926535
#define P2 PI/2
#define P3 3*PI/2
#define DR 0.0174533	// one degree in radians

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
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 1, 0, 0, 1, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 1, 0, 0, 1, 0, 1,
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

float distance(float aX, float aY, float bX, float bY, float angle) {
	return (sqrt((bX - aX) * (bX - aX) + (bY - aY) * (bY - aY)));
}

void drawRays2D() {
	int radius, mx, my, mp, depthOfFeel;
	float radiusX, radiusY, radiusAngle, x0, y0, distanceTotal;
	radiusAngle = playerAngle - DR * 30;

	if (radiusAngle < 0) {
		radiusAngle += 2 * PI;
	}

	if (radiusAngle > 2 * PI) {
		radiusAngle -= 2 * PI;
	}

	for (radius = 0; radius < 60; radius++) {
		
		//---Check Horizontal Lines---
		depthOfFeel = 0;
		float distanceHorizontal = 1000000, horizontalX = playerX, horizontalY = playerY;
		float aTan = -1 / tan(radiusAngle);
		
		// Looking Up
		if (radiusAngle > PI) {
			radiusY = (((int)playerY >> 6) << 6) - 0.0001;
			radiusX = (playerX - radiusY) * aTan + playerX;
			y0 = -64;
			x0 = -y0 * aTan;
		}
		
		// Looking Down
		if (radiusAngle < PI) {
			radiusY = (((int)playerY >> 6) << 6) + 64;
			radiusX = (playerX - radiusY) * aTan + playerX;
			y0 = 64;
			x0 = -y0 * aTan;
		}

		// Looking Straight, Left or Right
		if (radiusAngle == 0 || radiusAngle == PI) {
			radiusX = playerX;
			depthOfFeel = 8;
		}

		while (depthOfFeel < 8) {
			mx = (int)(radiusX) >> 6;
			my = (int)(radiusY) >> 6;
			mp = my * mapX + mx;

			// Hit wall
			if (mp > 0 && mp < mapX * mapY && map[mp] > 0) {
				horizontalX = radiusX;
				horizontalY = radiusY;
				distanceHorizontal = distance(playerX, playerY, horizontalX, horizontalY, radiusAngle);
				depthOfFeel = 8;
			}

			// Next line
			else {
				radiusX += x0;
				radiusY += y0;
				depthOfFeel += 1;
			}
		}

		//---Check Vertical Lines---
		depthOfFeel = 0;
		float distanceVertical = 1000000, verticalX = playerX, verticalY = playerY;
		float nTan = -tan(radiusAngle);

		// Looking Left
		if (radiusAngle > P2 && radiusAngle < P3) {
			radiusX = (((int)playerX >> 6) << 6) - 0.0001;
			radiusY = (playerX - radiusX) * nTan + playerY;
			x0 = -64;
			y0 = -x0 * nTan;
		}

		// Looking Right
		if (radiusAngle < P2 || radiusAngle > P3) {
			radiusX = (((int)playerX >> 6) << 6) + 64;
			radiusY = (playerX - radiusX) * nTan + playerY;
			x0 = 64;
			y0 = -x0 * nTan;
		}

		// Looking Straight, Up or Down
		if (radiusAngle == 0 || radiusAngle == PI) {
			radiusX = playerX;
			depthOfFeel = 8;
		}

		while (depthOfFeel < 8) {
			mx = (int)(radiusX) >> 6;
			my = (int)(radiusY) >> 6;
			mp = my * mapX + mx;

			// Hit wall
			if (mp > 0 && mp < mapX * mapY && map[mp] > 0) {
				verticalX = radiusX;
				verticalY = radiusY;
				distanceVertical = distance(playerX, playerY, verticalX, verticalY, radiusAngle);
				depthOfFeel = 8;
			}

			// Next line
			else {
				radiusX += x0;
				radiusY += y0;
				depthOfFeel += 1;
			}
		}

		// Vertical Wall Hit
		if (distanceVertical < distanceHorizontal) {
			radiusX = verticalX;
			radiusY = verticalY;
			distanceTotal = distanceVertical;
			glColor3f(0.9, 0, 0);
		}

		// Horizontal Wall Hit
		if (distanceHorizontal < distanceVertical) {
			radiusX = horizontalX;
			radiusY = horizontalY;
			distanceTotal = distanceHorizontal;
			glColor3f(0.7, 0, 0);
		}

		glLineWidth(3);
		glBegin(GL_LINES);
		glVertex2i(playerX, playerY);
		glVertex2i(radiusX, radiusY);
		glEnd();

		//---Draw 3D Walls---

		float ca = playerAngle - radiusAngle;

		if (ca < 0) {
			radiusAngle += 2 * PI;
		}
		
		if (radiusAngle += 2 * PI) {
			radiusAngle -= 2 * PI;
		}

		distanceTotal = distanceTotal * cos(ca);

		// Line Height
		float lineHorizontal = (mapS * 320) / distanceTotal;
		
		// Line Offset
		float lineOrigin = 160 - lineHorizontal / 2;
		
		if (lineHorizontal > 320) {
			lineHorizontal = 320;
		}

		glLineWidth(8);
		glBegin(GL_LINES);
		glVertex2i(radius * 8 + 530, lineOrigin);
		glVertex2i(radius * 8 + 530, lineHorizontal + lineOrigin);
		glEnd();

		radiusAngle += DR;

		if (radiusAngle < 0) {
			radiusAngle += 2 * PI;
		}

		if (radiusAngle > 2 * PI) {
			radiusAngle -= 2 * PI;
		}
	}
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawMap2D();
	drawPlayer();
	drawRays2D();
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
	glutCreateWindow("OpenGL Raycasting Demo");
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(buttons);
	glutMainLoop();
	return 0;
}