#define _CRT_SECURE_NO_WARNINGS 
#include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include <iostream>
#include <vector>
#include <random>
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

const int NUM_SHAPES = 3;

struct ShapeData {
	vector<GLfloat> positions;
	vector<GLfloat> colors;
	GLuint vao, vbo[2];
	int vertexCount;
	GLfloat x, y;
	GLfloat dragOffsetX, dragOffsetY;
	GLfloat velocityX = 0.0f;
	GLfloat velocityY = 0.0f;
};

std::vector<ShapeData> pointData;
std::vector<ShapeData> lineData;
std::vector<ShapeData> triangleData;
std::vector<ShapeData> rectangleData;
std::vector<ShapeData> pentagonData;
std::vector<ShapeData> hexagonData;

random_device rd;
default_random_engine dre(rd());
uniform_real_distribution<float> randomPosition(-1.0f, 1.0f);
uniform_real_distribution<float> randomColor(0.0f, 1.0f);

void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Motion(int x, int y);
GLvoid Timer(int value);
void updateShapeData(ShapeData& shapeData);
void InitBuffer();
bool isMouseOverShape(const ShapeData& shape, GLfloat mouseX, GLfloat mouseY);
bool isShapeOverlapping(const ShapeData& shape1, const ShapeData& shape2);

GLint width, height;
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

int window_Width = 800;
int window_Height = 600;

bool isDragging = false;
ShapeData* draggedShape = nullptr;

void initializeRandomSeed() {
	srand(static_cast<unsigned int>(time(0)));
}

void InitShapeData(ShapeData& shapeData) {
	glGenVertexArrays(1, &shapeData.vao);
	glBindVertexArray(shapeData.vao);
	glGenBuffers(2, shapeData.vbo);

	glBindBuffer(GL_ARRAY_BUFFER, shapeData.vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, shapeData.positions.size() * sizeof(GLfloat), shapeData.positions.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, shapeData.vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, shapeData.colors.size() * sizeof(GLfloat), shapeData.colors.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}

void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(window_Width, window_Height);
	glutCreateWindow("Example12");

	glewExperimental = GL_TRUE;
	glewInit();

	initializeRandomSeed();
	make_shaderProgram();
	InitBuffer();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
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

void InitBuffer() {
	for (int i = 0; i < 3; ++i) {
		ShapeData newPoint, newLine, newTriangle, newRectangle, newPentagon, newHexagon;

		float pointXpos = randomPosition(dre);
		float pointYpos = randomPosition(dre);
		newPoint.positions = {
			pointXpos, pointYpos, 0.0f,
			pointXpos + 0.02f, pointYpos, 0.0f,
			pointXpos, pointYpos + 0.02f, 0.0f,
			pointXpos + 0.02f, pointYpos + 0.02f, 0.0f
		};
		newPoint.x = pointXpos;
		newPoint.y = pointYpos;
		float r = randomColor(dre);
		float g = randomColor(dre);
		float b = randomColor(dre);
		newPoint.colors = { r, g, b, r, g, b, r, g, b, r, g, b };
		newPoint.vertexCount = 1;
		InitShapeData(newPoint);
		pointData.push_back(newPoint);

		float lineXPos = randomPosition(dre);
		float lineYPos = randomPosition(dre);
		newLine.positions = {
			lineXPos, lineYPos, 0.0f,
			lineXPos + 0.3f, lineYPos + 0.3f, 0.0f
		};
		newLine.x = lineXPos;
		newLine.y = lineYPos;
		r = randomColor(dre);
		g = randomColor(dre);
		b = randomColor(dre);
		newLine.colors = { r, g, b, r, g, b };
		newLine.vertexCount = 2;
		InitShapeData(newLine);
		lineData.push_back(newLine);

		float triXPos = randomPosition(dre);
		float triYPos = randomPosition(dre);
		newTriangle.positions = {
			triXPos - 0.15f, triYPos - 0.15f, 0.0f,
			triXPos + 0.15f, triYPos - 0.15f, 0.0f,
			triXPos, triYPos + 0.15f, 0.0f
		};
		newTriangle.x = triXPos;
		newTriangle.y = triYPos;
		r = randomColor(dre);
		g = randomColor(dre);
		b = randomColor(dre);
		newTriangle.colors = { r, g, b, r, g, b, r, g, b };
		newTriangle.vertexCount = 3;
		InitShapeData(newTriangle);
		triangleData.push_back(newTriangle);

		float rectXPos = randomPosition(dre);
		float rectYPos = randomPosition(dre);
		newRectangle.positions = {
			rectXPos, rectYPos, 0.0f,
			rectXPos + 0.3f, rectYPos, 0.0f,
			rectXPos, rectYPos + 0.3f, 0.0f,
			rectXPos + 0.3f, rectYPos + 0.3f, 0.0f
		};
		newRectangle.x = rectXPos;
		newRectangle.y = rectYPos;
		r = randomColor(dre);
		g = randomColor(dre);
		b = randomColor(dre);
		newRectangle.colors = { r, g, b, r, g, b, r, g, b, r, g, b };
		newRectangle.vertexCount = 4;
		InitShapeData(newRectangle);
		rectangleData.push_back(newRectangle);

		float pentXPos = randomPosition(dre);
		float pentYPos = randomPosition(dre);
		const float radius = 0.15f;
		const int numSides = 5;
		const float angleIncrement = 2.0f * 3.1415926f / numSides;

		newPentagon.positions.clear();

		for (int j = 0; j < numSides; ++j) {
			float angle = j * angleIncrement;
			float x = pentXPos + radius * cos(angle);
			float y = pentYPos + radius * sin(angle);
			newPentagon.positions.push_back(x);
			newPentagon.positions.push_back(y);
			newPentagon.positions.push_back(0.0f);
		}
		newPentagon.x = pentXPos;
		newPentagon.y = pentYPos;
		r = randomColor(dre);
		g = randomColor(dre);
		b = randomColor(dre);
		newPentagon.colors = { r, g, b, r, g, b, r, g, b, r, g, b, r, g, b };
		newPentagon.vertexCount = 5;
		InitShapeData(newPentagon);
		pentagonData.push_back(newPentagon);

		float hexXPos = randomPosition(dre);
		float hexYPos = randomPosition(dre);
		const int hexSides = 6;
		const float hexAngleIncrement = 2.0f * 3.1415926f / hexSides;

		newHexagon.positions.clear();
		for (int j = 0; j < hexSides; ++j) {
			float angle = j * hexAngleIncrement;
			float x = hexXPos + radius * cos(angle);
			float y = hexYPos + radius * sin(angle);
			newHexagon.positions.push_back(x);  
			newHexagon.positions.push_back(y);
			newHexagon.positions.push_back(0.0f);
		}
		newHexagon.x = hexXPos;
		newHexagon.y = hexYPos;
		r = randomColor(dre);
		g = randomColor(dre);
		b = randomColor(dre);
		newHexagon.colors = { r, g, b, r, g, b, r, g, b, r, g, b, r, g, b, r, g, b };
		newHexagon.vertexCount = 6;
		InitShapeData(newHexagon);
		hexagonData.push_back(newHexagon);
	}

	glutPostRedisplay();
}

GLvoid Timer(int value) {
	for (auto& line : lineData) {
		if (line.velocityX != 0.0f || line.velocityY != 0.0f) {
			line.x += line.velocityX;
			line.y += line.velocityY;

			if (line.x >= 1.0f) {
				line.x = 1.0f;
				line.velocityX = -line.velocityX;
			}
			else if (line.x <= -1.0f) {
				line.x = -1.0f;
				line.velocityX = -line.velocityX;
			}

			if (line.y >= 1.0f) {
				line.y = 1.0f;
				line.velocityY = -line.velocityY;
			}
			else if (line.y <= -1.0f) {
				line.y = -1.0f;
				line.velocityY = -line.velocityY;
			}

			GLfloat deltaX = line.velocityX;
			GLfloat deltaY = line.velocityY;

			for (size_t i = 0; i < line.positions.size(); i += 3) {
				line.positions[i] += deltaX;
				line.positions[i + 1] += deltaY;
			}

			glBindBuffer(GL_ARRAY_BUFFER, line.vbo[0]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, line.positions.size() * sizeof(GLfloat), line.positions.data());
		}
	}

	for (auto& triangle : triangleData) {
		if (triangle.velocityX != 0.0f || triangle.velocityY != 0.0f) {
			triangle.x += triangle.velocityX;
			triangle.y += triangle.velocityY;

			if (triangle.x >= 1.0f) {
				triangle.x = 1.0f;
				triangle.velocityX = -triangle.velocityX;
			}
			else if (triangle.x <= -1.0f) {
				triangle.x = -1.0f;
				triangle.velocityX = -triangle.velocityX;
			}

			if (triangle.y >= 1.0f) {
				triangle.y = 1.0f;
				triangle.velocityY = -triangle.velocityY;
			}
			else if (triangle.y <= -1.0f) {
				triangle.y = -1.0f;
				triangle.velocityY = -triangle.velocityY;
			}

			GLfloat deltaX = triangle.velocityX;
			GLfloat deltaY = triangle.velocityY;

			for (size_t i = 0; i < triangle.positions.size(); i += 3) {
				triangle.positions[i] += deltaX;
				triangle.positions[i + 1] += deltaY;
			}

			glBindBuffer(GL_ARRAY_BUFFER, triangle.vbo[0]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, triangle.positions.size() * sizeof(GLfloat), triangle.positions.data());
		}
	}

	for (auto& rectangle : rectangleData) {
		if (rectangle.velocityX != 0.0f || rectangle.velocityY != 0.0f) {
			rectangle.x += rectangle.velocityX;
			rectangle.y += rectangle.velocityY;

			if (rectangle.x >= 1.0f) {
				rectangle.x = 1.0f;
				rectangle.velocityX = -rectangle.velocityX;
			}
			else if (rectangle.x <= -1.0f) {
				rectangle.x = -1.0f;
				rectangle.velocityX = -rectangle.velocityX;
			}

			if (rectangle.y >= 1.0f) {
				rectangle.y = 1.0f;
				rectangle.velocityY = -rectangle.velocityY;
			}
			else if (rectangle.y <= -1.0f) {
				rectangle.y = -1.0f;
				rectangle.velocityY = -rectangle.velocityY;
			}

			GLfloat deltaX = rectangle.velocityX;
			GLfloat deltaY = rectangle.velocityY;

			for (size_t i = 0; i < rectangle.positions.size(); i += 3) {
				rectangle.positions[i] += deltaX;
				rectangle.positions[i + 1] += deltaY;
			}

			glBindBuffer(GL_ARRAY_BUFFER, rectangle.vbo[0]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, rectangle.positions.size() * sizeof(GLfloat), rectangle.positions.data());
		}
	}

	for (auto& pentagon : pentagonData) {
		if (pentagon.velocityX != 0.0f || pentagon.velocityY != 0.0f) {
			pentagon.x += pentagon.velocityX;
			pentagon.y += pentagon.velocityY;

			if (pentagon.x >= 1.0f) {
				pentagon.x = 1.0f;
				pentagon.velocityX = -pentagon.velocityX;
			}
			else if (pentagon.x <= -1.0f) {
				pentagon.x = -1.0f;
				pentagon.velocityX = -pentagon.velocityX;
			}

			if (pentagon.y >= 1.0f) {
				pentagon.y = 1.0f;
				pentagon.velocityY = -pentagon.velocityY;
			}
			else if (pentagon.y <= -1.0f) {
				pentagon.y = -1.0f;
				pentagon.velocityY = -pentagon.velocityY;
			}

			GLfloat deltaX = pentagon.velocityX;
			GLfloat deltaY = pentagon.velocityY;

			for (size_t i = 0; i < pentagon.positions.size(); i += 3) {
				pentagon.positions[i] += deltaX;
				pentagon.positions[i + 1] += deltaY;
			}

			glBindBuffer(GL_ARRAY_BUFFER, pentagon.vbo[0]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, pentagon.positions.size() * sizeof(GLfloat), pentagon.positions.data());
		}
	}

	for (auto& hexagon : hexagonData) {
		if (hexagon.velocityX != 0.0f || hexagon.velocityY != 0.0f) {
			hexagon.x += hexagon.velocityX;
			hexagon.y += hexagon.velocityY;

			if (hexagon.x >= 1.0f) {
				hexagon.x = 1.0f;
				hexagon.velocityX = -hexagon.velocityX;
			}
			else if (hexagon.x <= -1.0f) {
				hexagon.x = -1.0f;
				hexagon.velocityX = -hexagon.velocityX;
			}

			if (hexagon.y >= 1.0f) {
				hexagon.y = 1.0f;
				hexagon.velocityY = -hexagon.velocityY;
			}
			else if (hexagon.y <= -1.0f) {
				hexagon.y = -1.0f;
				hexagon.velocityY = -hexagon.velocityY;
			}

			GLfloat deltaX = hexagon.velocityX;
			GLfloat deltaY = hexagon.velocityY;

			for (size_t i = 0; i < hexagon.positions.size(); i += 3) {
				hexagon.positions[i] += deltaX;
				hexagon.positions[i + 1] += deltaY;
			}

			glBindBuffer(GL_ARRAY_BUFFER, hexagon.vbo[0]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, hexagon.positions.size() * sizeof(GLfloat), hexagon.positions.data());
		}
	}

	glutPostRedisplay();

	glutTimerFunc(16, Timer, 0);
}

GLvoid drawScene() {
	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	for (const auto& point : pointData) {
		glBindVertexArray(point.vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	for (const auto& line : lineData) {
		glBindVertexArray(line.vao);
		glDrawArrays(GL_LINES, 0, 2);
	}

	for (const auto& triangle : triangleData) {
		glBindVertexArray(triangle.vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	for (const auto& rectangle : rectangleData) {
		glBindVertexArray(rectangle.vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	for (const auto& pentagon : pentagonData) {
		glBindVertexArray(pentagon.vao);
		glDrawArrays(GL_POLYGON, 0, 5);
	}

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);
}

bool isMouseOverShape(const ShapeData& shape, GLfloat mouseX, GLfloat mouseY) {
	if (shape.positions.size() < 6) return false;

	GLfloat minX = shape.positions[0];
	GLfloat minY = shape.positions[1];
	GLfloat maxX = shape.positions[0];
	GLfloat maxY = shape.positions[1];

	for (size_t i = 0; i < shape.positions.size(); i += 3) {
		if (i + 1 >= shape.positions.size()) break;
		GLfloat x = shape.positions[i];
		GLfloat y = shape.positions[i + 1];

		if (x < minX) minX = x;
		if (y < minY) minY = y;
		if (x > maxX) maxX = x;
		if (y > maxY) maxY = y;
	}

	return (mouseX >= minX && mouseX <= maxX && mouseY >= minY && mouseY <= maxY);
}

bool isShapeOverlapping(const ShapeData& shape1, const ShapeData& shape2) {
	GLfloat minX1 = shape1.positions[0], maxX1 = shape1.positions[0];
	GLfloat minY1 = shape1.positions[1], maxY1 = shape1.positions[1];
	for (size_t i = 0; i < shape1.positions.size(); i += 3) {
		GLfloat x = shape1.positions[i];
		GLfloat y = shape1.positions[i + 1];
		if (x < minX1) minX1 = x;
		if (x > maxX1) maxX1 = x;
		if (y < minY1) minY1 = y;
		if (y > maxY1) maxY1 = y;
	}

	GLfloat minX2 = shape2.positions[0], maxX2 = shape2.positions[0];
	GLfloat minY2 = shape2.positions[1], maxY2 = shape2.positions[1];
	for (size_t i = 0; i < shape2.positions.size(); i += 3) {
		GLfloat x = shape2.positions[i];
		GLfloat y = shape2.positions[i + 1];
		if (x < minX2) minX2 = x;
		if (x > maxX2) maxX2 = x;
		if (y < minY2) minY2 = y;
		if (y > maxY2) maxY2 = y;
	}

	bool overlapX = (minX1 <= maxX2 && maxX1 >= minX2);
	bool overlapY = (minY1 <= maxY2 && maxY1 >= minY2);

	return overlapX && overlapY;
}

bool isPointNearLine(const ShapeData& point, const ShapeData& line) {
	GLfloat px = point.positions[0];
	GLfloat py = point.positions[1];

	GLfloat x1 = line.positions[0];
	GLfloat y1 = line.positions[1];
	GLfloat x2 = line.positions[3];
	GLfloat y2 = line.positions[4];

	GLfloat dx = x2 - x1;
	GLfloat dy = y2 - y1;
	GLfloat lengthSquared = dx * dx + dy * dy;

	if (lengthSquared == 0.0f) {
		return ((px - x1) * (px - x1) + (py - y1) * (py - y1)) < 0.01f;
	}

	GLfloat t = ((px - x1) * dx + (py - y1) * dy) / lengthSquared;
	t = fmax(0.0f, fmin(1.0f, t));

	GLfloat projX = x1 + t * dx;
	GLfloat projY = y1 + t * dy;

	GLfloat distanceSquared = (px - projX) * (px - projX) + (py - projY) * (py - projY);

	return distanceSquared < 0.01f;
}

bool checkAndHandlePointCollision() {
	for (size_t i = 0; i < pointData.size(); ++i) {
		for (size_t j = i + 1; j < pointData.size(); ++j) {
			if (isShapeOverlapping(pointData[i], pointData[j])) {
				ShapeData newLine;
				newLine.positions = {
					pointData[i].positions[0] - 0.15f, pointData[i].positions[1] - 0.15f, 0.0f,
					pointData[j].positions[0] + 0.15f, pointData[j].positions[1] + 0.15f, 0.0f
				};

				float r = randomColor(dre);
				float g = randomColor(dre);
				float b = randomColor(dre);
				newLine.colors = { r, g, b, r, g, b };
				newLine.vertexCount = 2;

				newLine.x = (pointData[i].positions[0] + pointData[j].positions[0]) / 2.0f;
				newLine.y = (pointData[i].positions[1] + pointData[j].positions[1]) / 2.0f;

				newLine.velocityX = 0.02f;
				newLine.velocityY = 0.02f;

				lineData.push_back(newLine);
				InitShapeData(lineData.back());

				glDeleteVertexArrays(1, &pointData[j].vao);
				glDeleteBuffers(2, pointData[j].vbo);
				pointData.erase(pointData.begin() + j);

				glDeleteVertexArrays(1, &pointData[i].vao);
				glDeleteBuffers(2, pointData[i].vbo);
				pointData.erase(pointData.begin() + i);

				glutPostRedisplay();
				return true;
			}
		}

		for (size_t j = 0; j < lineData.size(); ++j) {
			if (isPointNearLine(pointData[i], lineData[j])) {
				ShapeData newTriangle;
				newTriangle.positions = {
					pointData[i].positions[0] - 0.15f, pointData[i].positions[1] - 0.15f, 0.0f,
					pointData[i].positions[0] + 0.15f, pointData[i].positions[1] - 0.15f, 0.0f,
					pointData[i].positions[0], pointData[i].positions[1] + 0.15f, 0.0f
				};

				float r = randomColor(dre);
				float g = randomColor(dre);
				float b = randomColor(dre);
				newTriangle.colors = { r, g, b, r, g, b, r, g, b };
				newTriangle.vertexCount = 3;

				newTriangle.x = pointData[i].positions[0];
				newTriangle.y = pointData[i].positions[1];

				newTriangle.velocityX = 0.02f;
				newTriangle.velocityY = 0.02f;

				triangleData.push_back(newTriangle);
				InitShapeData(triangleData.back());

				glDeleteVertexArrays(1, &pointData[i].vao);
				glDeleteBuffers(2, pointData[i].vbo);
				pointData.erase(pointData.begin() + i);

				glDeleteVertexArrays(1, &lineData[j].vao);
				glDeleteBuffers(2, lineData[j].vbo);
				lineData.erase(lineData.begin() + j);

				glutPostRedisplay();
				return true;
			}
		}
		for (size_t j = 0; j < triangleData.size(); ++j) {
			if (isPointNearLine(pointData[i], triangleData[j])) {
				ShapeData newRectangle;
				newRectangle.positions = {
					pointData[i].positions[0], pointData[i].positions[1], 0.0f,
					pointData[i].positions[0] + 0.3f, pointData[i].positions[1], 0.0f,
					pointData[i].positions[0], pointData[i].positions[1] + 0.3f, 0.0f,
					pointData[i].positions[0] + 0.3f, pointData[i].positions[1] + 0.3f, 0.0f
				};

				newRectangle.x = pointData[i].positions[0];
				newRectangle.y = pointData[i].positions[1];

				newRectangle.velocityX = 0.02f;
				newRectangle.velocityY = 0.02f;

				float r = randomColor(dre);
				float g = randomColor(dre);
				float b = randomColor(dre);
				newRectangle.colors = { r, g, b, r, g, b, r, g, b, r, g, b };
				newRectangle.vertexCount = 4;

				rectangleData.push_back(newRectangle);
				InitShapeData(rectangleData.back());

				glDeleteVertexArrays(1, &pointData[i].vao);
				glDeleteBuffers(2, pointData[i].vbo);
				pointData.erase(pointData.begin() + i);

				glDeleteVertexArrays(1, &triangleData[j].vao);
				glDeleteBuffers(2, triangleData[j].vbo);
				triangleData.erase(triangleData.begin() + j);

				glutPostRedisplay();
				return true;
			}
		}

		for (size_t j = 0; j < rectangleData.size(); ++j) {
			if (isShapeOverlapping(pointData[i], rectangleData[j])) {
				ShapeData newPentagon;
				const float radius = 0.15f;
				const int numSides = 5;
				const float angleIncrement = 2.0f * 3.1415926f / numSides;

				float centerX = (pointData[i].positions[0] + rectangleData[j].x) / 2.0f;
				float centerY = (pointData[i].positions[1] + rectangleData[j].y) / 2.0f;

				newPentagon.positions.clear();
				for (int k = 0; k < numSides; ++k) {
					float angle = k * angleIncrement;
					float x = centerX + radius * cos(angle);
					float y = centerY + radius * sin(angle);
					newPentagon.positions.push_back(x);
					newPentagon.positions.push_back(y);
					newPentagon.positions.push_back(0.0f);
				}

				float r = randomColor(dre);
				float g = randomColor(dre);
				float b = randomColor(dre);
				for (int k = 0; k < numSides; ++k) {
					newPentagon.colors.push_back(r);
					newPentagon.colors.push_back(g);
					newPentagon.colors.push_back(b);
				}

				newPentagon.vertexCount = numSides;
				newPentagon.x = centerX;
				newPentagon.y = centerY;
				newPentagon.velocityX = 0.02f;
				newPentagon.velocityY = 0.02f;

				pentagonData.push_back(newPentagon);
				InitShapeData(pentagonData.back());

				glDeleteVertexArrays(1, &pointData[i].vao);
				glDeleteBuffers(2, pointData[i].vbo);
				pointData.erase(pointData.begin() + i);

				glDeleteVertexArrays(1, &rectangleData[j].vao);
				glDeleteBuffers(2, rectangleData[j].vbo);
				rectangleData.erase(rectangleData.begin() + j);

				glutPostRedisplay();
				return true;
			}
		}

		for (size_t j = 0; j < pentagonData.size(); ++j) {
			if (isPointNearLine(pointData[i], pentagonData[j])) {
				ShapeData newHexagon;
				const int numSides = 6;
				const float radius = 0.15f;
				const float angleIncrement = 2.0f * 3.1415926f / numSides;

				float centerX = (pointData[i].positions[0] + pentagonData[j].x) / 2.0f;
				float centerY = (pointData[i].positions[1] + pentagonData[j].y) / 2.0f;

				newHexagon.positions.clear();
				for (int k = 0; k < numSides; ++k) {
					float angle = k * angleIncrement;
					float x = centerX + radius * cos(angle);
					float y = centerY + radius * sin(angle);
					newHexagon.positions.push_back(x);
					newHexagon.positions.push_back(y);
					newHexagon.positions.push_back(0.0f);
				}

				float r = randomColor(dre);
				float g = randomColor(dre);
				float b = randomColor(dre);
				for (int k = 0; k < numSides; ++k) {
					newHexagon.colors.push_back(r);
					newHexagon.colors.push_back(g);
					newHexagon.colors.push_back(b);
				}

				newHexagon.vertexCount = numSides;
				newHexagon.x = centerX;
				newHexagon.y = centerY;
				newHexagon.velocityX = 0.02f; 
				newHexagon.velocityY = 0.02f; 

				hexagonData.push_back(newHexagon);
				InitShapeData(hexagonData.back());

				glDeleteVertexArrays(1, &pointData[i].vao);
				glDeleteBuffers(2, pointData[i].vbo);
				pointData.erase(pointData.begin() + i);

				glDeleteVertexArrays(1, &pentagonData[j].vao);
				glDeleteBuffers(2, pentagonData[j].vbo);
				pentagonData.erase(pentagonData.begin() + j);

				glutPostRedisplay();
				return true;
			}
		}
	}
	return false;
}

GLvoid Mouse(int button, int state, int x, int y) {
	GLfloat mouseX = (2.0f * x / window_Width) - 1.0f;
	GLfloat mouseY = 1.0f - (2.0f * y / window_Height);

	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			for (size_t i = 0; i < pointData.size(); ++i) {
				if (isMouseOverShape(pointData[i], mouseX, mouseY)) {
					isDragging = true;
					draggedShape = &pointData[i];
					draggedShape->dragOffsetX = mouseX - draggedShape->x;
					draggedShape->dragOffsetY = mouseY - draggedShape->y;
					return;
				}
			}

			for (size_t i = 0; i < lineData.size(); ++i) {
				if (isMouseOverShape(lineData[i], mouseX, mouseY)) {
					isDragging = true;
					draggedShape = &lineData[i];
					draggedShape->dragOffsetX = mouseX - draggedShape->x;
					draggedShape->dragOffsetY = mouseY - draggedShape->y;
					return;
				}
			}

			for (size_t i = 0; i < triangleData.size(); ++i) {
				if (isMouseOverShape(triangleData[i], mouseX, mouseY)) {
					isDragging = true;
					draggedShape = &triangleData[i];
					draggedShape->dragOffsetX = mouseX - draggedShape->x;
					draggedShape->dragOffsetY = mouseY - draggedShape->y;
					return;
				}
			}

			for (size_t i = 0; i < rectangleData.size(); ++i) {
				if (isMouseOverShape(rectangleData[i], mouseX, mouseY)) {
					isDragging = true;
					draggedShape = &rectangleData[i];
					draggedShape->dragOffsetX = mouseX - draggedShape->x;
					draggedShape->dragOffsetY = mouseY - draggedShape->y;
					return;
				}
			}

			for (size_t i = 0; i < pentagonData.size(); ++i) {
				if (isMouseOverShape(pentagonData[i], mouseX, mouseY)) {
					isDragging = true;
					draggedShape = &pentagonData[i];
					draggedShape->dragOffsetX = mouseX - draggedShape->x;
					draggedShape->dragOffsetY = mouseY - draggedShape->y;
					return;
				}
			}

			for (size_t i = 0; i < hexagonData.size(); ++i) {
				if (isMouseOverShape(hexagonData[i], mouseX, mouseY)) {
					isDragging = true;
					draggedShape = &hexagonData[i];
					draggedShape->dragOffsetX = mouseX - draggedShape->x;
					draggedShape->dragOffsetY = mouseY - draggedShape->y;
					return;
				}
			}
		}
	}
	if (state == GLUT_UP) {
		if (checkAndHandlePointCollision()) {
			glutPostRedisplay();
		}
	}
}

GLvoid Motion(int x, int y) {
	if (isDragging && draggedShape) {
		GLfloat mouseX = (2.0f * x / window_Width) - 1.0f;
		GLfloat mouseY = 1.0f - (2.0f * y / window_Height);

		GLfloat newX = mouseX - draggedShape->dragOffsetX;
		GLfloat newY = mouseY - draggedShape->dragOffsetY;

		GLfloat deltaX = newX - draggedShape->x;
		GLfloat deltaY = newY - draggedShape->y;

		draggedShape->x = newX;
		draggedShape->y = newY;

		for (size_t i = 0; i < draggedShape->positions.size(); i += 3) {
			draggedShape->positions[i] += deltaX;
			draggedShape->positions[i + 1] += deltaY;
		}

		glBindBuffer(GL_ARRAY_BUFFER, draggedShape->vbo[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, draggedShape->positions.size() * sizeof(GLfloat), draggedShape->positions.data());

		glutPostRedisplay();
	}
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
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