#include <iostream>
#include <random>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

using namespace std;

random_device seeder;
const auto seed = seeder.entropy() ? seeder() : time(nullptr);
mt19937 eng(static_cast<mt19937::result_type>(seed));
uniform_real_distribution<double> dist(0.0f, 1.0f);

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);

void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Example1");

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cerr << "Unalble to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else {
		std::cout << "GLEW Initialized\n";
	}
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(Keyboard);
	glutReshapeFunc(Reshape);
	glutMainLoop();
}

GLclampf r = 1.0f;
GLclampf g = 1.0f;
GLclampf b = 1.0f;

bool IsTimerAlive = true;

GLvoid drawScene() {
	glClearColor(r, g, b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	// 그리기 관련 부분
	glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);
}

void RandomRGB() {
	r = (GLclampf)dist(eng);
	g = (GLclampf)dist(eng);
	b = (GLclampf)dist(eng);
}

void TimerFunction(int value) {
	RandomRGB();
	glutPostRedisplay();
	if (IsTimerAlive) {
		glutTimerFunc(100, TimerFunction, 1);
	}
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'c':
		r = 0.0f;
		g = 1.0f;
		b = 1.0f;
		break;
	case 'm':
		r = 1.0f;
		g = 0.0f;
		b = 1.0f;
		break;
	case 'y':
		r = 1.0f;
		g = 1.0f;
		b = 0.0f;
		break;
	case 'a':
		RandomRGB();
		break;
	case 'w':
		r = 1.0f;
		g = 1.0f;
		b = 1.0f;
		break;
	case 'k':
		r = 0.0f;
		g = 0.0f;
		b = 0.0f;
		break;
	case 't':
		glutTimerFunc(1000, TimerFunction, 1);
		IsTimerAlive = true;
		break;
	case 's':
		IsTimerAlive = false;
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}