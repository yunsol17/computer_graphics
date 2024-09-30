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
    float width = 0.25f, height = 0.25f;
    GLclampf r, g, b;
};

vector<randRect> rects;

random_device rd;
default_random_engine dre(rd());
uniform_real_distribution<float> randomPosition(-1.0f, 1.0f);
uniform_real_distribution<float> randomColor(0.0f, 1.0f);

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Motion(int x, int y);

int window_Width = 800;
int window_Height = 600;

float back_r = 1.0f, back_g = 1.0f, back_b = 1.0f;

const int maxRects = 10;
bool drag = false;
int selectRectIndex = -1;

void main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(window_Width, window_Height);
    glutCreateWindow("Example3");

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

    for (const auto& rect : rects) {
        glColor3f(rect.r, rect.g, rect.b);
        glRectf(rect.x, rect.y, rect.x + rect.width, rect.y + rect.height);
    }
    glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

GLvoid Motion(int x, int y) {
    if (drag && selectRectIndex != -1) {
        GLfloat x_opengl = (2.0f * x / window_Width) - 1.0f;
        GLfloat y_opengl = 1.0f - (2.0f * y / window_Height);

        randRect& movingRect = rects[selectRectIndex];
        movingRect.x = x_opengl;
        movingRect.y = y_opengl;

        glutPostRedisplay();
    }
}

GLvoid Mouse(int button, int state, int x, int y) {
    GLfloat x_opengl = (2.0f * x / window_Width) - 1.0f;
    GLfloat y_opengl = 1.0f - (2.0f * y / window_Height);

    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            for (int i = 0; i < rects.size(); i++) {
                if (x_opengl >= rects[i].x && x_opengl <= rects[i].x + rects[i].width &&
                    y_opengl >= rects[i].y && y_opengl <= rects[i].y + rects[i].height) {
                    selectRectIndex = i;
                    drag = true;
                    break;
                }
            }
        }
        else if (state == GLUT_UP) {
            drag = false;

            if (selectRectIndex != -1) {
                randRect& movingRect = rects[selectRectIndex];
                for (int i = 0; i < rects.size(); ++i) {
                    if (i != selectRectIndex) {
                        randRect& targetRect = rects[i];
                        if (movingRect.x < targetRect.x + targetRect.width &&
                            movingRect.x + movingRect.width > targetRect.x &&
                            movingRect.y < targetRect.y + targetRect.height &&
                            movingRect.y + movingRect.height > targetRect.y) {

                            float new_x = min(movingRect.x, targetRect.x);
                            float new_y = min(movingRect.y, targetRect.y);
                            float new_width = max(movingRect.x + movingRect.width, targetRect.x + targetRect.width) - new_x;
                            float new_height = max(movingRect.y + movingRect.height, targetRect.y + targetRect.height) - new_y;

                            randRect newRect;
                            newRect.x = new_x;
                            newRect.y = new_y;
                            newRect.width = new_width;
                            newRect.height = new_height;
                            newRect.r = randomColor(dre);
                            newRect.g = randomColor(dre);
                            newRect.b = randomColor(dre);

                            rects.erase(rects.begin() + i);
                            if (selectRectIndex > i) selectRectIndex--;
                            rects[selectRectIndex] = newRect;

                            break;
                        }
                    }
                }
            }
            selectRectIndex = -1;
        }
    }
}

GLvoid Keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'a':
        if (rects.size() < maxRects) {
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
    case 'q':
        glutLeaveMainLoop();
        break;
    }
}

// 구조체에서 width,height를 0.25f로 고정해버리니까 문제가 발생하는것 같음.keyup을 해야 작동이 되게 해야할것 같음. 사각형의 크기가 이상하게 설정이 됨. 