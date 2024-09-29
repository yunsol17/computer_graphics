#include <iostream>
#include <random>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

using namespace std;

default_random_engine dre;
uniform_real_distribution<float> uid(0.0f, 1.0f);

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Mouse(int button, int state, int x, int y);

int window_Width = 800;
int window_Height = 600;

float r = 0.0f, g = 0.0f, b = 1.0f;
float Rectangle_red1 = 1.0f, Rectangle_green1 = 1.0f, Rectangle_blue1 = 0.0f;
float Rectangle_red2 = 0.0f, Rectangle_green2 = 1.0f, Rectangle_blue2 = 0.0f;
float Rectangle_red3 = 1.0f, Rectangle_green3 = 0.6f, Rectangle_blue3 = 0.0f;
float Rectangle_red4 = 0.5f, Rectangle_green4 = 0.8f, Rectangle_blue4 = 1.0f;

float rect1_size = 1.0f, rect2_size = 1.0f, rect3_size = 1.0f, rect4_size = 1.0f;

const float change_size = 0.1f;
const float min_size = 0.1f;
const float max_size = 1.0f;

void main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(window_Width, window_Height);
    glutCreateWindow("Example2");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Unalble to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }
    else {
        std::cout << "GLEW Initialized\n";
    }

    glutDisplayFunc(drawScene);
    glutMouseFunc(Mouse);
    glutReshapeFunc(Reshape);
    glutMainLoop();
}

GLvoid drawScene() {
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(Rectangle_red1, Rectangle_green1, Rectangle_blue1);
    glRectf(-1.0f, 0.0f, -1.0f + rect1_size, 0.0f + rect1_size);

    glColor3f(Rectangle_red2, Rectangle_green2, Rectangle_blue2);
    glRectf(0.0f, 0.0f, 0.0f + rect2_size, 0.0f + rect2_size);

    glColor3f(Rectangle_red3, Rectangle_green3, Rectangle_blue3);
    glRectf(-1.0f, -1.0f, -1.0f + rect3_size, -1.0f + rect3_size);

    glColor3f(Rectangle_red4, Rectangle_green4, Rectangle_blue4);
    glRectf(0.0f, -1.0f, 0.0f + rect4_size, -1.0f + rect4_size);

    glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}


GLvoid Mouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        GLfloat x_opengl = (2.0f * x / window_Width) - 1.0f;
        GLfloat y_opengl = 1.0f - (2.0f * y / window_Height);

        if (button == GLUT_LEFT_BUTTON) {
            if (x_opengl >= -1.0f && x_opengl <= -1.0f + rect1_size && y_opengl >= 0.0f && y_opengl <= 0.0f + rect1_size) {
                Rectangle_red1 = uid(dre);
                Rectangle_green1 = uid(dre);
                Rectangle_blue1 = uid(dre);
            }
            else if (x_opengl >= 0.0f && x_opengl <= 0.0f + rect2_size && y_opengl >= 0.0f && y_opengl <= 0.0f + rect2_size) {
                Rectangle_red2 = uid(dre);
                Rectangle_green2 = uid(dre);
                Rectangle_blue2 = uid(dre);
            }
            else if (x_opengl >= -1.0f && x_opengl <= -1.0f + rect3_size && y_opengl >= -1.0f && y_opengl <= -1.0f + rect3_size) {
                Rectangle_red3 = uid(dre);
                Rectangle_green3 = uid(dre);
                Rectangle_blue3 = uid(dre);
            }
            else if (x_opengl >= 0.0f && x_opengl <= 0.0f + rect4_size && y_opengl >= -1.0f && y_opengl <= -1.0f + rect4_size) {
                Rectangle_red4 = uid(dre);
                Rectangle_green4 = uid(dre);
                Rectangle_blue4 = uid(dre);
            }
            else {
                r = uid(dre);
                g = uid(dre);
                b = uid(dre);
            }
        }
        else if (button == GLUT_RIGHT_BUTTON) {
            if (x_opengl >= -1.0f && x_opengl <= -1.0f + rect1_size && y_opengl >= 0.0f && y_opengl <= 0.0f + rect1_size) {
                if (rect1_size > min_size) rect1_size -= change_size;
            }
            else if (x_opengl >= 0.0f && x_opengl <= 0.0f + rect2_size && y_opengl >= 0.0f && y_opengl <= 0.0f + rect2_size) {
                if (rect2_size > min_size) rect2_size -= change_size;
            }
            else if (x_opengl >= -1.0f && x_opengl <= -1.0f + rect3_size && y_opengl >= -1.0f && y_opengl <= -1.0f + rect3_size) {
                if (rect3_size > min_size) rect3_size -= change_size;
            }
            else if (x_opengl >= 0.0f && x_opengl <= 0.0f + rect4_size && y_opengl >= -1.0f && y_opengl <= -1.0f + rect4_size) {
                if (rect4_size > min_size) rect4_size -= change_size;
            }

            else if (x_opengl >= -1.0f && x_opengl <= 0.0f && y_opengl >= 0.0f && y_opengl <= 1.0f) {
                if (rect1_size < max_size) rect1_size += change_size;
            }
            else if (x_opengl >= 0.0f && x_opengl <= 1.0f && y_opengl >= 0.0f && y_opengl <= 1.0f) {
                if (rect2_size < max_size) rect2_size += change_size;
            }
            else if (x_opengl >= -1.0f && x_opengl <= 0.0f && y_opengl >= -1.0f && y_opengl <= 0.0f) {
                if (rect3_size < max_size) rect3_size += change_size;
            }
            else if (x_opengl >= 0.0f && x_opengl <= 1.0f && y_opengl >= -1.0f && y_opengl <= 0.0f) {
                if (rect4_size < max_size) rect4_size += change_size;
            }
        }
    }
    
    glutPostRedisplay();
}