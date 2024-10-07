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

ShapeData pointData, lineData, triangleData, rectangleData;
bool drawPointMode = false, drawLineMode = false, drawTriangleMode = false, drawRectangleMode = false;
bool isLineFirstClick = false;
GLfloat lineStartX, lineStartY;

void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
void updateShapeData(ShapeData& shapeData);
void updateBuffers();

GLint width, height;
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

int window_Width = 800;
int window_Height = 600;
int selectedShapeIndex = -1;
int selectedShapeType = -1;

const int maxShapeCount = 10;

int getTotalShapeCount() {
	return (pointData.positions.size() / 3) +
		(lineData.positions.size() / 6) +
		(triangleData.positions.size() / 9) +
		(rectangleData.positions.size() / 12);
}

void initializeRandomSeed() {
	srand(static_cast<unsigned int>(time(0)));
}

void selectRandomShape() {
	int totalShapes = getTotalShapeCount();
	if (totalShapes == 0) {
		selectedShapeIndex = -1;
		selectedShapeType = -1;
		return;
	}

	int randomShape = rand() % totalShapes;
	int count = 0;

	if (pointData.positions.size() / 3 > randomShape) {
		selectedShapeType = 0;
		selectedShapeIndex = randomShape;
		return;
	}
	count += pointData.positions.size() / 3;

	if (lineData.positions.size() / 6 > randomShape - count) {
		selectedShapeType = 1;
		selectedShapeIndex = randomShape - count;
		return;
	}
	count += lineData.positions.size() / 6;

	if (triangleData.positions.size() / 9 > randomShape - count) {
		selectedShapeType = 2;
		selectedShapeIndex = randomShape - count;
		return;
	}
	count += triangleData.positions.size() / 9;

	if (rectangleData.positions.size() / 12 > randomShape - count) {
		selectedShapeType = 3;
		selectedShapeIndex = randomShape - count;
		return;
	}
	count += rectangleData.positions.size() / 12;
}

void moveSelectedShape(float dx, float dy) {
	if (selectedShapeIndex == -1 || selectedShapeType == -1) return;

	vector<GLfloat>* positions = nullptr;
	int vertexCount = 0;

	switch (selectedShapeType) {
	case 0:
		positions = &pointData.positions;
		vertexCount = 1;
		break;
	case 1:
		positions = &lineData.positions;
		vertexCount = 2;
		break;
	case 2:
		positions = &triangleData.positions;
		vertexCount = 3;
		break;
	case 3:
		positions = &rectangleData.positions;
		vertexCount = 6;
		break;
	}

	if (positions) {
		int baseIndex = selectedShapeIndex * vertexCount * 3;
		if (baseIndex + (vertexCount * 3) <= positions->size()) {
			for (int i = 0; i < vertexCount; ++i) {
				(*positions)[baseIndex + i * 3] += dx;
				(*positions)[baseIndex + i * 3 + 1] += dy;
			}
		}
	}

	updateBuffers();
	glutPostRedisplay();
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
	InitShapeData(pointData);
	InitShapeData(lineData);
	InitShapeData(triangleData);
	InitShapeData(rectangleData);
}

void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(window_Width, window_Height);
	glutCreateWindow("Example7");

	glewExperimental = GL_TRUE;
	glewInit();

	initializeRandomSeed();

	make_shaderProgram();
	InitBuffer();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);

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

GLvoid drawScene() {
	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);

	auto drawShape = [](ShapeData& shapeData, GLenum mode, float pointSize = 1.0f) {
		if (!shapeData.positions.empty()) {
			glBindVertexArray(shapeData.vao);

			if (mode == GL_POINTS) {
				glPointSize(pointSize);
			}
			glDrawArrays(mode, 0, shapeData.positions.size() / 3);
		}
	};

	drawShape(pointData, GL_POINTS, 5.0f);
	drawShape(lineData, GL_LINES);
	drawShape(triangleData, GL_TRIANGLES);
	drawShape(rectangleData, GL_TRIANGLES);

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);
}

GLvoid Mouse(int button, int state, int x, int y) {
	GLfloat x_opengl = (2.0f * x / window_Width) - 1.0f;
	GLfloat y_opengl = 1.0f - (2.0f * y / window_Height);

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (getTotalShapeCount() > maxShapeCount) {
			return;
		}

		if (drawPointMode) {			
			drawPointMode = false;

			pointData.positions.push_back(x_opengl);
			pointData.positions.push_back(y_opengl);
			pointData.positions.push_back(0.0f);

			pointData.colors.push_back(0.0f);
			pointData.colors.push_back(0.0f);
			pointData.colors.push_back(1.0f);

			updateShapeData(pointData);
			glutPostRedisplay();
		}
		else if(drawLineMode) {
			if (!isLineFirstClick) {
				lineStartX = x_opengl;
				lineStartY = y_opengl;
				isLineFirstClick = true;
			}
			else {
				isLineFirstClick = false;
				drawLineMode = false;

				lineData.positions.push_back(lineStartX);
				lineData.positions.push_back(lineStartY);
				lineData.positions.push_back(0.0f);

				lineData.positions.push_back(x_opengl);
				lineData.positions.push_back(y_opengl);
				lineData.positions.push_back(0.0f);

				lineData.colors.push_back(0.0f);
				lineData.colors.push_back(1.0f);
				lineData.colors.push_back(0.0f);

				lineData.colors.push_back(0.0f);
				lineData.colors.push_back(1.0f);
				lineData.colors.push_back(0.0f);

				updateShapeData(lineData);
				glutPostRedisplay();
			}
		}
		else if (drawTriangleMode) {
			drawTriangleMode = false;

			GLfloat size = 0.1f;
			GLfloat x1 = x_opengl;
			GLfloat y1 = y_opengl + size;

			GLfloat x2 = x_opengl - size * 0.886f;
			GLfloat y2 = y_opengl - size * 0.5f;

			GLfloat x3 = x_opengl + size * 0.886f;
			GLfloat y3 = y_opengl - size * 0.5f;

			triangleData.positions.insert(triangleData.positions.end(), { x1, y1, 0.0f, x2, y2, 0.0f, x3, y3, 0.0f });
			triangleData.colors.insert(triangleData.colors.end(), { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f });

			updateShapeData(triangleData);
			glutPostRedisplay();
		}
		else if (drawRectangleMode) {
			drawRectangleMode = false;

			GLfloat width = 0.2f, height = 0.1f;

			GLfloat x1 = x_opengl;
			GLfloat y1 = y_opengl;
			GLfloat x2 = x_opengl + width;
			GLfloat y2 = y_opengl;
			GLfloat x3 = x_opengl;
			GLfloat y3 = y_opengl - height;
			GLfloat x4 = x_opengl + width;
			GLfloat y4 = y_opengl - height;

			rectangleData.positions.insert(rectangleData.positions.end(), {
				x1, y1, 0.0f, x2, y2, 0.0f, x3, y3, 0.0f,
				x2, y2, 0.0f, x4, y4, 0.0f, x3, y3, 0.0f
				});

			rectangleData.colors.insert(rectangleData.colors.end(), {
				1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
				1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f
				});

			updateShapeData(rectangleData);
			glutPostRedisplay();
		}
	}
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'p':
		drawPointMode = true;
		break;
	case 'l':
		drawLineMode = true;
		isLineFirstClick = false;
		break;
	case 't':
		drawTriangleMode = true;
		break;
	case 'r':
		drawRectangleMode = true;
		break;
	case 'w':
		selectRandomShape();
		moveSelectedShape(0.0f, 0.05f);
		break;
	case 'a':
		selectRandomShape();
		moveSelectedShape(-0.05f, 0.0f);
		break;
	case 's':
		selectRandomShape();
		moveSelectedShape(0.0f, -0.05f);
		break;
	case 'd':
		selectRandomShape();
		moveSelectedShape(0.05f, 0.0f);
		break;
	case 'c':
		pointData.positions.clear();
		pointData.colors.clear();
		lineData.positions.clear();
		lineData.colors.clear();
		triangleData.positions.clear();
		triangleData.colors.clear();
		rectangleData.positions.clear();
		rectangleData.colors.clear();

		updateBuffers();
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
	updateShapeData(pointData);
	updateShapeData(lineData);
	updateShapeData(triangleData);
	updateShapeData(rectangleData);
}