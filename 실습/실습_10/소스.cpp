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
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>

using namespace std;

GLuint vao, vbo;
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

int window_Width = 800;
int window_Height = 600;
bool isPointMode = false;
int numSpirals = 1;

struct Spiral {
    GLfloat x, y;
    GLfloat theta;
    GLfloat radius;
    GLfloat radiusChange;
    bool isGrowing;
    bool isPointMode;
};

vector<Spiral> spirals;

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

void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Timer(int value);
void InitBuffer();
void AddSpirals(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
void ChangeBackgroundColor();

void main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(window_Width, window_Height);
    glutCreateWindow("Example10");

    glewExperimental = GL_TRUE;
    glewInit();

    make_shaderProgram();
    InitBuffer();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(Mouse);
    glutTimerFunc(16, Timer, 0);

    glutMainLoop();
}

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
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 1000, NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (GLvoid*)0);
    glEnableVertexAttribArray(0);
}

void AddSpirals(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) {
    Spiral growingSpiral, shrinkingSpiral;

    growingSpiral.x = x1;
    growingSpiral.y = y1;
    growingSpiral.theta = 0.0f;
    growingSpiral.radius = 0.0f;
    growingSpiral.radiusChange = 0.01f;
    growingSpiral.isGrowing = true;
    growingSpiral.isPointMode = isPointMode;

    shrinkingSpiral.x = x2;
    shrinkingSpiral.y = y2;
    shrinkingSpiral.theta = 0.0f;
    shrinkingSpiral.radius = 0.3f;
    shrinkingSpiral.radiusChange = -0.01f;
    shrinkingSpiral.isGrowing = false;
    shrinkingSpiral.isPointMode = isPointMode;

    spirals.push_back(growingSpiral);
    spirals.push_back(shrinkingSpiral);
}

void AddRandomSpirals() {
    for (int i = 0; i < numSpirals; ++i) {
        GLfloat x1 = static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f;
        GLfloat y1 = static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f;
        

        AddSpirals(x1, y1, x1 + 0.314, y1);
    }
}

void ChangeBackgroundColor() {
    GLfloat r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    GLfloat g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    GLfloat b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    glClearColor(r, g, b, 1.0f);
}

GLvoid Timer(int value) {
    for (auto& spiral : spirals) {
        if (spiral.theta < 15.8f) {
            spiral.theta += 0.1f;
            spiral.radius += spiral.radiusChange;
        }

        if (spiral.isGrowing && spiral.radius >= 0.3f) {
            spiral.radiusChange = -spiral.radiusChange;
        }
        if (!spiral.isGrowing && spiral.radius <= 0.0f) {
            spiral.radiusChange = -spiral.radiusChange;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, Timer, 0);
}

GLvoid drawScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgramID);

    for (const auto& spiral : spirals) {
        vector<glm::vec2> points;
        GLfloat currentRadius = 0.0f;

        for (float theta = 0.0f; theta <= spiral.theta; theta += 0.1f) {
            currentRadius = spiral.radiusChange * theta;
            GLfloat x = spiral.x + currentRadius * cos(theta);
            GLfloat y = spiral.y + currentRadius * sin(theta);
            points.push_back(glm::vec2(x, y));
        }

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(glm::vec2), &points[0]);

        if (spiral.isPointMode) {
            glDrawArrays(GL_POINTS, 0, points.size());
        }
        else {
            glDrawArrays(GL_LINE_STRIP, 0, points.size());
        }
    }

    glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

GLvoid Mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        GLfloat x_opengl = (2.0f * x / window_Width) - 1.0f;
        GLfloat y_opengl = 1.0f - (2.0f * y / window_Height);

        AddSpirals(x_opengl - 0.157f, y_opengl, x_opengl + 0.157f, y_opengl);
        ChangeBackgroundColor();
    }

    glutPostRedisplay();
}

GLvoid Keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'p':
        isPointMode = true;
        glutPostRedisplay();
        break;
    case 'l':
        isPointMode = false;
        glutPostRedisplay();
        break;
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
        numSpirals = key - '0';
        AddRandomSpirals();
        ChangeBackgroundColor();
        break;
    case 'q':
        glutLeaveMainLoop();
        break;
    }
}