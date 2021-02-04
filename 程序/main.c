#include <stdio.h>
#include <Windows.h>
#include <glut.h>
#include "lodepng.h"

void init(void);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void display(void);
void drawCoordinates(void);
void drawCube12(void);
void drawCube34(void);
void drawCube56(void);
void upsidedown32(unsigned char* pImage, unsigned width, unsigned height);

int mx, my; //position of mouse
int m_state = 0; //mouse usage
float x_angle = 0.0f, y_angle = 0.0f; //angle of eye
float dist = 10.0f; //distance from the eye

float pnt[8][3] = { {0.0,0.0,0.0},{1.0,0.0,0.0}, {0.0,1.0,0.0}, {1.0,1.0,0.0},{0.0,0.0,1.0},{1.0,0.0,1.0}, {0.0,1.0,1.0}, {1.0,1.0,1.0} };
int shunxu[6][4] = { {4,5,7,6},{0,2,3,1},{1,3,7,5},{2,6,7,3},{0,4,6,2}, {0,1,5,4} };

GLuint texTouxiang;

void upsidedown32(unsigned char* pImage, unsigned width, unsigned height)
{
	unsigned i;
	unsigned char* pLine;

	pLine = (unsigned char*)malloc(4 * width);
	if (pLine == NULL)
	{
		printf("No memory left!");
		exit(0);
	}
	for (i = 0; i < height / 2; i++)
	{
		memcpy(pLine, &pImage[i * 4 * width], 4 * width);
		memcpy(&pImage[i * 4 * width], &pImage[(height - 1 - i) * 4 * width], 4 * width);
		memcpy(&pImage[(height - 1 - i) * 4 * width], pLine, 4 * width);
	}
	free(pLine);
}

void init(void)
{
	int i;
	unsigned error;
	unsigned width, height;
	unsigned char* pTouxiangImg;
	unsigned char img1D[16][3];

	//make touxiang texture
	error = lodepng_decode32_file(&pTouxiangImg, &width, &height, "touxiang.png");
	if (error)
	{
		printf("error %u: %s\n", error, lodepng_error_text(error));
		exit(0);
	}
	upsidedown32(pTouxiangImg, width, height); //Upside down the image
	glGenTextures(1, &texTouxiang);
	glBindTexture(GL_TEXTURE_2D, texTouxiang);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pTouxiangImg);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	free(pTouxiangImg);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glEnable(GL_DEPTH_TEST);
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}


void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		mx = x;
		my = y;
		m_state = 0;
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		mx = x;
		my = y;
		m_state = 1;
	}
}

void motion(int x, int y)
{
	int dx, dy; //offset of mouse;

	dx = x - mx;
	dy = y - my;

	if (m_state == 0)
	{
		y_angle += dx * 0.1f;
		x_angle += dy * 0.1f;
	}
	else if (m_state == 1)
		dist += (dx + dy) * 0.01f;

	mx = x;
	my = y;

	glutPostRedisplay();
}

void display(void)
{
	float lit_position[] = { 0.0f, 0.0f, 1.0f, 0.0f };
	int rect[4];
	float w, h;

	glGetIntegerv(GL_VIEWPORT, rect);
	w = rect[2];
	h = rect[3];

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); //对应单位阵I

	if (h < 1) h = 1;
	gluPerspective(30.0, w / h, 0.1, 20.0); //对应变换阵T0

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); //对应单位阵I

	glLightfv(GL_LIGHT0, GL_POSITION, lit_position);

	glTranslated(0.0, 0.0, -dist); //对应变换阵T1
	glRotatef(x_angle, 1.0f, 0.0f, 0.0f); //对应变换阵T2
	glRotatef(y_angle, 0.0f, 1.0f, 0.0f); //对应变换阵T3
	drawCoordinates();
	drawCube12();
	drawCube34();
	drawCube56();

//	glFlush();
	glutSwapBuffers();
}

void drawCoordinates(void)
{
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f); //画红色的x轴
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glColor3f(0.0, 1.0, 0.0); //画绿色的y轴
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glColor3f(0.0, 0.0, 1.0); //画蓝色的z轴
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 1.0f);
	glEnd();
}

void drawCube12(void)
{
	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON); //第一面
	glColor3f(1.0f, 0.0f, 0.0f); glVertex3fv(pnt[shunxu[0][0]]);
	glColor3f(0.0f, 1.0f, 0.0f); glVertex3fv(pnt[shunxu[0][1]]);
	glColor3f(0.0f, 0.0f, 1.0f); glVertex3fv(pnt[shunxu[0][2]]);
	glColor3f(1.0f, 1.0f, 1.0f); glVertex3fv(pnt[shunxu[0][3]]);
	glEnd();
	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON); //第二面
	glColor3f(1.0f, 0.0f, 0.0f); glVertex3fv(pnt[shunxu[1][0]]);
	glColor3f(0.0f, 1.0f, 0.0f); glVertex3fv(pnt[shunxu[1][1]]);
	glColor3f(0.0f, 0.0f, 1.0f); glVertex3fv(pnt[shunxu[1][2]]);
	glColor3f(1.0f, 1.0f, 1.0f); glVertex3fv(pnt[shunxu[1][3]]);
	glEnd();
}

void drawCube34(void)
{
	float mat_yellow[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	float mat_cyan[] = { 0.0f, 1.0f, 1.0f, 1.0f };

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_yellow);
	glMaterialfv(GL_BACK, GL_DIFFUSE, mat_cyan);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON); //第三面
	glVertex3fv(pnt[shunxu[2][0]]);
	glVertex3fv(pnt[shunxu[2][1]]);
	glVertex3fv(pnt[shunxu[2][2]]);
	glVertex3fv(pnt[shunxu[2][3]]);
	glEnd();
	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON); //第四面
	glVertex3fv(pnt[shunxu[3][0]]);
	glVertex3fv(pnt[shunxu[3][1]]);
	glVertex3fv(pnt[shunxu[3][2]]);
	glVertex3fv(pnt[shunxu[3][3]]);
	glEnd();

	glDisable(GL_LIGHTING);
}

void drawCube56(void)
{
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glDisable(GL_TEXTURE_1D);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texTouxiang);
	glBegin(GL_POLYGON); //第一面
	glTexCoord2f(1.0f, 0.0f); glVertex3fv(pnt[shunxu[4][0]]);
	glTexCoord2f(1.0f, 1.0f); glVertex3fv(pnt[shunxu[4][1]]);
	glTexCoord2f(0.0f, 1.0f); glVertex3fv(pnt[shunxu[4][2]]);
	glTexCoord2f(0.0f, 0.0f); glVertex3fv(pnt[shunxu[4][3]]);
	glEnd();

	glNormal3f(0.0f, -1.0f, 0.0f);
	glDisable(GL_TEXTURE_1D);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_POLYGON); //第二面
	glTexCoord2f(0.0f, 0.0f); glVertex3fv(pnt[shunxu[5][0]]);
	glTexCoord2f(1.0f, 0.0f); glVertex3fv(pnt[shunxu[5][1]]);
	glTexCoord2f(1.0f, 1.0f); glVertex3fv(pnt[shunxu[5][2]]);
	glTexCoord2f(0.0f, 1.0f); glVertex3fv(pnt[shunxu[5][3]]);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_1D);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Assignment by Haitao");
	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutMainLoop();

	glDeleteTextures(1, &texTouxiang);

	

	return 0;
}