#include "game.h"


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
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    Vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&Vao);

    Vbo.create();
    setupVBOAttribute();
}

void Game::resizeGL(int w, int h)
{
    mainCamera.setToIdentity();
    mainCamera.perspective(45.0f, w / float(h), 0.01f, 100.0f);
}

void Game::drawTriangle(){
    sProgram = new QOpenGLShaderProgram;
    bool noerror = true;
    noerror = sProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vertShader.vert");
    if (!noerror) {qDebug() << sProgram->log();}
    noerror = sProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fragShader.frag");
    if (!noerror) {qDebug() << sProgram->log();}
    sProgram->bindAttributeLocation("position", 0);
    noerror = sProgram->link();
    if (!noerror) {qDebug() << sProgram->log();}
    noerror = sProgram->bind();
    if (!noerror) {qDebug() << sProgram->log();}

    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };

    Vbo.bind();
    Vbo.allocate(vertices, sizeof(vertices));

    glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices)/sizeof(GLfloat));

    Vbo.release();
}

void Game::drawCircle(float cx, float cy, float scale){
    sProgram = new QOpenGLShaderProgram;
    bool noerror = true;
    noerror = sProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vertShader.vert");
    if (!noerror) {qDebug() << sProgram->log();}
    noerror = sProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fragShader.frag");
    if (!noerror) {qDebug() << sProgram->log();}
    sProgram->bindAttributeLocation("position", 0);
    noerror = sProgram->link();
    if (!noerror) {qDebug() << sProgram->log();}
    noerror = sProgram->bind();
    if (!noerror) {qDebug() << sProgram->log();}

    QVector<GLfloat> vertices;
    for(float i=0.0f; i<=2*M_PI; i+=(M_PI/360)){
        vertices.push_back(cx);
        vertices.push_back(cy);
        vertices.push_back(0.0f);

        vertices.push_back(fmaf(sinf(i), scale, cx));
        vertices.push_back(fmaf(cosf(i), scale, cy));
        vertices.push_back(0.0f);

        vertices.push_back(fmaf(sinf(i+M_PI/360), scale, cx));
        vertices.push_back(fmaf(cosf(i+M_PI/360), scale, cy));
        vertices.push_back(0.0f);
    }
    Vbo.bind();
    Vbo.allocate(vertices.data(), vertices.size() * sizeof(GLfloat));

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    Vbo.release();
}

void Game::setupVBOAttribute(){
    Vbo.bind();
    this->glEnableVertexAttribArray(0);
    this->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    Vbo.release();
}

void Game::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    QOpenGLVertexArrayObject::Binder vaoBinder(&Vao);

    drawCircle(0.0f, 0.0f, 1.0f);
}
