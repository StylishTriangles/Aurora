#include "game.h"


Game::Game(QWidget *parent) :
    QOpenGLWidget(parent)
{
    this->setMinimumSize(100, 100);
}

Game::~Game()
{
    delete sProgram;
    delete planetsProgram;
}

void Game::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    Vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&Vao);
    planetsProgram = new QOpenGLShaderProgram;

    projectionMat.setToIdentity();
    projectionMat.perspective(45.0f, float(this->width()) / float(this->height()), 0.01f, 100.0f);
    viewMat.setToIdentity();
    viewMat.translate(0.0f, 0.0f, -8.0f);

    planetVbo.create();


    // temp
}

void Game::resizeGL(int w, int h)
{
    projectionMat.setToIdentity();
    projectionMat.perspective(45.0f, w / float(h), 0.01f, 100.0f);
}

void Game::drawTriangle(){
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
    sProgram->release();
}

void Game::drawCircle(float cx, float cy, float scale){
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
    sProgram->release();
}

void Game::drawSphere(float radius)
{
    // compile shaders
    static bool shadersLoaded = false;
    if (!shadersLoaded)
    {
        bool noerror = true;
        noerror = planetsProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/sphereshaderV.vert");
        if (!noerror) {qDebug() << planetsProgram->log();}
        noerror = planetsProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/sphereshader.frag");
        if (!noerror) {qDebug() << planetsProgram->log();}
        planetsProgram->bindAttributeLocation("position", 0);
        planetsProgram->bindAttributeLocation("texCoord", 2);
        noerror = planetsProgram->link();
        if (!noerror) {qDebug() << planetsProgram->log();}
        noerror = planetsProgram->bind();
        if (!noerror) {qDebug() << planetsProgram->log();}
        planetsProgram->setUniformValue("ourTexture1", 0);
        shadersLoaded = true;
    }
    QMatrix4x4 modelMat;
    modelMat.setToIdentity();
    modelMat.translate(0.0f,0.0f,0.0f);
    GLsphere ocean;
    ocean.create(radius);
    QOpenGLTexture* tex = new QOpenGLTexture(QImage(QString(":/planets/oceaniczna.png")));
    ocean.setTexture(tex);

    planetVbo.bind();
    planetVbo.allocate(ocean.constData(),ocean.count()*sizeof(GLfloat));
    for (int i = 0; i < ocean.count(); i++)
    {
        if (ocean.constData()[i] > 1.0f or ocean.constData()[i] < -1.0f)
            qDebug() << i << ocean.constData()[i];
    }

    planetsProgram->setUniformValue("model",modelMat);
    planetsProgram->setUniformValue("view",viewMat);
    planetsProgram->setUniformValue("projection",projectionMat);
    this->glEnableVertexAttribArray(0);
    this->glEnableVertexAttribArray(2);
    this->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    this->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(3*sizeof(GLfloat)));


    ocean.getTexture()->bind();
    glDrawArrays(GL_TRIANGLES, 0, ocean.count());
    planetVbo.release();
    delete ocean.getTexture();
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

//    drawCircle(0.0f, 0.0f, 1.0f);
    drawSphere(0.5f);
}
