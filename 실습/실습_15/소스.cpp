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

GLuint vaoAxis, vaoCube, vaoTetrahedron, vaoCorn, vaoPyramid;
GLuint vboAxis, vboCube[2], vboTetrahedron[2], vboCorn[2], vboPyramid[2];
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;
Model modelCube, modelTetrahedron, modelCorn, modelPyramid;

GLfloat xRotationCube = 0.0f;
GLfloat yRotationCube = 0.0f;
GLfloat xRotationCorn = 0.0f;
GLfloat yRotationCorn = 0.0f;
GLfloat orbitAnglePositive = 0.0f;
GLfloat orbitAngleNegative = 0.0f;
bool rotatePositiveX = false;
bool rotateNegativeX = false;
bool rotatePositiveY = false;
bool rotateNegativeY = false;
bool rotateCube = false;
bool rotateCorn = false;
bool orbitPositive = false;
bool orbitNegative = false;
bool useAlternativeShapes = false;

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

void main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(window_Width, window_Height);
    glutCreateWindow("Example15");

    glewExperimental = GL_TRUE;
    glewInit();

    make_shaderProgram();
    InitCube();
    InitAxis();
    InitTetrahedron();
    InitPyramid();
    InitCorn();

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
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // Y축 회전
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));   // X축 회전

    unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");

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
        cubeModelMatrix = glm::translate(cubeModelMatrix, glm::vec3(-1.0f, 0.0f, 0.0f));  // 공전 반경 설정
    }
    else if (orbitNegative) {
        cubeModelMatrix = glm::rotate(cubeModelMatrix, glm::radians(orbitAngleNegative), glm::vec3(0.0f, 1.0f, 0.0f));  // Y축을 중심으로 회전
        cubeModelMatrix = glm::translate(cubeModelMatrix, glm::vec3(-1.0f, 0.0f, 0.0f));  // 공전 반경 설정
    }
    cubeModelMatrix = glm::translate(cubeModelMatrix, glm::vec3(-1.0f, 0.0f, 0.0f));  // 먼저 이동
    cubeModelMatrix = glm::rotate(cubeModelMatrix, glm::radians(yRotationCube), glm::vec3(0.0f, 1.0f, 0.0f));   // 회전 적용
    cubeModelMatrix = glm::rotate(cubeModelMatrix, glm::radians(xRotationCube), glm::vec3(1.0f, 0.0f, 0.0f));   // 회전 적용
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(cubeModelMatrix));

    if (useAlternativeShapes) {
        glBindVertexArray(vaoPyramid);
        glDrawElements(GL_TRIANGLES, modelPyramid.faces.size() * 3, GL_UNSIGNED_INT, 0);
    }
    else {
        glBindVertexArray(vaoCube);
        glDrawElements(GL_TRIANGLES, modelCube.faces.size() * 3, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);

    // --------------------------------------
    // 원뿔 그리기 (rotationMatrix 적용 및 회전과 이동 추가)
    // --------------------------------------
    glm::mat4 cornModelMatrix = rotationMatrix;
    if (orbitPositive) {
        cornModelMatrix = glm::rotate(cornModelMatrix, glm::radians(orbitAnglePositive), glm::vec3(0.0f, 1.0f, 0.0f));  // 반대 방향 공전
        cornModelMatrix = glm::translate(cornModelMatrix, glm::vec3(1.0f, 0.0f, 0.0f));  // 공전 반경 설정
    }
    else if (orbitNegative) {
        cornModelMatrix = glm::rotate(cornModelMatrix, glm::radians(orbitAngleNegative), glm::vec3(0.0f, 1.0f, 0.0f));  // 반대 방향 공전
        cornModelMatrix = glm::translate(cornModelMatrix, glm::vec3(1.0f, 0.0f, 0.0f));  // 공전 반경 설정
    }
    cornModelMatrix = glm::translate(cornModelMatrix, glm::vec3(1.0f, 0.0f, 0.0f));  // 이동 적용
    cornModelMatrix = glm::rotate(cornModelMatrix, glm::radians(yRotationCorn), glm::vec3(0.0f, 1.0f, 0.0f));   // 회전 적용
    cornModelMatrix = glm::rotate(cornModelMatrix, glm::radians(xRotationCorn), glm::vec3(1.0f, 0.0f, 0.0f));   // 회전 적용
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(cornModelMatrix));

    if (useAlternativeShapes) {
        glBindVertexArray(vaoTetrahedron);
        glDrawElements(GL_TRIANGLES, modelTetrahedron.faces.size() * 3, GL_UNSIGNED_INT, 0);
    }
    else {
        glBindVertexArray(vaoCorn);
        glDrawElements(GL_TRIANGLES, modelCorn.faces.size() * 3, GL_UNSIGNED_INT, 0);
    }
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
    case 'x':
        rotatePositiveX = !rotatePositiveX;
        rotateNegativeX = false;
        break;
    case 'X':
        rotatePositiveX = false;
        rotateNegativeX = !rotateNegativeX;
        break;
    case 'y':
        rotatePositiveY = !rotatePositiveY;
        rotateNegativeY = false;
        break;
    case 'Y':
        rotatePositiveY = false;
        rotateNegativeY = !rotateNegativeY;
        break;
    case '1': 
        rotateCube = !rotateCube;
        rotateCorn = false;
        break;
    case '2':
        rotateCorn = !rotateCorn;
        rotateCube = false;
        break;
    case '3':
        rotateCube = !rotateCube;
        rotateCorn = !rotateCorn;
        break;
    case 'r':
        orbitPositive = !orbitPositive;
        break;
    case 'R':
        orbitNegative = !orbitNegative;
        break;
    case 'c':
        useAlternativeShapes = true;
        break;
    case 's':
        rotatePositiveX = false;
        rotateNegativeX = false;
        rotatePositiveY = false;
        rotatePositiveY = false;
        rotateCube = false;
        rotateCorn = false;
        orbitPositive = false;
        orbitNegative = false;
        useAlternativeShapes = false;
    }
    glutPostRedisplay();
}

GLvoid Timer(int value) {
    if (rotateCube) {
        if (rotatePositiveX) {
            xRotationCube += 1.0f;
            if (xRotationCube > 360.0f) xRotationCube -= 360.0f;
        }
        if (rotateNegativeX) {
            xRotationCube -= 1.0f;
            if (xRotationCube < 0.0f) xRotationCube += 360.0f;
        }
        if (rotatePositiveY) {
            yRotationCube += 1.0f;
            if (yRotationCube > 360.0f) yRotationCube -= 360.0f;
        }
        if (rotateNegativeY) {
            yRotationCube -= 1.0f;
            if (yRotationCube < 0.0f) yRotationCube += 360.0f;
        }
    }

    if (rotateCorn) {
        if (rotatePositiveX) {
            xRotationCorn += 1.0f;
            if (xRotationCorn > 360.0f) xRotationCorn -= 360.0f;
        }
        if (rotateNegativeX) {
            xRotationCorn -= 1.0f;
            if (xRotationCorn < 0.0f) xRotationCorn += 360.0f;
        }
        if (rotatePositiveY) {
            yRotationCorn += 1.0f;
            if (yRotationCorn > 360.0f) yRotationCorn -= 360.0f;
        }
        if (rotateNegativeY) {
            yRotationCorn -= 1.0f;
            if (yRotationCorn < 0.0f) yRotationCorn += 360.0f;
        }
    }

    if (orbitPositive) {
        orbitAnglePositive += 1.0f;
        if (orbitAnglePositive > 360.0f) orbitAnglePositive -= 360.0f;
    }

    else if (orbitNegative) {
        orbitAngleNegative -= 1.0f;
        if (orbitAngleNegative < 0.0f) orbitAngleNegative += 360.0f;
    }

    glutPostRedisplay();
    glutTimerFunc(16, Timer, 0);
}