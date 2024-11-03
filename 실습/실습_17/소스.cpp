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

GLuint vaoAxis, vaoCube, vaoPyramid;
GLuint vboAxis, vboCube[2], vboPyramid[2];
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;
Model modelCube, topCube, leftCube, rightCube, frontCube, backCube, bottomCube, modelPyramid;

GLfloat t = 0.0f, f = 0.0f;
GLfloat yRotation = 0.0f;
GLfloat s = 0.0f;
GLfloat bX = 1.0f, bY = 1.0f, bZ = 1.0f;
GLfloat oRotation = 0.0f;
bool isCullingEnabled = false;
bool isYRotation = false;
bool isCubeAppear = false;
bool isPyramidAppear = false;
bool isTopRotation = false;
bool isFrontRotation = false;
bool isSideMove = false;
bool isBackScale = false;
bool isPyramidRotation = false;
bool isOrthographic = false;

GLfloat faceRotations[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
int currentOpeningFace = 0;
int currentClosingFace = 3;
bool isSequentialOpening = false;
bool isSequentialClosing = false;

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
void InitPyramid();
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
    InitPyramid();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutTimerFunc(16, Timer, 0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

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

    unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");

    // x, y, z 축 그리기 (rotationMatrix 적용)
    glm::mat4 rotationMatrix = glm::mat4(1.0f);
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(30.0f), glm::vec3(1.0f, 1.0f, 0.0f));
    glm::mat4 axisModelMatrix = rotationMatrix;
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(axisModelMatrix));

    glBindVertexArray(vaoAxis);
    glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // 도형 회전을 위한 회전 변환 행렬 (y축 기준으로 자전 적용)
    glm::mat4 cubeRotationMatrix = glm::rotate(rotationMatrix, glm::radians(yRotation), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(cubeRotationMatrix));
    // 정육면체 그리기
    if (isCubeAppear) {
        glBindVertexArray(vaoCube);

        // 윗면
        glm::mat4 topFaceMatrix = cubeRotationMatrix;
        topFaceMatrix = glm::translate(topFaceMatrix, glm::vec3(0.0f, 0.5f, 0.0f));
        topFaceMatrix = glm::rotate(topFaceMatrix, glm::radians(t), glm::vec3(1.0f, 0.0f, 0.0f));
        topFaceMatrix = glm::translate(topFaceMatrix, glm::vec3(0.0f, -0.5f, 0.0f));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(topFaceMatrix));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(GLuint)));

        // 앞면
        glm::mat4 frontFaceMatrix = cubeRotationMatrix;
        frontFaceMatrix = glm::translate(frontFaceMatrix, glm::vec3(0.0f, -0.5f, 0.5f));
        frontFaceMatrix = glm::rotate(frontFaceMatrix, glm::radians(f), glm::vec3(1.0f, 0.0f, 0.0f));
        frontFaceMatrix = glm::translate(frontFaceMatrix, glm::vec3(0.0f, 0.5f, -0.5f));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(frontFaceMatrix));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(GLuint)));

        // 왼쪽면
        glm::mat4 leftFaceMatrix = cubeRotationMatrix;
        leftFaceMatrix = glm::translate(leftFaceMatrix, glm::vec3(0.0f, s, 0.0f));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(leftFaceMatrix));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(12 * sizeof(GLuint)));

        // 아랫면
        glm::mat4 bottomFaceMatrix = cubeRotationMatrix;
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(bottomFaceMatrix));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(18 * sizeof(GLuint)));

        // 오른쪽면
        glm::mat4 rightFaceMatrix = cubeRotationMatrix;
        rightFaceMatrix = glm::translate(rightFaceMatrix, glm::vec3(0.0f, s, 0.0f));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(rightFaceMatrix));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(24 * sizeof(GLuint)));

        // 뒷면
        glm::mat4 backFaceMatrix = cubeRotationMatrix;
        backFaceMatrix = glm::scale(backFaceMatrix, glm::vec3(bX, bY, bZ));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(backFaceMatrix));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(30 * sizeof(GLuint)));

        glBindVertexArray(0);
    }
   
    // 피라미드 그리기 (rotationMatrix 적용 및 회전과 이동 추가)
    glm::mat4 pyramidModelMatrix = glm::rotate(rotationMatrix, glm::radians(yRotation), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(pyramidModelMatrix));
    // 피라미드 그리기
    if (isPyramidAppear) {
        glBindVertexArray(vaoPyramid);
        
        // 앞
        glm::mat4 frontPyramidMatrix = pyramidModelMatrix;
        frontPyramidMatrix = glm::translate(frontPyramidMatrix, glm::vec3(0.0f, -0.5f, 0.5f));
        frontPyramidMatrix = glm::rotate(frontPyramidMatrix, isSequentialOpening ? glm::radians(faceRotations[0]) : glm::radians(oRotation), glm::vec3(1.0f, 0.0f, 0.0f));
        frontPyramidMatrix = glm::translate(frontPyramidMatrix, glm::vec3(0.0f, 0.5f, -0.5f));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(frontPyramidMatrix));
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(0 * sizeof(GLuint)));

        // 왼쪽
        glm::mat4 leftPyramidMatrix = pyramidModelMatrix;
        leftPyramidMatrix = glm::translate(leftPyramidMatrix, glm::vec3(-0.5f, -0.5f, 0.0f));
        leftPyramidMatrix = glm::rotate(leftPyramidMatrix, isSequentialOpening ? glm::radians(faceRotations[1]) : glm::radians(oRotation), glm::vec3(0.0f, 0.0f, 1.0f));
        leftPyramidMatrix = glm::translate(leftPyramidMatrix, glm::vec3(0.5f, 0.5f, 0.0f));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(leftPyramidMatrix));
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(3 * sizeof(GLuint)));

        // 오른쪽
        glm::mat4 rightPyramidMatrix = pyramidModelMatrix;
        rightPyramidMatrix = glm::translate(rightPyramidMatrix, glm::vec3(0.5f, -0.5f, 0.0f));
        rightPyramidMatrix = glm::rotate(rightPyramidMatrix, isSequentialOpening ? glm::radians(-faceRotations[2]) : glm::radians(-oRotation), glm::vec3(0.0f, 0.0f, 1.0f));
        rightPyramidMatrix = glm::translate(rightPyramidMatrix, glm::vec3(-0.5f, 0.5f, 0.0f));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(rightPyramidMatrix));
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(6 * sizeof(GLuint)));

        // 뒤
        glm::mat4 backPyramidMatrix = pyramidModelMatrix;
        backPyramidMatrix = glm::translate(backPyramidMatrix, glm::vec3(0.0f, -0.5f, -0.5f));
        backPyramidMatrix = glm::rotate(backPyramidMatrix, isSequentialOpening ? glm::radians(-faceRotations[3]) : glm::radians(-oRotation), glm::vec3(1.0f, 0.0f, 0.0f));
        backPyramidMatrix = glm::translate(backPyramidMatrix, glm::vec3(0.0f, 0.5f, 0.5f));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(backPyramidMatrix));
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(9 * sizeof(GLuint)));

        // 밑
        glm::mat4 bottomPyramidMatrix = pyramidModelMatrix;
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(bottomPyramidMatrix));
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(12 * sizeof(GLuint)));
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(15 * sizeof(GLuint)));

        glBindVertexArray(0);
    }
   
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
    case 'h':
        isCullingEnabled = !isCullingEnabled;
        if (isCullingEnabled) {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
        }
        else {
            glDisable(GL_CULL_FACE);
        }
        break;
    case 'y':
        isYRotation = !isYRotation;
        break;
    case 'c':
        isCubeAppear = !isCubeAppear;
        break;
    case 'a':
        isPyramidAppear = !isPyramidAppear;
        break;
    case 't':
        isTopRotation = !isTopRotation;
        break;
    case 'f':
        isFrontRotation = !isFrontRotation;
        break;
    case 's':
        isSideMove = !isSideMove;
        break;
    case 'b':
        isBackScale = !isBackScale;
        break;
    case 'o':
        isPyramidRotation = !isPyramidRotation;
        break;
    case 'r':
        if (!isSequentialOpening && !isSequentialClosing) {
            isSequentialOpening = true;
            currentOpeningFace = 0;
        }
        else if (isSequentialOpening) {
            isSequentialOpening = false;
            isSequentialClosing = true;
            currentClosingFace = 3;
        }
        else if (isSequentialClosing) {
            isSequentialClosing = false; // 모든 동작을 멈춤
        }
        break;
    case 'p':
        isOrthographic = !isOrthographic;
        if (isOrthographic) {
            // 직각투영
            glm::mat4 projectionMatrix = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 50.0f);
            projectionMatrix = glm::translate(projectionMatrix, glm::vec3(0.0f, 0.0f, -5.0f));
            unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
            glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        }
        else {
            // 원근투영
            glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)window_Width / (float)window_Height, 0.1f, 50.0f);
            projectionMatrix = glm::translate(projectionMatrix, glm::vec3(0.0f, 0.0f, -5.0f));
            unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
            glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        }
        break;
        glutPostRedisplay();
    }
}

GLvoid Timer(int value) {
    if (isYRotation) {
        yRotation += 1.0f;
        if (yRotation >= 360.0f) yRotation -= 360.0f;
    }
    
    if (isTopRotation == true) {
        t += 1.0f;
        if (t >= 360.0f) t -= 360.0f;
    }

    if (isFrontRotation) {
        if (f < 90.0f) {
            f += 1.0f;
            if (f >= 90.0f) f = 90.0f;
        }
    }
    else {
        if (f > 0.0f) {
            f -= 1.0f;
            if (f <= 0.0f) f = 0.0f;
        }
    }

    if (isSideMove) {
        if (s < 1.0f) {
            s += 0.1f;
            if (s >= 1.0f) s = 1.0f;
        }
    }
    else {
        if (s > 0.0f) {
            s -= 0.1f;
            if (s <= 0.0f) s = 0.0f;
        }
    }

    if (isBackScale) {
        if (bX > 0.0f && bY > 0.0f && bZ > 0.0f) {
            bX -= 0.1f, bY -= 0.1f, bZ -= 0.1f;
            if (bX <= 0.0f && bY <= 0.0f && bZ <= 0.0f) bX = 0.0f, bY = 0.0f, bZ = 0.0f;
        }
    }
    else {
        if (bX < 1.0f && bY < 1.0f && bZ < 1.0f) {
            bX += 0.1f, bY += 0.1f, bZ += 0.1f;
            if (bX >= 1.0f && bY >= 1.0f && bZ >= 1.0f) bX = 1.0f, bY = 1.0f, bZ = 1.0f;
        }
    }

    if (isPyramidRotation) {
        if (oRotation < 233.0f) {
            oRotation += 1.0f;
            if (oRotation >= 233.0f) oRotation = 233.0f;
        }
    }
    else {
        if (oRotation > 0.0f) {
            oRotation -= 1.0f;
            if (oRotation <= 0.0f) oRotation = 0.0f;
        }
    }

    if (isSequentialOpening) {
        if (currentOpeningFace < 4) {
            if (faceRotations[currentOpeningFace] < 90.0f) {
                faceRotations[currentOpeningFace] += 1.0f;
                if (faceRotations[currentOpeningFace] >= 90.0f) {
                    faceRotations[currentOpeningFace] = 90.0f;
                    currentOpeningFace++;
                }
            }
        }
    }
    else if (isSequentialClosing) { // 조건 추가: isSequentialClosing이 true일 때만 실행
        if (currentClosingFace >= 0) {
            if (faceRotations[currentClosingFace] > 0.0f) {
                faceRotations[currentClosingFace] -= 1.0f;
                if (faceRotations[currentClosingFace] <= 0.0f) {
                    faceRotations[currentClosingFace] = 0.0f;
                    currentClosingFace--;
                }
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, Timer, 0);
}