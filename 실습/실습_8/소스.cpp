#define _CRT_SECURE_NO_WARNINGS 
#include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include <iostream>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

using namespace std;

GLuint vao;
GLuint vbo[2];

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

struct Triangle {
    GLfloat x, y;
    GLfloat size;
    GLfloat r, g, b;
    bool exists;
};

vector<Triangle> triangles[4];

void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);

GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

int window_Width = 800;
int window_Height = 600;

bool isWireframe = false;

void InitBuffer() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(2, vbo);

    GLfloat lineVertices[] = {
        -1.0f,  0.0f, 0.0f,
         1.0f,  0.0f, 0.0f,
         0.0f, -1.0f, 0.0f,
         0.0f,  1.0f, 0.0f
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);
}

void main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(window_Width, window_Height);
    glutCreateWindow("Example8");

    glewExperimental = GL_TRUE;
    glewInit();

    srand(static_cast<unsigned int>(time(0)));

    make_shaderProgram();
    InitBuffer();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(Mouse);

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

int getQuadrant(GLfloat x, GLfloat y) {
    if (x > 0 && y > 0) return 0;
    if (x < 0 && y > 0) return 1;
    if (x < 0 && y < 0) return 2;
    if (x > 0 && y < 0) return 3;
    return -1;
}

void createRandomTriangle(int quadrant, GLfloat x, GLfloat y, bool isRightClick) {
    if (isRightClick) {
        if (triangles[quadrant].size() < 3) {
            Triangle newTriangle;
            newTriangle.x = x;
            newTriangle.y = y;
            newTriangle.size = (rand() % 40 + 10) / 100.0f;
            newTriangle.r = rand() / (float)RAND_MAX;
            newTriangle.g = rand() / (float)RAND_MAX;
            newTriangle.b = rand() / (float)RAND_MAX;
            newTriangle.exists = true;
            triangles[quadrant].push_back(newTriangle); // 삼각형 추가
        }
    }
    else {
        triangles[quadrant].clear();
        Triangle newTriangle;
        newTriangle.x = x;
        newTriangle.y = y;
        newTriangle.size = (rand() % 40 + 10) / 100.0f;
        newTriangle.r = rand() / (float)RAND_MAX;
        newTriangle.g = rand() / (float)RAND_MAX;
        newTriangle.b = rand() / (float)RAND_MAX;
        newTriangle.exists = true;
        triangles[quadrant].push_back(newTriangle);
    }
}

void drawTriangle(Triangle& triangle) {
    GLfloat halfSize = triangle.size / 2.0f;

    GLfloat vertices[] = {
        triangle.x, triangle.y + triangle.size, 0.0f,  triangle.r, triangle.g, triangle.b,
        triangle.x - halfSize, triangle.y - halfSize, 0.0f, triangle.r, triangle.g, triangle.b,
        triangle.x + halfSize, triangle.y - halfSize, 0.0f, triangle.r, triangle.g, triangle.b
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

GLvoid drawScene() {
    glClearColor(1.0, 1.0, 1.0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgramID);

    if (isWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glDisableVertexAttribArray(1);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glUniform4f(glGetUniformLocation(shaderProgramID, "vColor"), 0.0f, 0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_LINES, 0, 4);

    for (int i = 0; i < 4; i++) {
        for (Triangle& triangle : triangles[i]) {
            if (triangle.exists) {
                drawTriangle(triangle);
            }
        }
    }

    glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

GLvoid Mouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        GLfloat x_opengl = (2.0f * x / window_Width) - 1.0f;
        GLfloat y_opengl = 1.0f - (2.0f * y / window_Height);

        int quadrant = getQuadrant(x_opengl, y_opengl);

        if (quadrant != -1) {
            if (button == GLUT_LEFT_BUTTON) {
                createRandomTriangle(quadrant, x_opengl, y_opengl, false);
            }
            else if (button == GLUT_RIGHT_BUTTON) {
                createRandomTriangle(quadrant, x_opengl, y_opengl, true);
            }
            glutPostRedisplay();
        }
    }
}

GLvoid Keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'a':
        isWireframe = false;
        glutPostRedisplay();
        break;
    case 'b':
        isWireframe = true;
        glutPostRedisplay();
        break;
    case 'q':
        glutLeaveMainLoop();
        break;
    }
}