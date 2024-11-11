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

GLuint vaoTriangle, vaoRectangle, vaoPentagon, vaoHexagon, vaoBox;
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

int window_Width = 800;
int window_Height = 600;

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

struct Vertex {
    GLfloat x, y, z;
    glm::vec3 color;
};

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
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(window_Width, window_Height);
    glutCreateWindow("숙제");

    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEPTH_TEST);

    InitTriangle();
    InitRectangle();
    InitPentagon();
    InitHexagon();
    InitPathBuffer();
    InitBox();

    make_shaderProgram();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutTimerFunc(16, Timer, 0);

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
    glClearColor(0.5, 0.8, 1.0, 1.0f);  // 하늘색
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgramID);

    GLint modelLocation = glGetUniformLocation(shaderProgramID, "model");

    for (size_t i = 0; i < activeShapes.size(); ++i) {
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, positions[i]);  // 각 도형의 현재 위치로 변환
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
    if (animationTimer >= animationInterval) {
        SelectRandomShape();
        animationTimer = 0;
    }
    else {
        animationTimer++;
    }

    for (size_t i = 0; i < positions.size(); ++i) {
        positions[i] += velocity;  // 도형의 이동

        if (drawPath) {
            paths[i].push_back(positions[i]);
        }
    }

    boxPosition += boxVelocity;
    if (boxPosition.x > 0.85f || boxPosition.x < -0.85f) {
        boxVelocity.x = -boxVelocity.x;  // 화면 경계를 넘으면 속도 반전
    }

    glutPostRedisplay();
    glutTimerFunc(16, Timer, 0);
}