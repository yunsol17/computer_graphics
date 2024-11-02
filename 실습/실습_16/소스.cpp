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

GLuint vaoAxis, vaoSpiral, vaoCube, vaoTetrahedron, vaoCorn, vaoPyramid, vaoSphere;
GLuint vboAxis, vboSpiral[2], vboCube[2], vboTetrahedron[2], vboCorn[2], vboPyramid[2], vboSphere[2];
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;
Model modelCube, modelTetrahedron, modelCorn, modelPyramid, modelSphere;

GLfloat cubeX = 0.0f, cubeY = 0.0f, cubeZ = 1.0f;
GLfloat sphereX = 0.0f, sphereY = 0.0f, sphereZ = -1.0f;
GLfloat orbitAnglePositive = 0.0f;
GLfloat cubeScaleX = 1.0f, cubeScaleY = 1.0f, cubeScaleZ = 1.0f;
GLfloat sphereScaleX = 1.0f, sphereScaleY = 1.0f, sphereScaleZ = 1.0f;
GLfloat xRotationCube = 0.0f, xRotationSphere = 0.0f;
int spiralIndex = 0;
bool isSwappingTwo = false;
bool isSpiralActive = false;
bool orbitPositive = false;
bool isSwappingFour = false;
bool cubeMovingUp = true;
bool sphereMovingDown = true;
bool isSwappingFive = false;

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
void InitAxis();
void InitCube();
void InitCorn();
void InitPyramid();
void InitTetrahedron();
void InitSphere();
void InitSpiral();
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

void InitAxis() {
    std::vector<Vertex> axisVertices = {
        { -10.0f, 0.0f, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f) },  // X축 시작점
        { 10.0f, 0.0f, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f) },   // X축 끝점

        { 0.0f, -10.0f, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f) },  // Y축 시작점
        { 0.0f,  10.0f, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f) },  // Y축 끝점

        { 0.0f, 0.0f, -10.0f, glm::vec3(0.0f, 0.0f, 1.0f) },  // z축 시작점
        { 0.0f,  0.0f, 10.0f, glm::vec3(0.0f, 0.0f, 1.0f) }  // z축 끝점
    };

    std::vector<unsigned int> axisIndices = { 0, 1, 2, 3, 4, 5 };

    InitBuffer(vaoAxis, &vboAxis, axisVertices, axisIndices);  // InitBuffer 호출
}

void InitCube() {
    read_obj_file("cube.obj", modelCube);  // OBJ 파일 읽기

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

    InitBuffer(vaoCube, vboCube, expandedVertices, indices);  // InitBuffer 호출
}

void InitCorn() {
    read_obj_file("corn.obj", modelCorn); // corn.obj 읽기

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    std::vector<glm::vec3> vertex_colors = {
        glm::vec3(1.0f, 0.0f, 0.0f),    // 빨간색
        glm::vec3(0.0f, 1.0f, 0.0f),    // 초록색
        glm::vec3(0.0f, 0.0f, 1.0f),    // 파란색
        glm::vec3(1.0f, 1.0f, 0.0f),    // 노란색
        glm::vec3(0.0f, 1.0f, 1.0f),    // 청록색
        glm::vec3(1.0f, 0.0f, 1.0f)
    };

    for (size_t i = 0; i < modelCorn.faces.size(); ++i) {
        Vertex v1 = modelCorn.vertices[modelCorn.faces[i].v1];
        Vertex v2 = modelCorn.vertices[modelCorn.faces[i].v2];
        Vertex v3 = modelCorn.vertices[modelCorn.faces[i].v3];

        // 정점 색상 설정
        v1.color = vertex_colors[modelCorn.faces[i].v1 % vertex_colors.size()];
        v2.color = vertex_colors[modelCorn.faces[i].v2 % vertex_colors.size()];
        v3.color = vertex_colors[modelCorn.faces[i].v3 % vertex_colors.size()];

        // 확장된 정점 벡터에 추가
        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        // 인덱스 추가
        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    // 원뿔의 VAO, VBO 초기화
    InitBuffer(vaoCorn, vboCorn, expandedVertices, indices);
}

void InitTetrahedron() {
    read_obj_file("tetrahedron.obj", modelTetrahedron); // tetrahedron.obj 읽기

    glGenVertexArrays(1, &vaoTetrahedron);
    glBindVertexArray(vaoTetrahedron);

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    std::vector<glm::vec3> face_colors = {
        glm::vec3(1.0f, 0.0f, 0.0f),    // 빨간색
        glm::vec3(0.0f, 1.0f, 0.0f),    // 초록색
        glm::vec3(0.0f, 0.0f, 1.0f),    // 파란색
        glm::vec3(1.0f, 1.0f, 0.0f)     // 노란색
    };

    for (size_t i = 0; i < modelTetrahedron.faces.size(); ++i) {
        glm::vec3 face_color = face_colors[i];

        Vertex v1 = modelTetrahedron.vertices[modelTetrahedron.faces[i].v1];
        Vertex v2 = modelTetrahedron.vertices[modelTetrahedron.faces[i].v2];
        Vertex v3 = modelTetrahedron.vertices[modelTetrahedron.faces[i].v3];

        v1.color = face_color;
        v2.color = face_color;
        v3.color = face_color;

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    InitBuffer(vaoTetrahedron, vboTetrahedron, expandedVertices, indices);
}

void InitPyramid() {
    read_obj_file("pyramid.obj", modelPyramid); // pyramid.obj 읽기

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    std::vector<glm::vec3> vertex_colors = {
        glm::vec3(1.0f, 0.0f, 0.0f),    // 빨간색
        glm::vec3(0.0f, 1.0f, 0.0f),    // 초록색
        glm::vec3(0.0f, 0.0f, 1.0f),    // 파란색
        glm::vec3(1.0f, 1.0f, 0.0f),    // 노란색
        glm::vec3(0.0f, 1.0f, 1.0f),    // 청록색
    };

    for (size_t i = 0; i < modelPyramid.faces.size(); ++i) {
        Vertex v1 = modelPyramid.vertices[modelPyramid.faces[i].v1];
        Vertex v2 = modelPyramid.vertices[modelPyramid.faces[i].v2];
        Vertex v3 = modelPyramid.vertices[modelPyramid.faces[i].v3];

        v1.color = vertex_colors[modelPyramid.faces[i].v1];
        v2.color = vertex_colors[modelPyramid.faces[i].v2];
        v3.color = vertex_colors[modelPyramid.faces[i].v3];

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    InitBuffer(vaoPyramid, vboPyramid, expandedVertices, indices);
}

void InitSphere() {
    read_obj_file("sphere.obj", modelSphere);

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    std::vector<glm::vec3> vertex_colors = {
        glm::vec3(1.0f, 0.0f, 0.0f),    // 빨간색
        glm::vec3(0.0f, 1.0f, 0.0f),    // 초록색
        glm::vec3(0.0f, 0.0f, 1.0f),    // 파란색
        glm::vec3(1.0f, 1.0f, 0.0f),    // 노란색
        glm::vec3(0.0f, 1.0f, 1.0f),    // 청록색
        glm::vec3(1.0f, 0.0f, 1.0f)
    };

    for (size_t i = 0; i < modelSphere.faces.size(); ++i) {
        Vertex v1 = modelSphere.vertices[modelSphere.faces[i].v1];
        Vertex v2 = modelSphere.vertices[modelSphere.faces[i].v2];
        Vertex v3 = modelSphere.vertices[modelSphere.faces[i].v3];

        // 정점 색상 설정
        v1.color = vertex_colors[modelSphere.faces[i].v1 % vertex_colors.size()];
        v2.color = vertex_colors[modelSphere.faces[i].v2 % vertex_colors.size()];
        v3.color = vertex_colors[modelSphere.faces[i].v3 % vertex_colors.size()];

        // 확장된 정점 벡터에 추가
        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        // 인덱스 추가
        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    InitBuffer(vaoSphere, vboSphere, expandedVertices, indices);
}

std::vector<Vertex> spiralPath;

void InitSpiral() {
    std::vector<unsigned int> indices;

    float radius = 0.1f;
    float angle = 0.0f;
    float angleIncrement = 0.1f;
    float radiusIncrement = 0.01f;

    // 스파이럴 정점 생성
    for (int i = 0; i < 500; ++i) {
        Vertex vertex;
        vertex.x = radius * cos(angle);  // x 좌표
        vertex.y = 0.0f;                 // y 좌표 (xz 평면)
        vertex.z = radius * sin(angle);  // z 좌표
        vertex.color = glm::vec3(0.0f, 0.0f, 1.0f); // 파란색 설정

        spiralPath.push_back(vertex);
        indices.push_back(i);  // 각 점을 연결하여 스파이럴 형성

        angle += angleIncrement;
        radius += radiusIncrement;
    }

    // InitBuffer를 사용해 스파이럴을 VAO와 VBO에 설정
    InitBuffer(vaoSpiral, vboSpiral, spiralPath, indices);
}

void main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(window_Width, window_Height);
    glutCreateWindow("Example16");

    glewExperimental = GL_TRUE;
    glewInit();

    make_shaderProgram();
    InitCube();
    InitAxis();
    InitTetrahedron();
    InitPyramid();
    InitCorn();
    InitSphere();
    InitSpiral();

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

GLvoid drawScene() {
    glClearColor(1.0, 1.0, 1.0, 1.0f);  // 배경을 흰색으로 설정
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgramID);

    // 뷰 변환
    glm::mat4 viewMatrix = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 5.0f),  // 카메라 위치
        glm::vec3(0.0f, 0.0f, 0.0f),  // 카메라가 바라보는 대상 (원점)
        glm::vec3(0.0f, 1.0f, 0.0f)   // 월드의 위쪽 방향 (y 축)
    );

    unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // -----------------------------------
    // 회전 변환 (축과 도형 모두 동일한 회전 적용)
    // -----------------------------------
    glm::mat4 rotationMatrix = glm::mat4(1.0f);  // 기본 행렬
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(30.0f), glm::vec3(1.0f, 1.0f, 0.0f));   // X축 회전

    unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");

    // 스파이럴 그리기
    if (isSpiralActive == true) {
        glm::mat4 spiralModelMatrix = rotationMatrix;
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(spiralModelMatrix));

        glBindVertexArray(vaoSpiral);
        glDrawElements(GL_LINE_STRIP, spiralPath.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    // -----------------------------------
    // x, y, z 축 그리기 (rotationMatrix 적용)
    // -----------------------------------
    glm::mat4 axisModelMatrix = rotationMatrix;
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(axisModelMatrix));

    glBindVertexArray(vaoAxis);
    glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // -------------------------------------------
    // 정육면체 그리기 (rotationMatrix 적용 및 회전과 이동 추가)
    // -------------------------------------------

    glm::mat4 cubeModelMatrix = rotationMatrix;

    if (orbitPositive) {
        cubeModelMatrix = glm::rotate(cubeModelMatrix, glm::radians(orbitAnglePositive), glm::vec3(0.0f, 1.0f, 0.0f));  // Y축을 중심으로 회전
        cubeModelMatrix = glm::translate(cubeModelMatrix, glm::vec3(0.0f, 0.0f, 1.0f));  // 공전 반경 설정
    }

    if (isSwappingFive) {
        cubeModelMatrix = glm::rotate(cubeModelMatrix, glm::radians(orbitAnglePositive), glm::vec3(0.0f, 1.0f, 0.0f));
        cubeModelMatrix = glm::scale(cubeModelMatrix, glm::vec3(cubeScaleX, cubeScaleY, cubeScaleZ));
        cubeModelMatrix = glm::translate(cubeModelMatrix, glm::vec3(0.0f, 0.0f, 1.0f));
        cubeModelMatrix = glm::rotate(cubeModelMatrix, glm::radians(xRotationCube), glm::vec3(1.0f, 0.0f, 0.0f));
        cubeModelMatrix = glm::translate(cubeModelMatrix, glm::vec3(0.0f, 0.0f, -1.0f));
    }
    cubeModelMatrix = glm::translate(cubeModelMatrix, glm::vec3(cubeX, cubeY, cubeZ));  // 먼저 이동
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(cubeModelMatrix));

    glBindVertexArray(vaoCube);
    glDrawElements(GL_TRIANGLES, modelCube.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // --------------------------------------
    // 원 그리기 (rotationMatrix 적용 및 회전과 이동 추가)
    // --------------------------------------
    glm::mat4 sphereModelMatrix = rotationMatrix;

    if (orbitPositive) {
        sphereModelMatrix = glm::rotate(sphereModelMatrix, glm::radians(orbitAnglePositive), glm::vec3(0.0f, 1.0f, 0.0f));  // Y축을 중심으로 회전
        sphereModelMatrix = glm::translate(sphereModelMatrix, glm::vec3(0.0f, 0.0f, -1.0f));  // 공전 반경 설정
    }

    if (isSwappingFive) {
        sphereModelMatrix = glm::rotate(sphereModelMatrix, glm::radians(orbitAnglePositive), glm::vec3(0.0f, 1.0f, 0.0f));
        sphereModelMatrix = glm::scale(sphereModelMatrix, glm::vec3(sphereScaleX, sphereScaleY, sphereScaleZ));
        sphereModelMatrix = glm::translate(sphereModelMatrix, glm::vec3(0.0f, 0.0f, -1.0f));
        sphereModelMatrix = glm::rotate(sphereModelMatrix, glm::radians(xRotationCube), glm::vec3(1.0f, 0.0f, 0.0f));
        sphereModelMatrix = glm::translate(sphereModelMatrix, glm::vec3(0.0f, 0.0f, 1.0f));
    }
    sphereModelMatrix = glm::translate(sphereModelMatrix, glm::vec3(sphereX, sphereY, sphereZ));  // 이동 적용
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(sphereModelMatrix));

    glBindVertexArray(vaoSphere);
    glDrawElements(GL_TRIANGLES, modelSphere.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);


    glutSwapBuffers();  // 더블 버퍼링으로 화면 갱신
}

GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);

    float aspectRatio = (float)w / (float)h;
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

    unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
}

GLvoid Keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'q':
        glutLeaveMainLoop();
        break;
    case '1':
        isSpiralActive = true;
        break;
    case '2':
        isSwappingTwo = true;
        break;
    case '3':
        orbitPositive = true;
        break;
    case '4':
        isSwappingFour = true;
        cubeMovingUp = true;
        sphereMovingDown = true;
        break;
    case '5':
        isSwappingFive = true;
        break;
    }
    glutPostRedisplay();
}

GLvoid Timer(int value) {
    if (isSwappingTwo == true) {
        cubeZ -= 0.1f;
        sphereZ += 0.1f;

        if (cubeZ <= -1.0f && sphereZ >= 1.0f) {
            cubeZ = -1.0f;
            sphereZ = 1.0f;
            isSwappingTwo = false;
        }
    }

    if (isSpiralActive && spiralIndex < spiralPath.size()) {
        // 스파이럴 경로의 x, z 좌표를 사용하여 도형의 위치 업데이트
        cubeX = spiralPath[spiralIndex].x;
        cubeZ = 1.0f + spiralPath[spiralIndex].z;
        sphereX = spiralPath[spiralIndex].x;
        sphereZ = -1.0f + spiralPath[spiralIndex].z;
        spiralIndex++;

        // 경로가 끝나면 스파이럴 비활성화
        if (spiralIndex >= spiralPath.size()) {
            isSpiralActive = false;
            spiralIndex = 0; // 인덱스 초기화
        }
    }

    if (orbitPositive) {
        orbitAnglePositive += 1.0f;
        if (orbitAnglePositive >= 180.0f) orbitAnglePositive = 180.0f;
    }

    if (isSwappingFour == true) {
        if (cubeMovingUp) {
            cubeY += 0.1f;
            cubeZ -= 0.1f;
            if (cubeZ <= 0.0f) cubeMovingUp = false;
        }
        else {
            cubeY -= 0.1f;
            cubeZ -= 0.1f;
            if (cubeZ <= -1.0f) isSwappingFour = false;
        }

        if (sphereMovingDown) {
            sphereY -= 0.1f;
            sphereZ += 0.1f;
            if (sphereZ >= 0.0f) sphereMovingDown = false;
        }
        else {
            sphereY += 0.1f;
            sphereZ += 0.1f;
            if (sphereZ >= 1.0f) isSwappingFour = false;
        }
    }

    if (isSwappingFive) {
        if (isSwappingFive) {
            if (cubeScaleX < 1.5f) {
                cubeScaleX += 0.01f;
                cubeScaleY += 0.01f;
                cubeScaleZ += 0.01f;
            }

            if (sphereScaleX > 0.5f) {
                sphereScaleX -= 0.01f;
                sphereScaleY -= 0.01f;
                sphereScaleZ -= 0.01f;
            }

            xRotationCube += 1.0f;
            xRotationSphere += 1.0f;
            orbitAnglePositive += 1.0f;
            if (xRotationCube >= 360.0f) xRotationCube -= 360.0f;
            if (xRotationSphere >= 360.0f) xRotationSphere -= 360.0f;
            if (orbitAnglePositive >= 360.0f) orbitAnglePositive -= 360.0f;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, Timer, 0);
}