#define _CRT_SECURE_NO_WARNINGS 
#include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include <iostream>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

char* filetobuf(const char* file) {
	FILE* fptr;
	long length;
	char* buf;

	fptr = fopen(file, "rb");
	if (!fptr)
		return NULL;
	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);
	buf = (char*)malloc(length + 1);
	fseek(fptr, 0, SEEK_SET);
	fread(buf, length, 1, fptr);
	fclose(fptr);
	buf[length] = 0;
	return buf;
}

using namespace std;

struct ShapeData {
	vector<GLfloat> positions;
	vector<GLfloat> colors;
	GLuint vao, vbo[2];
};

ShapeData lineData, triangleData, rectangleData, pentagonData;

void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Timer(int value);
void updateShapeData(ShapeData& shapeData);
void updateBuffers();

GLint width, height;
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

int window_Width = 800;
int window_Height = 600;

bool isChanging = false;
int changeType = 0;

void initializeRandomSeed() {
	srand(static_cast<unsigned int>(time(0)));
}

void InitShapeData(ShapeData& shapeData) {
	glGenVertexArrays(1, &shapeData.vao);
	glBindVertexArray(shapeData.vao);
	glGenBuffers(2, shapeData.vbo);

	glBindBuffer(GL_ARRAY_BUFFER, shapeData.vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, shapeData.vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}

void InitBuffer() {
	InitShapeData(lineData);
	InitShapeData(triangleData);
	InitShapeData(rectangleData);
	InitShapeData(pentagonData);
}

void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(window_Width, window_Height);
	glutCreateWindow("Example11");

	glewExperimental = GL_TRUE;
	glewInit();

	initializeRandomSeed();

	make_shaderProgram();
	InitBuffer();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(16, Timer, 0);

	glutMainLoop();
}

GLchar* vertexSource, * fragmentSource;

void make_vertexShaders() {
	GLchar* vertexSource;

	vertexSource = filetobuf("vertex.glsl");

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
	glCompileShader(vertexShader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

void make_fragmentShaders() {
	GLchar* fragmentSource;

	fragmentSource = filetobuf("fragment.glsl");

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
	glCompileShader(fragmentShader);

	GLint result;
	GLchar errorLog[512];

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

GLuint make_shaderProgram() {
	make_vertexShaders();
	make_fragmentShaders();

	shaderProgramID = glCreateProgram();

	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glUseProgram(shaderProgramID);
	return shaderProgramID;
}

static GLfloat lineY = 0.8f;
static GLfloat triangleX = 0.2f;
static GLfloat rectangleY = -0.2f;
static GLfloat pentagonX1 = 0.2f, pentagonY1 = -0.3f;
static GLfloat pentagonX2 = 0.7f, pentagonX3 = 0.8f, pentagonY3 = -0.3f, pentagonX4 = 0.7f;

GLvoid Timer(int value) {
	if (isChanging) {
		switch (changeType) {
		case 1:
			if (lineY > 0.2f) {
				lineY -= 0.01f;
			}
			else {
				isChanging = false;
			}
			break;
		case 2:
			if (triangleX < 0.8f) {
				triangleX += 0.01f;
			}
			else {
				isChanging = false;
			}
			break;
		case 3:
			if (rectangleY > -0.4f) {
				rectangleY -= 0.01f;
			}
			else {
				isChanging = false;
			}
			break;
		case 4:
			if (pentagonX1 < 0.5f) pentagonX1 += 0.01f;
			if (pentagonY1 < -0.1f) pentagonY1 += 0.01f;
			if (pentagonX2 > 0.3f) pentagonX2 -= 0.01f;
			if (pentagonX3 > 0.5f) pentagonX3 -= 0.01f;
			if (pentagonY3 < -0.1f) pentagonY3 += 0.01f;
			if (pentagonX4 > 0.3f) pentagonX4 -= 0.01f;

			if (pentagonX1 >= 0.5f && pentagonY1 >= -0.2f && pentagonX2 <= 0.3f &&
				pentagonX3 <= 0.5f && pentagonY3 >= -0.2f && pentagonX4 <= 0.3f) {
				isChanging = false;
			}
			break;
		}
		glutPostRedisplay();
	}
	glutTimerFunc(16, Timer, 0);
}

GLvoid drawScene() {
	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	lineData.positions = { -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f };
	lineData.colors = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	updateShapeData(lineData);
	glBindVertexArray(lineData.vao);
	glDrawArrays(GL_LINES, 0, 4);

	triangleData.positions = { 0.2f, 0.2f, 0.0f, 0.8f, 0.2f, 0.0f, 0.2f, 0.8f, 0.0f, 0.8f, 0.2f, 0.0f, triangleX, 0.8f, 0.0f, 0.2f, 0.8f, 0.0f };
	triangleData.colors = { 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f };
	updateShapeData(triangleData);
	glBindVertexArray(triangleData.vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glLineWidth(5.0f);
	triangleData.positions = { -0.8f, 0.2f, 0.0f, -0.2f, 0.8f, 0.0f, -0.2f, lineY, 0.0f };
	triangleData.colors = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	updateShapeData(triangleData);
	glBindVertexArray(triangleData.vao);
	glDrawArrays(GL_LINE_LOOP, 0, 3);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	rectangleData.positions = {
	-0.8f, -0.8f, 0.0f, 
	-0.5f, -0.2f, 0.0f, 
	-0.8f, rectangleY, 0.0f,

	-0.5f, -0.2f, 0.0f,
	-0.8f, -0.8f, 0.0f,
	-0.2f, -0.8f, 0.0f,

	-0.5f, -0.2f, 0.0f,
	-0.2f, -0.8f, 0.0f,
	-0.2f, rectangleY, 0.0f };

	rectangleData.colors = {
	0.0f, 1.0f, 0.0f, 
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,

	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 
	0.0f, 1.0f, 0.0f,

	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f };

	updateShapeData(rectangleData);
	glBindVertexArray(rectangleData.vao);
	glDrawArrays(GL_TRIANGLES, 0, 9);

	glLineWidth(5.0f);
	pentagonData.positions = { 
		0.5f, -0.1f, 0.0f,
		pentagonX1, pentagonY1, 0.0f,
		0.3f, -0.8f, 0.0f,

		0.5f, -0.1f, 0.0f,
		0.3f, -0.8f, 0.0f,
		pentagonX2, -0.8f, 0.0f,

		0.5f, -0.1f, 0.0f,
		pentagonX4, -0.8f, 0.0f,
		pentagonX3, pentagonY3, 0.0f };

	pentagonData.colors = {
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f };

	updateShapeData(pentagonData);
	glBindVertexArray(pentagonData.vao);
	glDrawArrays(GL_LINE_LOOP, 0, 9);
	glDrawArrays(GL_TRIANGLES, 0, 9);

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'l':
		isChanging = true;
		changeType = 1;
		break;
	case 't':
		isChanging = true;
		changeType = 2;
		break;
	case 'r':
		isChanging = true;
		changeType = 3;
		break;
	case 'p':
		isChanging = true;
		changeType = 4;
		break;
	case 'a':
		lineY = 0.8f;
		triangleX = 0.2f;
		rectangleY = -0.2f;
		pentagonX1 = 0.2f;
		pentagonY1 = -0.3f;
		pentagonX2 = 0.7f;
		pentagonX3 = 0.8f;
		pentagonY3 = -0.3f;
		pentagonX4 = 0.7f;
		glutPostRedisplay();
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
}

void updateShapeData(ShapeData& shapeData) {
	glBindVertexArray(shapeData.vao);

	glBindBuffer(GL_ARRAY_BUFFER, shapeData.vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, shapeData.positions.size() * sizeof(GLfloat), shapeData.positions.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, shapeData.vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, shapeData.colors.size() * sizeof(GLfloat), shapeData.colors.data(), GL_DYNAMIC_DRAW);
}

void updateBuffers() {
	updateShapeData(lineData);
	updateShapeData(triangleData);
	updateShapeData(rectangleData);
	updateShapeData(pentagonData);
}