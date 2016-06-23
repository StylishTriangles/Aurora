#include "game.h"
#include <QOpenGLFunctions>

Game::Game(QWidget *parent) :
    QOpenGLWidget(parent)
{
    this->setMinimumSize(100, 100);
}

Game::~Game()
{
}

void Game::initializeGL()
{
    initializeOpenGLFunctions();
    mainCamera.setToIdentity();
    mainCamera.translate(0, 0, -1);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

}

void Game::resizeGL(int w, int h)
{
    // Update projection matrix and other size related settings:
    mainCamera.setToIdentity();
    mainCamera.perspective(45.0f, w / float(h), 0.01f, 100.0f);
}

static const char* vertexShaderSource = "\
#version 330\
layout (location = 0) in vec3 position;\
void main()\
{\
    gl_Position = vec4(position.x, position.y, position.z, 1.0);\
}";

static const char* fragmentShaderSource = "\
#version 330\
out vec4 color;\
void main()\
{\
    color = vec4(0.0f, 0.0f, 0.0f, 1.0f);\
}";

void Game::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };
    GLint success;
    GLchar infoLog[512];

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    GLuint vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        qDebug() << infoLog;
    }
    GLuint fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    GLuint shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    //glLinkProgram(shaderProgram);
    //glUseProgram(shaderProgram);
    //glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    std::this_thread::sleep_for(std::chrono::seconds(5));
}


