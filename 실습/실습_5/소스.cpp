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
    float width = 0.1f, height = 0.1f;
    GLclampf r, g, b;
};

vector<randRect> rects;

random_device rd;
default_random_engine dre(rd());
uniform_real_distribution<float> randomPosition(-1.0f, 1.0f);
uniform_real_distribution<float> randomColor(0.0f, 1.0f);
uniform_int_distribution<int> randomCount(20, 40);

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Motion(int x, int y);
GLvoid eraseRectangles();

int window_Width = 800;
int window_Height = 600;

float back_r = 1.0f, back_g = 1.0f, back_b = 1.0f;

bool isErasing = false;
bool isInitialized = false;
randRect eraser;

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
    glutMotionFunc(Motion);
    glutReshapeFunc(Reshape);
    glutMainLoop();
}

GLvoid drawScene() {
    glClearColor(back_r, back_g, back_b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (!isInitialized) {
        int numRects = randomCount(dre);
        for (int i = 0; i < numRects; ++i) {
            randRect newRects;
            newRects.x = randomPosition(dre);
            newRects.y = randomPosition(dre);
            newRects.r = randomColor(dre);
            newRects.g = randomColor(dre);
            newRects.b = randomColor(dre);

            rects.push_back(newRects);
        }
        isInitialized = true;
    }

    for (const auto& rect : rects) {
        glColor3f(rect.r, rect.g, rect.b);
        glRectf(rect.x, rect.y, rect.x + rect.width, rect.y + rect.height);
    }

    if (isErasing) {
        glColor3f(eraser.r, eraser.g, eraser.b);
        glRectf(eraser.x, eraser.y, eraser.x + eraser.width, eraser.y + eraser.height);
    }

    eraseRectangles();
    glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

GLvoid Mouse(int button, int state, int x, int y) {
    GLfloat x_opengl = (2.0f * x / window_Width) - 1.0f;
    GLfloat y_opengl = 1.0f - (2.0f * y / window_Height);

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        isErasing = true;

        eraser.x = x_opengl - eraser.width / 2.0f;
        eraser.y = y_opengl - eraser.height / 2.0f;

        eraser.width = 0.2f;
        eraser.height = 0.2f;
        eraser.r = 0.0f;
        eraser.g = 0.0f;
        eraser.b = 0.0f;

        glutPostRedisplay();
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        isErasing = false;

        glutPostRedisplay();
    }
}

GLvoid Motion(int x, int y) {
    GLfloat x_opengl = (2.0f * x / window_Width) - 1.0f;
    GLfloat y_opengl = 1.0f - (2.0f * y / window_Height);

    if (isErasing) {
        eraser.x = x_opengl - eraser.width / 2.0f;
        eraser.y = y_opengl - eraser.height / 2.0f;

        glutPostRedisplay();
    }
}

GLvoid Keyboard(unsigned char key, int x, int y) {
    randRect newRect;
    switch (key) {
    case 'q':
        glutLeaveMainLoop();
        break;
    case 'r':
        rects.clear();

        int numRects = randomCount(dre);
        for (int i = 0; i < numRects; ++i) {
            randRect newRect;
            newRect.x = randomPosition(dre);
            newRect.y = randomPosition(dre);
            newRect.r = randomColor(dre);
            newRect.g = randomColor(dre);
            newRect.b = randomColor(dre);

            rects.push_back(newRect);
        }

        glutPostRedisplay();
        break;
    }
}

GLvoid eraseRectangles() {
    if (!isErasing) return;

    vector<randRect> newRects;

    for (int i = 0; i < rects.size(); ++i) {
        if (eraser.x < rects[i].x + rects[i].width &&
            eraser.x + eraser.width > rects[i].x &&
            eraser.y < rects[i].y + rects[i].height &&
            eraser.y + eraser.height > rects[i].y) {

            eraser.r = rects[i].r;
            eraser.g = rects[i].g;
            eraser.b = rects[i].b;

            eraser.width += rects[i].width / 2;
            eraser.height += rects[i].height / 2;

        }
        else {
            newRects.push_back(rects[i]);
        }
    }

    rects = newRects;
}