#include <iostream>
#include <random>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>

using namespace std;

struct randRect {
    float x, y;
    float initX, initY;
    float width = 0.25f, height = 0.25f;
    float dx = 0.01f, dy = 0.01f;
    GLclampf r, g, b;
};

vector<randRect> rects;

random_device rd;
default_random_engine dre(rd());
uniform_real_distribution<float> randomPosition(-1.0f, 1.0f);
uniform_real_distribution<float> randomColor(0.0f, 1.0f);
uniform_real_distribution<float> randomSize(-0.05f, 0.05f);

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Timer(int value);

int window_Width = 800;
int window_Height = 600;

float back_r = 0.7f, back_g = 0.7f, back_b = 0.7f;

const int maxRects = 5;
bool isMovingDiagonal = false;
bool isMovingZigzag = false;
bool isChangeSize = false;
bool isChangeColor = false;
bool moveBackToInitPos = false;

void main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(window_Width, window_Height);
    glutCreateWindow("Example4");

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
    glutMouseFunc(Mouse);
    glutReshapeFunc(Reshape);
    glutTimerFunc(16, Timer, 0);
    glutMainLoop();
}

GLvoid drawScene() {
    glClearColor(back_r, back_g, back_b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (const auto& rect : rects) {
        glColor3f(rect.r, rect.g, rect.b);
        glRectf(rect.x, rect.y, rect.x + rect.width, rect.y + rect.height);
    }
    glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

GLvoid Mouse(int button, int state, int x, int y) {
    GLfloat x_opengl = (2.0f * x / window_Width) - 1.0f;
    GLfloat y_opengl = 1.0f - (2.0f * y / window_Height);

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (rects.size() < maxRects) {
            randRect newRect;
            newRect.x = x_opengl - newRect.width / 2.0f;
            newRect.y = y_opengl - newRect.height / 2.0f;
            newRect.initX = newRect.x;
            newRect.initY = newRect.y;
            newRect.r = randomColor(dre);
            newRect.g = randomColor(dre);
            newRect.b = randomColor(dre);
            rects.push_back(newRect);
        }
        glutPostRedisplay();
    }
}

GLvoid Keyboard(unsigned char key, int x, int y) {
    randRect newRect;
    switch (key) {
    case 'a':
        isMovingDiagonal = !isMovingDiagonal;
        break;
    case 'b':
        isMovingZigzag = !isMovingZigzag;
        break;
    case 'c':
        isChangeSize = !isChangeSize;
        break;
    case 'd':
        isChangeColor = !isChangeColor;
        break;
    case 's':
        if (isMovingDiagonal == true) {
            isMovingDiagonal = false;
        }
        if (isMovingZigzag == true) {
            isMovingZigzag = false;
        }
        if (isChangeSize == true) {
            isChangeSize = false;
        }
        if (isChangeColor == true) {
            isChangeColor = false;
        }
        break;
    case 'm':
        moveBackToInitPos = true;
        break;
    case 'r':
        rects.clear();
        glutPostRedisplay();
        break;
    case 'q':
        glutLeaveMainLoop();
        break;
    }
}

GLvoid Timer(int value) {
    if (isMovingDiagonal) {
        for (auto& rect : rects) {
            rect.x += rect.dx;
            rect.y += rect.dy;

            if (rect.x <= -1.0f || rect.x + rect.width >= 1.0f) {
                rect.dx = -rect.dx;
            }
            if (rect.y <= -1.0f || rect.y + rect.height >= 1.0f) {
                rect.dy = -rect.dy;
            }
        }
    }

    if (isMovingZigzag) {
        static int zigzagCounter = 0;
        zigzagCounter++;

        if (zigzagCounter % 30 == 0) {
            for (auto& rect : rects) {
                rect.dx = -rect.dx;
            }
        }
        for (auto& rect : rects) {
            rect.x += rect.dx;
            rect.y += rect.dy;

            if (rect.x <= -1.0f || rect.x + rect.width >= 1.0f) {
                rect.dx = -rect.dx;
            }
            if (rect.y <= -1.0f || rect.y + rect.height >= 1.0f) {
                rect.dy = -rect.dy;
            }
        }
    }

    if (isChangeSize) {
        for (auto& rect : rects) {
            rect.width += randomSize(dre);
            rect.height += randomSize(dre);

            if (rect.width > 0.5f) rect.width = 0.5f;
            if (rect.height > 0.5f) rect.height = 0.5f;
            if (rect.width < 0.05f) rect.width = 0.05f;
            if (rect.height < 0.05f) rect.height = 0.05f;
        }
    }

    if (isChangeColor) {
        for (auto& rect : rects) {
            rect.r = randomColor(dre);
            rect.g = randomColor(dre);
            rect.b = randomColor(dre);
        }
    }

    if (moveBackToInitPos) {
        for (auto& rect : rects) {
            rect.x = rect.initX;
            rect.y = rect.initY;
        }
        moveBackToInitPos = false;
    }
    glutPostRedisplay();
    glutTimerFunc(50, Timer, 0);
}