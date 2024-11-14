#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>

GLuint vaoTriangle, vaoRectangle, vaoPentagon, vaoHexagon, vaoBox, vaoCreateNewPolygon;
GLuint vboTriangle[2], vboRectangle[2], vboPentagon[2], vboHexagon[2], vboBox[2];
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

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
void InitTriangle();
void InitRectangle();
void InitPentagon();
void InitHexagon();
void InitPathBuffer();
void InitBox();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Timer(int value);
GLvoid Mouse(int button, int state, int x, int y);

struct Vertex {
    GLfloat x, y, z;
    glm::vec3 color;
};

int window_Width = 800;
int window_Height = 600;

int startX, startY, endX, endY;

std::vector<int> activeShapes; //(0: 삼각형, 1: 사각형, 2: 오각형, 3: 육각형)
std::vector<glm::vec3> positions;
glm::vec3 velocity(0.01f, -0.001f, 0.0f);
int animationTimer = 0;
const int animationInterval = 180;

GLuint vaoPath, vboPath;
bool drawPath = false;
std::vector<std::vector<glm::vec3>> paths;

glm::vec3 boxPosition(0.0f, -0.8f, 0.0f);
glm::vec3 boxVelocity(0.01f, 0.0f, 0.0f);

glm::vec3 gravity(0.0f, -0.001f, 0.0f); // 중력 가속도
std::vector<glm::vec3> velocities; // 각 도형의 속도

std::vector<GLuint> newPolygonVAOs;
std::vector<std::vector<glm::vec3>> newPolygonVertices; // 새로 생성된 다각형의 정점들
std::vector<glm::vec3> newPositions;

GLfloat zRotation = 0.0f;
glm::vec3 initialPosition(-1.5f, 0.5f, 0.0f);

glm::vec2 ScreenToOpenGLCoords(int x, int y, int windowWidth, int windowHeight) {
    float oglX = 2.0f * x / windowWidth - 1.0f;
    float oglY = 1.0f - 2.0f * y / windowHeight; // Y 축은 반전 필요
    return glm::vec2(oglX, oglY);
}

bool LineIntersection(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& q1, const glm::vec2& q2, glm::vec2& intersection) {
    glm::vec2 r = p2 - p1;
    glm::vec2 s = q2 - q1;
    float rxs = r.x * s.y - r.y * s.x;
    float qpxr = (q1.x - p1.x) * r.y - (q1.y - p1.y) * r.x;

    if (rxs == 0 && qpxr == 0) return false; // collinear
    if (rxs == 0) return false; // parallel

    float t = ((q1.x - p1.x) * s.y - (q1.y - p1.y) * s.x) / rxs;
    float u = ((q1.x - p1.x) * r.y - (q1.y - p1.y) * r.x) / rxs;

    if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
        intersection = p1 + t * r;
        return true;
    }

    return false;
}

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);  // VAO 바인드 해제
}

void InitTriangle() {
    std::vector<Vertex> vertices = {
        {0.0f, 0.133f, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f)},
        {-0.2f, -0.133f, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f)},
        {0.2f, -0.133f, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f)}
    };
    std::vector<unsigned int> indices = { 0, 1, 2 };

    InitBuffer(vaoTriangle, vboTriangle, vertices, indices);
}

void InitRectangle() {
    std::vector<Vertex> vertices = {
        {-0.2f, 0.2f, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f)},
        {0.2f, 0.2f, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f)},
        {0.2f, -0.2f, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f)},
        {-0.2f, -0.2f, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f)}
    };
    std::vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3 };

    InitBuffer(vaoRectangle, vboRectangle, vertices, indices);
}

void InitPentagon() {
    std::vector<Vertex> vertices = {
        {0.0f, 0.133f, 0.0f, glm::vec3(1.0f, 0.4f, 0.7f)},
        {-0.2f, 0.041f, 0.0f, glm::vec3(1.0f, 0.4f, 0.7f)},
        {-0.123f, -0.133f, 0.0f, glm::vec3(1.0f, 0.4f, 0.7f)},
        {0.123f, -0.133f, 0.0f, glm::vec3(1.0f, 0.4f, 0.7f)},
        {0.2f, 0.041f, 0.0f, glm::vec3(1.0f, 0.4f, 0.7f)}
    };
    std::vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3, 0, 3, 4 };

    InitBuffer(vaoPentagon, vboPentagon, vertices, indices);
}

void InitHexagon() {
    std::vector<Vertex> vertices = {
        {-0.1f, 0.173f, 0.0f, glm::vec3(0.0f, 0.0f, 0.5f)},
        {-0.2f, 0.0f, 0.0f, glm::vec3(0.0f, 0.0f, 0.5f)},
        {-0.1f, -0.173f, 0.0f, glm::vec3(0.0f, 0.0f, 0.5f)},
        {0.1f, -0.173f, 0.0f, glm::vec3(0.0f, 0.0f, 0.5f)},
        {0.2f, 0.0f, 0.0f, glm::vec3(0.0f, 0.0f, 0.5f)},
        {0.1f, 0.173f, 0.0f, glm::vec3(0.0f, 0.0f, 0.5f)}
    };
    std::vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 5 };

    InitBuffer(vaoHexagon, vboHexagon, vertices, indices);
}

void InitPathBuffer() {
    glGenVertexArrays(1, &vaoPath);
    glGenBuffers(1, &vboPath);

    glBindVertexArray(vaoPath);
    glBindBuffer(GL_ARRAY_BUFFER, vboPath);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW); // 초기에는 크기를 0으로 설정

    // 위치 속성 (glm::vec3로 구성되어 있으므로 3개의 float)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void InitBox() {
    std::vector<Vertex> vertices = {
        {-0.2f, 0.05f, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f)},
        {0.2f, 0.05f, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f)},
        {0.2f, -0.05f, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f)},
        {-0.2f, -0.05f, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f)}
    };
    std::vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3 };

    InitBuffer(vaoBox, vboBox, vertices, indices);
}

void SelectRandomShape() {
    int shape = std::rand() % 4;
    activeShapes.push_back(shape);
    glm::vec3 initialPosition(-1.5f, 0.5f, 0.0f);
    positions.push_back(initialPosition);

    paths.emplace_back(std::vector<glm::vec3>{initialPosition});
}

void main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(window_Width, window_Height);
    glutCreateWindow("숙제");

    glewExperimental = GL_TRUE;
    glewInit();

    InitTriangle();
    InitRectangle();
    InitPentagon();
    InitHexagon();
    InitPathBuffer();
    InitBox();
    SelectRandomShape();

    make_shaderProgram();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutTimerFunc(16, Timer, 0);
    glutMouseFunc(Mouse);

    glutMainLoop();
}

void CreateNewPolygonVAO(const std::vector<glm::vec3>& vertices) {
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    std::vector<Vertex> coloredVertices;
    for (const auto& vertex : vertices) {
        coloredVertices.push_back({ vertex.x, vertex.y, vertex.z, glm::vec3(0.0f, 0.0f, 1.0f) }); // Blue color
    }

    // VAO 바인딩
    glBindVertexArray(vao);

    // VBO 바인딩 및 데이터 설정
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, coloredVertices.size() * sizeof(Vertex), coloredVertices.data(), GL_DYNAMIC_DRAW);
    // 정점 속성 설정 (예: 위치)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);

    // VAO 바인딩 해제
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    newPolygonVAOs.push_back(vao);
    newPolygonVertices.push_back(vertices);
}

void PerformTriangleSplit(const std::vector<glm::vec2>& triangleVertices, const glm::vec2& start, const glm::vec2& end) {
    std::vector<glm::vec2> intersections; // 교차점 저장
    std::vector<size_t> intersectedEdges; // 교차한 변의 인덱스 저장

    // 삼각형의 각 변과 마우스 드래그 선분의 교차 검사
    for (size_t i = 0; i < 3; ++i) {
        size_t next = (i + 1) % 3; // 삼각형의 다음 변
        glm::vec2 intersection;

        if (LineIntersection(triangleVertices[i], triangleVertices[next], start, end, intersection)) {
            intersections.push_back(intersection);
            intersectedEdges.push_back(i); // 교차한 변의 시작 인덱스 저장
        }
    }

    // 교차점이 두 개일 경우 삼각형을 두 개의 새로운 다각형으로 나눔
    if (intersections.size() == 2 && !intersectedEdges.empty()) {
        // 새로운 다각형 정의
        std::vector<glm::vec2> newTriangle1, newTriangle2;

        // 첫 번째 다각형 생성
        newTriangle1.push_back(triangleVertices[intersectedEdges[0]]);
        newTriangle1.push_back(intersections[0]);
        newTriangle1.push_back(intersections[1]);

        // 두 번째 다각형 생성
        newTriangle2.push_back(intersections[0]);
        newTriangle2.push_back(intersections[1]);
        newTriangle2.push_back(triangleVertices[(intersectedEdges[0] + 1) % 3]);
        newTriangle2.push_back(triangleVertices[(intersectedEdges[1] + 1) % 3]);

        std::vector<glm::vec3> newTriangle1_3D, newTriangle2_3D;
        for (const auto& vertex : newTriangle1) {
            newTriangle1_3D.emplace_back(vertex.x, vertex.y, 0.0f);
        }
        for (const auto& vertex : newTriangle2) {
            newTriangle2_3D.emplace_back(vertex.x, vertex.y, 0.0f);
        }

        glm::vec3 offset(0.01f, 0.01f, 0.0f);
        for (auto& vertex : newTriangle1_3D) {
            vertex += offset;
        }
        for (auto& vertex : newTriangle2_3D) {
            vertex -= offset;
        }

        // 새로운 다각형을 VAO/VBO에 등록하여 그리기
        CreateNewPolygonVAO(newTriangle1_3D);
        CreateNewPolygonVAO(newTriangle2_3D);

        glm::vec3 existingPosition = positions[0];

        // 쪼개진 도형의 위치를 기존 위치로 설정
        newPositions.push_back(existingPosition); // 첫 번째 쪼개진 도형의 위치
        newPositions.push_back(existingPosition); // 두 번째 쪼개진 도형의 위치

        velocities.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        velocities.push_back(glm::vec3(0.0f, 0.0f, 0.0f));

        // 기존 도형 관리 로직
        positions.erase(positions.begin());
        activeShapes.erase(activeShapes.begin());
    }
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
    glClearColor(0.5, 0.8, 1.0, 1.0f);  // 하늘색
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgramID);

    GLint modelLocation = glGetUniformLocation(shaderProgramID, "model");

    for (size_t i = 0; i < activeShapes.size(); ++i) {
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, positions[i]);  // 각 도형의 현재 위치로 변환
        modelMatrix = glm::rotate(modelMatrix, glm::radians(-zRotation), glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &modelMatrix[0][0]);

        // 현재 활성화된 도형을 그리기
        switch (activeShapes[i]) {
        case 0:  // 삼각형
            glBindVertexArray(vaoTriangle);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
            break;
        case 1:  // 사각형
            glBindVertexArray(vaoRectangle);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
            break;
        case 2:  // 오각형
            glBindVertexArray(vaoPentagon);
            glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
            break;
        case 3:  // 육각형
            glBindVertexArray(vaoHexagon);
            glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
            break;
        }
    }

    for (size_t i = 0; i < newPolygonVAOs.size(); ++i) {
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, newPositions[i]);  // 새로 생성된 다각형의 위치로 변환
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &modelMatrix[0][0]);

        glBindVertexArray(newPolygonVAOs[i]);
        glDrawArrays(GL_TRIANGLES, 0, newPolygonVertices[i].size());
        glBindVertexArray(0);
    }

    if (drawPath) {
        std::vector<glm::vec3> pathVertices;
        for (const auto& path : paths) {
            pathVertices.insert(pathVertices.end(), path.begin(), path.end());
        }

        glBindVertexArray(vaoPath);
        glBindBuffer(GL_ARRAY_BUFFER, vboPath);
        glBufferData(GL_ARRAY_BUFFER, pathVertices.size() * sizeof(glm::vec3), pathVertices.data(), GL_DYNAMIC_DRAW);

        GLint modelLocation = glGetUniformLocation(shaderProgramID, "model");
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
        // 경로 그리기
        glDrawArrays(GL_LINE_STRIP, 0, pathVertices.size());

        // VAO 및 VBO 바인드 해제
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    glm::mat4 boxModelMatrix = glm::mat4(1.0f);
    boxModelMatrix = glm::translate(boxModelMatrix, boxPosition); // 박스를 화면 아래로 이동 (필요에 따라 조정 가능)
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &boxModelMatrix[0][0]);

    glBindVertexArray(vaoBox);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

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
    case 'l':
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case 'f':
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    case 'p':
        drawPath = !drawPath;
        break;
    case '+':
        velocity *= 1.2f;
        break;
    case '-':
        velocity *= 0.8f;
        break;
    }
    glutPostRedisplay();
}

GLvoid Timer(int value) {
    zRotation += 1.0f;
    if (zRotation >= 360.0f) {
        zRotation -= 360.0f; // 0~360도 범위로 유지
    }

    if (animationTimer >= animationInterval) {
        SelectRandomShape();
        animationTimer = 0;
    }
    else {
        animationTimer++;
    }

    for (size_t i = 0; i < positions.size();) { // for문을 i++ 없이 사용하여 조건에 따라 제거 가능
        positions[i] += velocity;  // 도형의 이동

        // 중력 적용 (분리된 삼각형에만 적용)
        if (i < velocities.size()) {
            velocities[i] += gravity; // 중력 가속도 적용
            positions[i] += velocities[i]; // 속도에 따른 위치 이동
        }

        // 화면 경계를 벗어나는 경우 도형 제거 (예시로 -1.0 ~ 1.0 범위를 기준으로 설정)
        if (positions[i].x > 1.2f) {
            if (i < activeShapes.size()) activeShapes.erase(activeShapes.begin() + i);
            if (i < positions.size()) positions.erase(positions.begin() + i);
            if (!paths.empty() && i < paths.size()) paths.erase(paths.begin() + i);
            if (!velocities.empty() && i < velocities.size()) velocities.erase(velocities.begin() + i);

            continue;
        }

        // paths 벡터가 활성화된 경우 경로를 추가
        if (drawPath && i < paths.size()) {
            paths[i].push_back(positions[i]);
        }

        ++i; // 다음 도형으로 이동
    }

    // 박스의 이동 및 경계 검사 (예시)
    boxPosition += boxVelocity;
    if (boxPosition.x > 0.85f || boxPosition.x < -0.85f) {
        boxVelocity.x = -boxVelocity.x;  // 화면 경계를 넘으면 속도 반전
    }

    glutPostRedisplay();
    glutTimerFunc(16, Timer, 0);
}

GLvoid Mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // 마우스 클릭 시, 드래그 시작 지점 저장
        startX = x;
        startY = y;
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        // 마우스 버튼을 놓았을 때, 드래그 종료 지점 저장
        endX = x;
        endY = y;

        glm::vec2 start = ScreenToOpenGLCoords(startX, startY, window_Width, window_Height);
        glm::vec2 end = ScreenToOpenGLCoords(endX, endY, window_Width, window_Height);

        if (!positions.empty()) {
            glm::vec3 currentPosition = positions[0]; // 삼각형의 현재 위치
            std::vector<glm::vec2> baseTriangleVertices = {
                glm::vec2(0.0f, 0.133f),
                glm::vec2(-0.2f, -0.133f),
                glm::vec2(0.2f, -0.133f)
            };

            std::vector<glm::vec2> triangleVertices;
            for (const auto& vertex : baseTriangleVertices) {
                glm::vec2 transformedVertex = glm::vec2(vertex.x + currentPosition.x, vertex.y + currentPosition.y);
                triangleVertices.push_back(transformedVertex);
            }
            // 드래그된 선을 기준으로 도형을 자르기
            PerformTriangleSplit(triangleVertices, start, end);

            glutPostRedisplay();
        }
        else {
            std::cout << "Error : No triangles available for splitting." << std::endl;
            return;
        }
    }
}

void RegisterMouseCallbacks() {
    glutMouseFunc(Mouse);
}