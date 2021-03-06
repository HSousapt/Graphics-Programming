

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>

#include <IL/il.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

float camX = 00, camY = 70, camZ = 50;
int startX, startY, tracking = 0;
int polMode = 0;

int alpha = 0, beta = 45, r = 100;

int frame = 0, timebase = 0;

unsigned int imageHeight, imageWidth;
GLuint verteces, vertecesCount;

void changeSize(int w, int h)
{

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if (h == 0)
		h = 1;

	// compute window's aspect ratio
	float ratio = w * 1.0 / h;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective
	gluPerspective(45, ratio, 1, 1000);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}

void drawTerrain()
{
	glBindBuffer(GL_ARRAY_BUFFER, verteces);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	for (int i = 0; i < imageHeight - 1; i++)
	{
		glDrawArrays(GL_TRIANGLE_STRIP, (imageWidth)*2 * i, (imageWidth)*2);
	}
}

void show_fps(void)
{
	int time;
	char s[64];
	float fps;

	frame++;
	time = glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000)
	{
		fps = frame * 1000.0 / (time - timebase);
		timebase = time;
		frame = 0;
		sprintf(s, "FPS: %6.2f", fps);
		glutSetWindowTitle(s);
	}
}

void renderScene(void)
{

	float pos[4] = {-1.0, 1.0, 1.0, 0.0};

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(camX, camY, camZ,
			  0.0, 0.0, 0.0,
			  0.0f, 1.0f, 0.0f);

	drawTerrain();

	show_fps();

	// End of frame
	glutSwapBuffers();
}

void processKeys(unsigned char key, int xx, int yy)
{
	if (key == 'm')
	{
		polMode = (polMode + 1) % 3;
		if (polMode == 0)
			glPolygonMode(GL_FRONT, GL_LINE);
		if (polMode == 1)
			glPolygonMode(GL_FRONT, GL_POINT);
		if (polMode == 2)
			glPolygonMode(GL_FRONT, GL_FILL);
		glutPostRedisplay();
	}
}

void processMouseButtons(int button, int state, int xx, int yy)
{

	if (state == GLUT_DOWN)
	{
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON)
			tracking = 1;
		else if (button == GLUT_RIGHT_BUTTON)
			tracking = 2;
		else
			tracking = 0;
	}
	else if (state == GLUT_UP)
	{
		if (tracking == 1)
		{
			alpha += (xx - startX);
			beta += (yy - startY);
		}
		else if (tracking == 2)
		{

			r -= yy - startY;
			if (r < 3)
				r = 3.0;
		}
		tracking = 0;
	}
}

void processMouseMotion(int xx, int yy)
{

	int deltaX, deltaY;
	int alphaAux, betaAux;
	int rAux;

	if (!tracking)
		return;

	deltaX = xx - startX;
	deltaY = yy - startY;

	if (tracking == 1)
	{

		alphaAux = alpha + deltaX;
		betaAux = beta + deltaY;

		if (betaAux > 85.0)
			betaAux = 85.0;
		else if (betaAux < -85.0)
			betaAux = -85.0;

		rAux = r;
	}
	else if (tracking == 2)
	{

		alphaAux = alpha;
		betaAux = beta;
		rAux = r - deltaY;
		if (rAux < 3)
			rAux = 3;
	}
	camX = rAux * sin(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
	camZ = rAux * cos(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
	camY = rAux * sin(betaAux * 3.14 / 180.0);
}

float height(ILubyte *imageData, int i, int j)
{
	return (float)imageData[i * imageWidth + j];
}

void fill_vertex_array(ILubyte *imageData, int th, int tw)
{
	std::vector<float> ps;
	glBindBuffer(GL_ARRAY_BUFFER, verteces);
	float h = ((float)th - 1) / 2;
	float w = ((float)tw - 1) / 2;
	for (int i = 1; i <= th; i++)
	{
		for (int j = 1; j <= tw; j++)
		{
			//Point a
			ps.push_back(-w + (j - 1));
			ps.push_back(height(imageData, (i - 1), (j - 1)));
			ps.push_back(-h + (i - 1));
			//Point b
			ps.push_back(-w + (j - 1));
			ps.push_back(height(imageData, i, (j - 1)));
			ps.push_back(-h + i);
		}
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ps.size(), ps.data(), GL_STATIC_DRAW);
}

void init()
{

	verteces = 0;
	// 	Load the height map "terreno.jpg"
	unsigned int t;
	ILubyte *imageData;
	ilGenImages(1, &t);
	ilBindImage(t);
	// terreno.jpg is the image containing our height map
	ilLoadImage((ILstring) "terreno.jpg");
	// convert the image to single channel per pixel
	// with values ranging between 0 and 255
	ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);

	imageHeight = ilGetInteger(IL_IMAGE_WIDTH);
	imageWidth = ilGetInteger(IL_IMAGE_HEIGHT);
	// imageData is a LINEAR array with the pixel values
	imageData = ilGetData();

	glGenBuffers(1, &verteces);
	// 	Build the vertex arrays
	fill_vertex_array(imageData, imageHeight, imageWidth);

	// 	OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

int main(int argc, char **argv)
{

	// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("CG@DI-UM");

	// Required callback registry
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	ilInit();

	// Callback registration for keyboard processing
	glutKeyboardFunc(processKeys);
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);

	if (glewInit() != GLEW_OK)
	{
		printf("glew initialization failed!");
	}

	glEnableClientState(GL_VERTEX_ARRAY);

	glPolygonMode(GL_FRONT, GL_LINE);

	init();

	// enter GLUT's main cycle
	glutMainLoop();

	return 0;
}
