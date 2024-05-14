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

//-----------------------------MAP----------------------------------------------
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

//-----------------------------------------------------------------------------

//------------------------PLAYER------------------------------------------------
float degToRad(int angle) { 
	return angle * PI / 180.0;
}

int FixAng(int angle) {
	
	if (angle > 359) {
		angle -= 360;
	} 
	
	if (angle < 0) {
		angle += 360;
	} 
	
	return angle;
}

//player position
float playerX, playerY, playerXDelta, playerYDelta, playerAngle;

void drawPlayer() {
	glColor3f(1, 1, 0);
	glPointSize(8);
	glLineWidth(4);
	glBegin(GL_POINTS);
	glVertex2i(playerX, playerY);
	glEnd();

	glBegin(GL_LINES);
	glVertex2i(playerX, playerY);
	glVertex2i(playerX + playerXDelta * 20, playerY + playerYDelta * 20);
	glEnd();
}

void buttons(unsigned char key, int x, int y) {

	if (key == 'a') {
		playerAngle += 5;
		playerAngle = FixAng(playerAngle);
		playerXDelta = cos(degToRad(playerAngle));
		playerYDelta = -sin(degToRad(playerAngle));
	}

	if (key == 'd') {
		playerAngle -= 5;
		playerAngle = FixAng(playerAngle);
		playerXDelta = cos(degToRad(playerAngle));
		playerYDelta = -sin(degToRad(playerAngle));
	}

	if (key == 'w') {
		playerX += playerXDelta * 5;
		playerY += playerYDelta * 5;
	}

	if (key == 's') {
		playerX -= playerXDelta * 5;
		playerY -= playerYDelta * 5;
	}

	glutPostRedisplay();
}

//-----------------------------------------------------------------------------

//---------------------------Draw Rays and Walls--------------------------------

float distance(float aX, float aY, float bX, float bY, float angle) {
	return cos(degToRad(angle)) * (bX - aX) - sin(degToRad(angle)) * (bY - aY);
}

void drawRays2D() {

	glColor3f(0, 1, 1); 
	glBegin(GL_QUADS); 
	glVertex2i(526, 0); 
	glVertex2i(1006, 0); 
	glVertex2i(1006, 160); 
	glVertex2i(526, 160); 
	glEnd();

	glColor3f(0, 0, 1); 
	glBegin(GL_QUADS); 
	glVertex2i(526, 160); 
	glVertex2i(1006, 160); 
	glVertex2i(1006, 320); 
	glVertex2i(526, 320); 
	glEnd();

	int radius, mx, my, mp, depthOfFeel, side;
	float verticalX, verticalY, radiusX, radiusY, radiusAngle, x0, y0, distanceVertical, distanceHorizontal;

	// Ray sets back to 30 degrees
	radiusAngle = FixAng(playerAngle + 30);

	for (radius = 0; radius < 60; radius++) {
		
		//---Check Vertical Lines---
		depthOfFeel = 0;
		side = 0;
		distanceVertical = 1000000;
		float Tan = tan(degToRad(radiusAngle));

		// Looking Left
		if (cos(degToRad(radiusAngle)) > 0.001) {
			radiusX = (((int)playerX >> 6) << 6) + 64;
			radiusY = (playerX - radiusX) * Tan + playerY;
			x0 = 64;
			y0 = -x0 * Tan;
		}

		// Looking Right
		else if (cos(degToRad(radiusAngle)) < -0.001) {
			radiusX = (((int)playerX >> 6) << 6) - 0.0001;
			radiusY = (playerX - radiusX) * Tan + playerY;
			x0 = -64;
			y0 = -x0 * Tan;
		}

		// Looking Straight, Up or Down
		else {
			radiusX = playerX;
			radiusY = playerY;
			depthOfFeel = 8;
		}

		while (depthOfFeel < 8) {
			mx = (int)(radiusX) >> 6;
			my = (int)(radiusY) >> 6;
			mp = my * mapX + mx;

			// Hit wall
			if (mp > 0 && mp < mapX * mapY && map[mp] == 1) {
				depthOfFeel = 8;
				distanceVertical = cos(degToRad(radiusAngle)) * (radiusX - playerX) - sin(degToRad(radiusAngle)) * (radiusY - playerY);
			}

			// Next line
			else {
				radiusX += x0;
				radiusY += y0;
				depthOfFeel += 1;
			}
		}

		verticalX = radiusX;
		verticalY = radiusY;

		//---Check Horizontal Lines---
		depthOfFeel = 0;
		distanceHorizontal = 1000000;
		Tan = 1 / Tan;
		
		// Looking Up
		if (sin(degToRad(radiusAngle)) > 0.001) {
			radiusY = (((int)playerY >> 6) << 6) - 0.0001;
			radiusX = (playerX - radiusY) * Tan + playerX;
			y0 = -64;
			x0 = -y0 * Tan;
		}
		
		// Looking Down
		else if (sin(degToRad(radiusAngle)) < -0.001) {
			radiusY = (((int)playerY >> 6) << 6) + 64;
			radiusX = (playerX - radiusY) * Tan + playerX;
			y0 = 64;
			x0 = -y0 * Tan;
		}

		// Looking Straight, Up or Down
		else {
			radiusX = playerX;
			radiusY = playerY;
			depthOfFeel = 8;
		}

		while (depthOfFeel < 8) {
			mx = (int)(radiusX) >> 6;
			my = (int)(radiusY) >> 6;
			mp = my * mapX + mx;

			// Hit wall
			if (mp > 0 && mp < mapX * mapY && map[mp] == 1) {
				depthOfFeel = 8;
				distanceHorizontal = cos(degToRad(radiusAngle)) * (radiusX - playerX) - sin(degToRad(radiusAngle)) * (radiusY - playerY);
			}

			// Next line
			else {
				radiusX += x0;
				radiusY += y0;
				depthOfFeel += 1;
			}
		}

		// Horizontal hit first
		glColor3f(0, 0.8, 0);
		if (distanceVertical < distanceHorizontal) { 
			radiusX = verticalX; 
			radiusY = verticalY; 
			distanceHorizontal = distanceVertical; 
			glColor3f(0, 0.6, 0); 
		}

		// Draw 2D ray
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex2i(playerX, playerY);
		glVertex2i(radiusX, radiusY);
		glEnd();

		// Fix Fisheye
		int ca = FixAng(playerAngle - radiusAngle); distanceHorizontal = distanceHorizontal * cos(degToRad(ca));

		// Line height and limit
		int lineHorizontal = (mapS * 320) / (distanceHorizontal); 
		
		if (lineHorizontal > 320) {
			lineHorizontal = 320;
		}

		// Line offset
		int lineOff = 160 - (lineHorizontal >> 1);

		// Draw vertical wall
		glLineWidth(8);
		glBegin(GL_LINES);
		glVertex2i(radius * 8 + 530, lineOff);
		glVertex2i(radius * 8 + 530, lineHorizontal + lineOff);
		glEnd();

		// Go to next ray
		radiusAngle = FixAng(radiusAngle - 1);
	}
}

void init() {
	glClearColor(0.3, 0.3, 0.3, 0);
	gluOrtho2D(0, 1024, 510, 0);
	playerX = 300;
	playerY = 300;
	playerXDelta = cos(degToRad(playerAngle));
	playerYDelta = -sin(degToRad(playerAngle));
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawMap2D();
	drawPlayer();
	drawRays2D();
	glutSwapBuffers();
}

int main(int argc, char* argv[]) {
	
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(1024, 510);
	glutCreateWindow("OpenGL Raycasting Demo");
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(buttons);
	glutMainLoop();
	return 0;
}