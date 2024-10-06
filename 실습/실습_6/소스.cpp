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
    float length;
    GLclampf r, g, b;
};

vector<randRect> rects;
vector<randRect> dividedRects;

random_device rd;
default_random_engine dre(rd());
uniform_real_distribution<float> randomPosition(-0.8f, 0.8f);
uniform_real_distribution<float> randomColor(0.0f, 1.0f);
uniform_real_distribution<float> randomSize(0.1f, 0.5f);
uniform_int_distribution<int> randomCount(5, 10);

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Timer(int value);

int window_Width = 800;
int window_Height = 600;

float back_r = 1.0f, back_g = 1.0f, back_b = 1.0f;

bool isInitialized = false;
bool isDivided = false;
bool isMoving1 = false;
bool isMoving2 = false;
bool isMoving3 = false;
bool isMoving4 = false;
bool alreadyDivided = true;
float speed = 0.05f;

void main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(window_Width, window_Height);
    glutCreateWindow("Example6");

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

    if (!isInitialized) {
        int numRects = randomCount(dre);
        for (int i = 0; i < numRects; ++i) {
            randRect newRects;
            newRects.x = randomPosition(dre);
            newRects.y = randomPosition(dre);
            newRects.length = randomSize(dre);
            newRects.r = randomColor(dre);
            newRects.g = randomColor(dre);
            newRects.b = randomColor(dre);

            rects.push_back(newRects);

        }
        isInitialized = true;
    }

    for (const auto& rect : rects) {
        glColor3f(rect.r, rect.g, rect.b);
        glRectf(rect.x, rect.y, rect.x + rect.length, rect.y + rect.length);
    }

    for (const auto& rect : dividedRects) {
        glColor3f(rect.r, rect.g, rect.b);
        glRectf(rect.x, rect.y, rect.x + rect.length, rect.y + rect.length);
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
        for (int i = 0; i < rects.size(); ++i) {
            randRect& rect = rects[i];
            if (x_opengl >= rect.x && x_opengl <= rect.x + rect.length && y_opengl >= rect.y && y_opengl <= rect.y + rect.length) {
                float interval = (rect.length - 0.01f) / 2.0f;
                GLclampf r = rect.r, g = rect.g, b = rect.b;

                randRect newRect1 = { rect.x, rect.y, interval, r, g, b };     // 좌하단
                randRect newRect2 = { rect.x + interval + 0.01f, rect.y, interval, r, g, b };    // 우하단
                randRect newRect3 = { rect.x, rect.y + interval + 0.01f, interval, r, g, b };      // 좌상단
                randRect newRect4 = { rect.x + interval + 0.01f, rect.y + interval + 0.01f, interval, r, g, b };      // 우상단

                rects.erase(rects.begin() + i);
                dividedRects.push_back(newRect1);
                dividedRects.push_back(newRect2);
                dividedRects.push_back(newRect3);
                dividedRects.push_back(newRect4);

                isDivided = true;
                break;
            }
        }
        glutPostRedisplay();
    }
}

GLvoid Keyboard(unsigned char key, int x, int y) {
    randRect newRect;
    switch (key) {
    case 'q':
        glutLeaveMainLoop();
        break;
    case '1':
        isMoving1 = !isMoving1;
        break;
    case '2':
        isMoving2 = !isMoving2;
        break;
    case '3':
        isMoving3 = !isMoving3;
        break;
    case '4':
        isMoving4 = !isMoving4;
        alreadyDivided = false;
        break;
    }
}

GLvoid Timer(int value) {
    if (isMoving1 && isDivided) {
        for (int i = 0; i < dividedRects.size(); ++i) {
            randRect& rect = dividedRects[i];

            if (i % 4 == 0) {
                rect.y -= speed;
            }
            else if (i % 4 == 1) {
                rect.x += speed;
            }
            else if (i % 4 == 2) {
                rect.x -= speed;
            }
            else if (i % 4 == 3) {
                rect.y += speed;
            }

            if (rect.x < -1.0f || rect.x + rect.length > 1.0f) {
                speed = -speed;
            }
            if (rect.y < -1.0f || rect.y + rect.length > 1.0f) {
                speed = -speed;
            }

            rect.length -= 0.01f;

            if (rect.length <= 0.01f) {
                dividedRects.erase(dividedRects.begin() + i);
                --i;
            }
        }
    }

    else if (isMoving2 && isDivided) {
        for (int i = 0; i < dividedRects.size(); ++i) {
            randRect& rect = dividedRects[i];

            if (i % 4 == 0) {
                rect.x -= speed;
                rect.y -= speed;
            }
            else if (i % 4 == 1) {
                rect.x += speed;
                rect.y -= speed;
            }
            else if (i % 4 == 2) {
                rect.x -= speed;
                rect.y += speed;
            }
            else if (i % 4 == 3) {
                rect.x += speed;
                rect.y += speed;
            }

            if (rect.x < -1.0f || rect.x + rect.length > 1.0f) {
                speed = -speed;
            }
            if (rect.y < -1.0f || rect.y + rect.length > 1.0f) {
                speed = -speed;
            }

            rect.length -= 0.01f;

            if (rect.length <= 0.01f) {
                dividedRects.erase(dividedRects.begin() + i);
                --i;
            }
        }
    }

    else if (isMoving3 && isDivided) {
        for (int i = 0; i < dividedRects.size(); ++i) {
            randRect& rect = dividedRects[i];

            if (i % 4 == 0) {
                rect.x -= speed;
            }
            else if (i % 4 == 1) {
                rect.x -= speed;
            }
            else if (i % 4 == 2) {
                rect.x -= speed;
            }
            else if (i % 4 == 3) {
                rect.x -= speed;
            }

            if (rect.x < -1.0f || rect.x + rect.length > 1.0f) {
                speed = -speed;
            }

            rect.length -= 0.01f;

            if (rect.length <= 0.01f) {
                dividedRects.erase(dividedRects.begin() + i);
                --i;
            }
        }
    }

    else if (isMoving4 && isDivided) {
        vector<randRect> newRects;

        for (int i = 0; i < dividedRects.size(); ++i) {
            randRect& rect = dividedRects[i];
            float halfLength = (rect.length - 0.01f) / 2.0f;

            randRect newRect1 = { rect.x, rect.y, halfLength, rect.r, rect.g, rect.b };     // 좌측 사각형
            randRect newRect2 = { rect.x + halfLength + 0.01f, rect.y, halfLength, rect.r, rect.g, rect.b };    // 우측 사각형

            newRects.push_back(newRect1);
            newRects.push_back(newRect2);
        }

        dividedRects = newRects;


        for (int i = 0; i < dividedRects.size(); ++i) {
            randRect& rect = dividedRects[i];

            if (i % 8 == 0) rect.x -= speed;
            else if (i % 8 == 1) rect.x += speed;
            else if (i % 8 == 2) rect.y += speed;
            else if (i % 8 == 3) rect.y -= speed;
            else if (i % 8 == 4) rect.x -= speed, rect.y += speed;
            else if (i % 8 == 5) rect.x += speed, rect.y += speed;
            else if (i % 8 == 6) rect.x -= speed, rect.y -= speed;
            else if (i % 8 == 7) rect.x += speed, rect.y -= speed;

            if (rect.x < -1.0f || rect.x + rect.length > 1.0f) speed = -speed;
            if (rect.y < -1.0f || rect.y + rect.length > 1.0f) speed = -speed;

            rect.length -= 0.01f;
            if (rect.length <= 0.01f) {
                dividedRects.erase(dividedRects.begin() + i);
                --i;
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(50, Timer, 0);
}