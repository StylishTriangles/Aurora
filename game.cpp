#include "game.h"
#include <QDebug>


Game::Game(QWidget *parent) :
    QOpenGLWidget(parent),
    shadersCompiled(false),
    camXRot(0), camYRot(0), camZRot(0),
    viewDist(-2.0f)
{
    this->setMinimumSize(100, 100);
    this->resize(parent->size());
    this->setFocus();
}

Game::~Game()
{
    delete sProgram;
    planetsProgram->release();
    delete planetsProgram;
    // temp
    delete tex;
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void Game::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != camXRot) {
        camXRot = angle;
        update();
    }
}

void Game::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != camYRot) {
        camYRot = angle;
        update();
    }
}

void Game::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != camZRot) {
        camZRot = angle;
        update();
    }
}

void Game::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);

    Vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&Vao);
    planetsProgram = new QOpenGLShaderProgram;
    sProgram = new QOpenGLShaderProgram;

    projectionMat.setToIdentity();
    projectionMat.perspective(45.0f, float(this->width()) / float(this->height()), 0.01f, 100.0f);
//    projectionMat.frustum(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f); // orthographic projection mode

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
    if (!shadersCompiled)
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
        shadersCompiled = true;
        // temp
        tex = new QOpenGLTexture(QImage(QString(":/planets/oceaniczna.png")));
    }
    QMatrix4x4 modelMat;
    modelMat.setToIdentity();
    modelMat.translate(0.0f,0.0f,0.0f);
    GLsphere ocean;
    ocean.create(radius);
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
//    delete ocean.getTexture();
}

void Game::setupVBOAttribute(){
    Vbo.bind();
    this->glEnableVertexAttribArray(0);
    this->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    Vbo.release();
}

void Game::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    viewMat.setToIdentity();
    viewMat.translate(0.0f, 0.0f, viewDist);
    viewMat.rotate(camXRot/16.0f, 1.0f, .0f, .0f);
    viewMat.rotate(camYRot/16.0f, .0f, 1.0f, .0f);
    viewMat.rotate(camZRot/16.0f, .0f, .0f, 1.0f);

    QOpenGLVertexArrayObject::Binder vaoBinder(&Vao);
    drawSphere(0.5f);
}

void Game::mousePressEvent(QMouseEvent *event)
{
    lastCursorPos = event->pos();
}

void Game::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastCursorPos.x();
    int dy = event->y() - lastCursorPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(camXRot + 8 * dy);
        setYRotation(camYRot + 8 * dx);
    }
//    else if (event->buttons() & Qt::RightButton) { // Hey I just met you
//        setXRotation(m_xRot + 8 * dy);                and this is crazy
//        setZRotation(m_zRot + 8 * dx);                here is my event
//    }                                                 so fix me maybe
    lastCursorPos = event->pos();
}

void Game::wheelEvent(QWheelEvent *event)
{
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta();
    if (!numPixels.isNull())
    {
        viewDist += float(numPixels.y())/5000.0f;
    }
    else
    {
        viewDist += float(numDegrees.y())/5000.0f;
    }
    event->accept();
    this->update();
}

void Game::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        emit exitToMenu();
    }
    else if (event->key() == Qt::Key_W)
    {

    }
    else
        event->ignore();
    event->accept();
}
