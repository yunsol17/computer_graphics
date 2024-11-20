#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include "헤더.h"

GLuint vaoCube, vaoBottom, vaoMiddle, vaoLeftArm, vaoRightArm, vaoLeftBarrel, vaoRightBarrel;
GLuint vboCube[2], vboBottom[2], vboMiddle[2], vboLeftArm[2], vboRightArm[2], vboLeftBarrel[2], vboRightBarrel[2];
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;
Model modelCube, modelBottom, modelMiddle, modelLeftArm, modelRightArm, modelLeftBarrel, modelRightBarrel;

GLfloat cameraX = 0.0f;
GLfloat cameraZ = 3.0f;
GLfloat cameraYrotate = 0.0f;     // 자전
GLfloat cameraYrevolve = 0.0f;    // 공전
GLfloat bottomX = 0.0f;
GLfloat middleX = 0.0f;
GLfloat leftArmX = -0.06f;
GLfloat rightArmX = 0.06f;
GLfloat middleY = 0.0f;
GLfloat leftArmY = 0.0f;
GLfloat rightArmY = 0.0f;
GLfloat leftBarrelY = 0.0f;
GLfloat rightBarrelY = 0.0f;
GLfloat leftArmZ = 0.0f;
GLfloat rightArmZ = 0.0f;
GLfloat leftBarrelX = -0.06f;
GLfloat rightBarrelX = 0.06f;

bool isCameraXmove = false;
bool isCameraXmoveReverse = false;
bool isCameraZmove = false;
bool isCameraZmoveReverse = false;
bool isCameraYrotate = false;
bool isCameraYrotateReverse = false;
bool isCameraYrevolve = false;
bool isCameraYrevolveReverse = false;
bool isXmove = false;
bool isXmoveReverse = false;
bool isYrotate = false;
bool isYrotateReverse = false;
bool isBarrelYrotate = false;
bool isBarrelYrotateReverse = false;
bool isArmZrotate = false;
bool isArmZrotateReverse = false;
bool isBarrelXmove = false;
bool isBarrelXmoveReverse = false;

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
void InitBuffer();
void InitCube();
void InitBottom();
void InitMiddle();
void InitLeftArm();
void InitRightArm();
void InitLeftBarrel();
void InitRightBarrel();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Timer(int value);

int window_Width = 800;
int window_Height = 600;

void InitBuffer(GLuint& vao, GLuint* vbo, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(2, vbo);

    // 정점버퍼 (위치와 색상)
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // 버퍼 요소들 (인덱스)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // 정점을 사용하게 함
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);  // Position attribute
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));  // Color attribute
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);  // VAO 바인드 해제
}

void InitCube() {
    read_obj_file("cube.obj", modelCube);

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    std::vector<glm::vec3> face_colors = {
        glm::vec3(1.0f, 0.0f, 0.0f),    // 빨간색
        glm::vec3(0.0f, 1.0f, 0.0f),    // 초록색
        glm::vec3(0.0f, 0.0f, 1.0f),    // 파란색
        glm::vec3(1.0f, 1.0f, 0.0f),    // 노란색
        glm::vec3(0.0f, 1.0f, 1.0f),    // 청록색
        glm::vec3(1.0f, 0.0f, 1.0f)     // 자홍색
    };

    for (size_t i = 0; i < modelCube.faces.size(); ++i) {
        Vertex v1 = { modelCube.vertices[modelCube.faces[i].v1].x,
              modelCube.vertices[modelCube.faces[i].v1].y,
              modelCube.vertices[modelCube.faces[i].v1].z,
              face_colors[i / 2] };

        Vertex v2 = { modelCube.vertices[modelCube.faces[i].v2].x,
                      modelCube.vertices[modelCube.faces[i].v2].y,
                      modelCube.vertices[modelCube.faces[i].v2].z,
                      face_colors[i / 2] };

        Vertex v3 = { modelCube.vertices[modelCube.faces[i].v3].x,
                      modelCube.vertices[modelCube.faces[i].v3].y,
                      modelCube.vertices[modelCube.faces[i].v3].z,
                      face_colors[i / 2] };

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    InitBuffer(vaoCube, vboCube, expandedVertices, indices);
}

void InitBottom() {
    read_obj_file("bottom.obj", modelBottom);

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    std::vector<glm::vec3> face_colors = {
        glm::vec3(1.0f, 0.0f, 0.0f),    // 빨간색
        glm::vec3(0.0f, 1.0f, 0.0f),    // 초록색
        glm::vec3(0.0f, 0.0f, 1.0f),    // 파란색
        glm::vec3(1.0f, 1.0f, 0.0f),    // 노란색
        glm::vec3(0.0f, 1.0f, 1.0f),    // 청록색
        glm::vec3(1.0f, 0.0f, 1.0f)     // 자홍색
    };

    for (size_t i = 0; i < modelBottom.faces.size(); ++i) {
        Vertex v1 = { modelBottom.vertices[modelBottom.faces[i].v1].x,
              modelBottom.vertices[modelBottom.faces[i].v1].y,
              modelBottom.vertices[modelBottom.faces[i].v1].z,
              face_colors[i / 2] };

        Vertex v2 = { modelBottom.vertices[modelBottom.faces[i].v2].x,
                      modelBottom.vertices[modelBottom.faces[i].v2].y,
                      modelBottom.vertices[modelBottom.faces[i].v2].z,
                      face_colors[i / 2] };

        Vertex v3 = { modelBottom.vertices[modelBottom.faces[i].v3].x,
                      modelBottom.vertices[modelBottom.faces[i].v3].y,
                      modelBottom.vertices[modelBottom.faces[i].v3].z,
                      face_colors[i / 2] };

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    InitBuffer(vaoBottom, vboBottom, expandedVertices, indices);
}

void InitMiddle() {
    read_obj_file("middle.obj", modelMiddle);

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    std::vector<glm::vec3> face_colors = {
        glm::vec3(1.0f, 0.0f, 0.0f),    // 빨간색
        glm::vec3(0.0f, 1.0f, 0.0f),    // 초록색
        glm::vec3(0.0f, 0.0f, 1.0f),    // 파란색
        glm::vec3(1.0f, 1.0f, 0.0f),    // 노란색
        glm::vec3(0.0f, 1.0f, 1.0f),    // 청록색
        glm::vec3(1.0f, 0.0f, 1.0f)     // 자홍색
    };

    for (size_t i = 0; i < modelMiddle.faces.size(); ++i) {
        Vertex v1 = { modelMiddle.vertices[modelMiddle.faces[i].v1].x,
              modelMiddle.vertices[modelMiddle.faces[i].v1].y,
              modelMiddle.vertices[modelMiddle.faces[i].v1].z,
              face_colors[i / 2] };

        Vertex v2 = { modelMiddle.vertices[modelMiddle.faces[i].v2].x,
                      modelMiddle.vertices[modelMiddle.faces[i].v2].y,
                      modelMiddle.vertices[modelMiddle.faces[i].v2].z,
                      face_colors[i / 2] };

        Vertex v3 = { modelMiddle.vertices[modelMiddle.faces[i].v3].x,
                      modelMiddle.vertices[modelMiddle.faces[i].v3].y,
                      modelMiddle.vertices[modelMiddle.faces[i].v3].z,
                      face_colors[i / 2] };

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    InitBuffer(vaoMiddle, vboMiddle, expandedVertices, indices);
}

void InitLeftArm() {
    read_obj_file("leftArm.obj", modelLeftArm);

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    std::vector<glm::vec3> face_colors = {
        glm::vec3(1.0f, 0.0f, 0.0f),    // 빨간색
        glm::vec3(0.0f, 1.0f, 0.0f),    // 초록색
        glm::vec3(0.0f, 0.0f, 1.0f),    // 파란색
        glm::vec3(1.0f, 1.0f, 0.0f),    // 노란색
        glm::vec3(0.0f, 1.0f, 1.0f),    // 청록색
        glm::vec3(1.0f, 0.0f, 1.0f)     // 자홍색
    };

    for (size_t i = 0; i < modelLeftArm.faces.size(); ++i) {
        Vertex v1 = { modelLeftArm.vertices[modelLeftArm.faces[i].v1].x,
                      modelLeftArm.vertices[modelLeftArm.faces[i].v1].y,
                      modelLeftArm.vertices[modelLeftArm.faces[i].v1].z,
                      face_colors[i % face_colors.size()] };

        Vertex v2 = { modelLeftArm.vertices[modelLeftArm.faces[i].v2].x,
                      modelLeftArm.vertices[modelLeftArm.faces[i].v2].y,
                      modelLeftArm.vertices[modelLeftArm.faces[i].v2].z,
                      face_colors[i % face_colors.size()] };

        Vertex v3 = { modelLeftArm.vertices[modelLeftArm.faces[i].v3].x,
                      modelLeftArm.vertices[modelLeftArm.faces[i].v3].y,
                      modelLeftArm.vertices[modelLeftArm.faces[i].v3].z,
                      face_colors[i % face_colors.size()] };

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    InitBuffer(vaoLeftArm, vboLeftArm, expandedVertices, indices);
}

void InitRightArm() {
    read_obj_file("rightArm.obj", modelRightArm);

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    std::vector<glm::vec3> face_colors = {
        glm::vec3(1.0f, 0.0f, 0.0f),    // 빨간색
        glm::vec3(0.0f, 1.0f, 0.0f),    // 초록색
        glm::vec3(0.0f, 0.0f, 1.0f),    // 파란색
        glm::vec3(1.0f, 1.0f, 0.0f),    // 노란색
        glm::vec3(0.0f, 1.0f, 1.0f),    // 청록색
        glm::vec3(1.0f, 0.0f, 1.0f)     // 자홍색
    };

    for (size_t i = 0; i < modelRightArm.faces.size(); ++i) {
        Vertex v1 = { modelRightArm.vertices[modelRightArm.faces[i].v1].x,
                      modelRightArm.vertices[modelRightArm.faces[i].v1].y,
                      modelRightArm.vertices[modelRightArm.faces[i].v1].z,
                      face_colors[i % face_colors.size()] };

        Vertex v2 = { modelRightArm.vertices[modelRightArm.faces[i].v2].x,
                      modelRightArm.vertices[modelRightArm.faces[i].v2].y,
                      modelRightArm.vertices[modelRightArm.faces[i].v2].z,
                      face_colors[i % face_colors.size()] };

        Vertex v3 = { modelRightArm.vertices[modelRightArm.faces[i].v3].x,
                      modelRightArm.vertices[modelRightArm.faces[i].v3].y,
                      modelRightArm.vertices[modelRightArm.faces[i].v3].z,
                      face_colors[i % face_colors.size()] };

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    InitBuffer(vaoRightArm, vboRightArm, expandedVertices, indices);
}

void InitLeftBarrel() {
    read_obj_file("leftBarrel.obj", modelLeftBarrel);

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    std::vector<glm::vec3> face_colors = {
        glm::vec3(1.0f, 0.0f, 0.0f),    // 빨간색
        glm::vec3(0.0f, 1.0f, 0.0f),    // 초록색
        glm::vec3(0.0f, 0.0f, 1.0f),    // 파란색
        glm::vec3(1.0f, 1.0f, 0.0f),    // 노란색
        glm::vec3(0.0f, 1.0f, 1.0f),    // 청록색
        glm::vec3(1.0f, 0.0f, 1.0f)     // 자홍색
    };

    for (size_t i = 0; i < modelLeftBarrel.faces.size(); ++i) {
        Vertex v1 = { modelLeftBarrel.vertices[modelLeftBarrel.faces[i].v1].x,
                      modelLeftBarrel.vertices[modelLeftBarrel.faces[i].v1].y,
                      modelLeftBarrel.vertices[modelLeftBarrel.faces[i].v1].z,
                      face_colors[i % face_colors.size()] };

        Vertex v2 = { modelLeftBarrel.vertices[modelLeftBarrel.faces[i].v2].x,
                      modelLeftBarrel.vertices[modelLeftBarrel.faces[i].v2].y,
                      modelLeftBarrel.vertices[modelLeftBarrel.faces[i].v2].z,
                      face_colors[i % face_colors.size()] };

        Vertex v3 = { modelLeftBarrel.vertices[modelLeftBarrel.faces[i].v3].x,
                      modelLeftBarrel.vertices[modelLeftBarrel.faces[i].v3].y,
                      modelLeftBarrel.vertices[modelLeftBarrel.faces[i].v3].z,
                      face_colors[i % face_colors.size()] };

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    InitBuffer(vaoLeftBarrel, vboLeftBarrel, expandedVertices, indices);
}

void InitRightBarrel() {
    read_obj_file("rightBarrel.obj", modelRightBarrel);

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    std::vector<glm::vec3> face_colors = {
        glm::vec3(1.0f, 0.0f, 0.0f),    // 빨간색
        glm::vec3(0.0f, 1.0f, 0.0f),    // 초록색
        glm::vec3(0.0f, 0.0f, 1.0f),    // 파란색
        glm::vec3(1.0f, 1.0f, 0.0f),    // 노란색
        glm::vec3(0.0f, 1.0f, 1.0f),    // 청록색
        glm::vec3(1.0f, 0.0f, 1.0f)     // 자홍색
    };

    for (size_t i = 0; i < modelRightBarrel.faces.size(); ++i) {
        Vertex v1 = { modelRightBarrel.vertices[modelRightBarrel.faces[i].v1].x,
                      modelRightBarrel.vertices[modelRightBarrel.faces[i].v1].y,
                      modelRightBarrel.vertices[modelRightBarrel.faces[i].v1].z,
                      face_colors[i % face_colors.size()] };

        Vertex v2 = { modelRightBarrel.vertices[modelRightBarrel.faces[i].v2].x,
                      modelRightBarrel.vertices[modelRightBarrel.faces[i].v2].y,
                      modelRightBarrel.vertices[modelRightBarrel.faces[i].v2].z,
                      face_colors[i % face_colors.size()] };

        Vertex v3 = { modelRightBarrel.vertices[modelRightBarrel.faces[i].v3].x,
                      modelRightBarrel.vertices[modelRightBarrel.faces[i].v3].y,
                      modelRightBarrel.vertices[modelRightBarrel.faces[i].v3].z,
                      face_colors[i % face_colors.size()] };

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    InitBuffer(vaoRightBarrel, vboRightBarrel, expandedVertices, indices);
}

void main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(window_Width, window_Height);
    glutCreateWindow("실습19");

    glewExperimental = GL_TRUE;
    glewInit();

    make_shaderProgram();
    InitCube();
    InitBottom();
    InitMiddle();
    InitLeftArm();
    InitRightArm();
    InitLeftBarrel();
    InitRightBarrel();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutTimerFunc(16, Timer, 0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_CULL_FACE);

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

void renderSceneObjects() {
    // 바닥
    glm::mat4 cubeModelMatrix = glm::mat4(1.0f);
    GLint modelMatrixLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(cubeModelMatrix));

    glBindVertexArray(vaoCube);
    glDrawElements(GL_TRIANGLES, modelCube.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // 탱크 바닥
    glm::mat4 bottomModelMatrix = glm::mat4(1.0f);
    bottomModelMatrix = glm::translate(bottomModelMatrix, glm::vec3(bottomX, 0.0f, 0.0f));
    bottomModelMatrix = glm::translate(bottomModelMatrix, glm::vec3(0.0f, 0.0625f, 0.0f));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(bottomModelMatrix));

    glBindVertexArray(vaoBottom);
    glDrawElements(GL_TRIANGLES, modelBottom.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // 탱크 중간
    glm::mat4 middleModelMatrix = glm::mat4(1.0f);
    middleModelMatrix = glm::translate(middleModelMatrix, glm::vec3(middleX, 0.0f, 0.0f));
    middleModelMatrix = glm::translate(middleModelMatrix, glm::vec3(0.0f, 0.1425f, 0.0f));
    middleModelMatrix = glm::rotate(middleModelMatrix, glm::radians(middleY), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(middleModelMatrix));

    glBindVertexArray(vaoMiddle);
    glDrawElements(GL_TRIANGLES, modelMiddle.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // 왼쪽 팔
    glm::mat4 leftArmModelMatrix = glm::mat4(1.0f);
    leftArmModelMatrix = glm::rotate(middleModelMatrix, glm::radians(leftArmY), glm::vec3(0.0f, 1.0f, 0.0f));
    leftArmModelMatrix = glm::rotate(leftArmModelMatrix, glm::radians(leftArmZ), glm::vec3(0.0f, 0.0f, 1.0f));
    leftArmModelMatrix = glm::translate(leftArmModelMatrix, glm::vec3(-0.06f, 0.1f, 0.0f));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(leftArmModelMatrix));

    glBindVertexArray(vaoLeftArm);
    glDrawElements(GL_TRIANGLES, modelLeftArm.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // 오른쪽 팔
    glm::mat4 rightArmModelMatrix = glm::mat4(1.0f);
    rightArmModelMatrix = glm::rotate(middleModelMatrix, glm::radians(rightArmY), glm::vec3(0.0f, 1.0f, 0.0f));
    rightArmModelMatrix = glm::rotate(middleModelMatrix, glm::radians(rightArmZ), glm::vec3(0.0f, 0.0f, 1.0f));
    rightArmModelMatrix = glm::translate(rightArmModelMatrix, glm::vec3(0.06f, 0.1f, 0.0f));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(rightArmModelMatrix));

    glBindVertexArray(vaoRightArm);
    glDrawElements(GL_TRIANGLES, modelRightArm.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // 왼쪽 포신
    glm::mat4 leftBarrelModelMatrix = glm::mat4(1.0f);
    leftBarrelModelMatrix = glm::rotate(leftBarrelModelMatrix, glm::radians(leftBarrelY), glm::vec3(0.0f, 1.0f, 0.0f));
    leftBarrelModelMatrix = glm::translate(leftBarrelModelMatrix, glm::vec3(leftBarrelX, 0.1025f, 0.26f));
    leftBarrelModelMatrix = glm::rotate(leftBarrelModelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(leftBarrelModelMatrix));

    glBindVertexArray(vaoLeftBarrel);
    glDrawElements(GL_TRIANGLES, modelLeftBarrel.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // 오른쪽 포신
    glm::mat4 rightBarrelModelMatrix = glm::mat4(1.0f);
    rightBarrelModelMatrix = glm::rotate(rightBarrelModelMatrix, glm::radians(rightBarrelY), glm::vec3(0.0f, 1.0f, 0.0f));
    rightBarrelModelMatrix = glm::translate(rightBarrelModelMatrix, glm::vec3(rightBarrelX, 0.1025f, 0.26f));
    rightBarrelModelMatrix = glm::rotate(rightBarrelModelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(rightBarrelModelMatrix));

    glBindVertexArray(vaoRightBarrel);
    glDrawElements(GL_TRIANGLES, modelRightBarrel.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

GLvoid drawScene() {
    glClearColor(1.0, 1.0, 1.0, 1.0f);  // 배경을 흰색으로 설정
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgramID);

    GLint projMatrixLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
    GLint viewMatrixLocation = glGetUniformLocation(shaderProgramID, "viewTransform");

    // 1. 원근 투영이 적용된 뷰포트 (왼쪽 하단)
    glViewport(0, 0, window_Width / 2, window_Height / 2);
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)window_Width / (2 * window_Height), 0.1f, 100.0f);
    glUniformMatrix4fv(projMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // 카메라 위치 및 방향 계산 (공전/자전 포함)
    float revolveAngle = glm::radians(cameraYrevolve);
    glm::mat4 revolveMatrix = glm::rotate(glm::mat4(1.0f), revolveAngle, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 initialCameraPos(cameraX, 0.3f, cameraZ);
    glm::vec3 finalCameraPos = glm::vec3(revolveMatrix * glm::vec4(initialCameraPos, 1.0f));

    float rotateAngle = glm::radians(cameraYrotate);
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotateAngle, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 direction = target - finalCameraPos;
    glm::vec3 rotatedDirection = glm::vec3(rotationMatrix * glm::vec4(direction, 0.0f));
    glm::vec3 rotatedTarget = finalCameraPos + rotatedDirection;

    glm::mat4 viewMatrix = glm::lookAt(
        finalCameraPos,    // 카메라 위치
        rotatedTarget,     // 카메라가 보는 지점
        glm::vec3(0.0f, 1.0f, 0.0f)  // 위쪽 방향
    );

    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    renderSceneObjects();

    // 2. XZ 평면 직각 투영이 적용된 뷰포트 (오른쪽 하단)
    glViewport(window_Width / 2, 0, window_Width / 2, window_Height / 2);
    projectionMatrix = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -10.0f, 10.0f);
    glUniformMatrix4fv(projMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    viewMatrix = glm::lookAt(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));  // XZ 평면
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    renderSceneObjects();

    // 3. XY 평면 직각 투영이 적용된 뷰포트 (상단)
    glViewport(0, window_Height / 2, window_Width, window_Height / 2);
    projectionMatrix = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -10.0f, 10.0f);
    glUniformMatrix4fv(projMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // XY 평면
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    renderSceneObjects();

    glutSwapBuffers();
}


GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

GLvoid Keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'q':
        glutLeaveMainLoop();
        break;
    case 'x':
        isCameraXmove = !isCameraXmove;
        break;
    case 'X':
        isCameraXmoveReverse = !isCameraXmoveReverse;
        break;
    case 'z':
        isCameraZmove = !isCameraZmove;
        break;
    case 'Z':
        isCameraZmoveReverse = !isCameraZmoveReverse;
        break;
    case 'y':
        isCameraYrotate = !isCameraYrotate;
        break;
    case 'Y':
        isCameraYrotateReverse = !isCameraYrotateReverse;
        break;
    case 'r':
        isCameraYrevolve = !isCameraYrevolve;
        break;
    case 'R':
        isCameraYrevolveReverse = !isCameraYrevolveReverse;
        break;
    case 'b':
        isXmove = !isXmove;
        break;
    case 'B':
        isXmoveReverse = !isXmoveReverse;
        break;
    case 'm':
        isYrotate = !isYrotate;
        break;
    case 'M':
        isYrotateReverse = !isYrotateReverse;
        break;
    case 'f':
        isBarrelYrotate = !isBarrelYrotate;
        break;
    case 'F':
        isBarrelYrotateReverse = !isBarrelYrotateReverse;
        break;
    case 'e':
        isBarrelXmove = !isBarrelXmove;
        break;
    case 'E':
        isBarrelXmoveReverse = !isBarrelXmoveReverse;
        break;
    case 't':
        isArmZrotate = !isArmZrotate;
        break;
    case 'T':
        isArmZrotateReverse = !isArmZrotateReverse;
        break;
    }
    glutPostRedisplay();
}

GLvoid Timer(int value) {
    if (isCameraXmove) {
        if (isCameraXmoveReverse) {
            cameraX -= 0.01f;
        }
        else {
            cameraX += 0.01f;
        }
    }

    if (isCameraZmove) {
        if (isCameraZmoveReverse) {
            cameraZ -= 0.01f;
        }
        else {
            cameraZ += 0.01f;
        }
    }

    if (isCameraYrotate) {
        if (isCameraYrotateReverse) {
            cameraYrotate -= 1.5f;
        }
        else {
            cameraYrotate += 1.5f;
        }
    }

    if (isCameraYrevolve) {
        if (isCameraYrevolveReverse) {
            cameraYrevolve -= 1.5f;
        }
        else {
            cameraYrevolve += 1.5f;
        }
    }

    if (isXmove) {
        if (isXmoveReverse) {
            bottomX -= 0.01f;
            middleX -= 0.01f;
            leftArmX -= 0.01f;
            rightArmX -= 0.01f;
        }
        else {
            bottomX += 0.01f;
            middleX += 0.01f;
            leftArmX += 0.01f;
            rightArmX += 0.01f;
        }
    }

    if (isYrotate) {
        if (isYrotateReverse) {
            middleY -= 0.5f;
            leftArmY -= 0.5f;
            rightArmY -= 0.5f;
        }
        else {
            middleY += 0.5f;
            leftArmY += 0.5f;
            rightArmY += 0.5f;
        }
    }

    if (isBarrelYrotate) {
        if (isBarrelYrotateReverse) {
            leftBarrelY -= 0.5f;
            rightBarrelY += 0.5f;
        }
        else {
            leftBarrelY += 0.5f;
            rightBarrelY -= 0.5f;
        }
    }

    if (isArmZrotate) {
        if (isArmZrotateReverse) {
            leftArmZ += 1.0f;
            rightArmZ -= 1.0f;
            if (leftArmZ >= 90.0f) {
                leftArmZ = 90.0f;
                rightArmZ = -90.0f;
            }
            
        }
        else {
            leftArmZ -= 1.0f;
            rightArmZ += 1.0f;
            if (leftArmZ <= -90.0f) {
                leftArmZ = -90.0f;
                rightArmZ = 90.0f;
            }
        }
    }

    if (isBarrelXmove) {
        if (isBarrelXmoveReverse) {
            leftBarrelX += 0.01f;
            rightBarrelX -= 0.01f;
            if (leftBarrelX >= 0.0f) {
                leftBarrelX = 0.0f;
                rightBarrelX = 0.0f;
            }
        }
        else {
            leftBarrelX -= 0.01f;
            rightBarrelX += 0.01f;
            if (leftBarrelX <= -0.06f) {
                leftBarrelX = -0.06;
                rightBarrelX = 0.06f;
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, Timer, 0);
}