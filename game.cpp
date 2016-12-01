#include "game.h"
#include <QDebug>
#include <QtMath>
#include <QResource>

Game::Game(QWidget *parent) :
    QOpenGLWidget(parent),
    shadersCompiled(false),
    camXRot(0), camYRot(-90.0f), camZRot(0)
{
    QSurfaceFormat format;
    format.setSamples(4);
    format.setAlphaBufferSize(8);
    setFormat(format);
    this->setMinimumSize(100, 100);
    this->resize(parent->size());
    this->setFocus();
}

Game::~Game()
{
    planetsProgram->release();
    delete planetsProgram;
    // temp
    delete tex;
    delete skyboxTexture;
}

inline void qNormalizeAngle(float& a)
{
    if (a > 360.0f)
        a -= 360.0f;
    else if (a < 0.0f)
        a += 360.0f;
}

void Game::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(.0f, .0f, .0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glEnable(GL_MULTISAMPLE);

    Vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&Vao);
    planetsProgram = new QOpenGLShaderProgram;

    projectionMat.setToIdentity();
    projectionMat.perspective(45.0f, float(this->width()) / float(this->height()), 0.01f, 100.0f);
//    projectionMat.frustum(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f); // orthographic projection mode

    planetVbo.create();

    camPos   = QVector3D(0.0f, 0.0f,  4.5f);
    camFront = QVector3D(0.0f, 0.0f, -1.0f);
    camUp    = QVector3D(0.0f, 1.0f,  0.0f);
    camSpeed = 0.05f;
    rotationSpeed = 0.1f;

    // load textures
    skyboxTexture = new QOpenGLTexture(QImage(QString(":/misc/skybox.png")));

    // initialize models
//    GeometryProvider::geosphere(geosphereModel, GeometryProvider::Four, 5, 0, 3);
    GeometryProvider::geosphere(geosphereModel, GeometryProvider::Four, 5, 0, 3);
    GeometryProvider::titan(titanModel, GeometryProvider::Two, 5, 0, 3, -1, 420);
    // compile shaders
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
#ifdef QT_DEBUG
    atmo = new QOpenGLTexture(QImage(QString("../Aurora/atmosphere.png")));
#else
    QResource::registerResource("textures/textures.rcc");
    atmo = new QOpenGLTexture(QImage(QString(":/planets/atmosphere.png")));
#endif
}

void Game::resizeGL(int w, int h)
{
    projectionMat.setToIdentity();
    projectionMat.perspective(45.0f, w / float(h), 0.01f, 100.0f);
}

void Game::drawSphere(float radius, float x, float y, float z)
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
    modelMat.translate(x, y, z);
    modelMat.scale(radius);
    QVector<GLfloat> ocean;
    GeometryProvider::sphere(ocean, 12, 12);

    planetVbo.bind();
    planetVbo.allocate(ocean.constData(),ocean.count()*sizeof(GLfloat));

    planetsProgram->setUniformValue("model",modelMat);
    planetsProgram->setUniformValue("view",viewMat);
    planetsProgram->setUniformValue("projection",projectionMat);

    this->glEnableVertexAttribArray(0);
    this->glEnableVertexAttribArray(2);
    this->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    this->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(3*sizeof(GLfloat)));


    tex->bind();
    glDrawArrays(GL_TRIANGLES, 0, ocean.count());
    planetVbo.release();
//    delete ocean.getTexture();
}

void Game::drawGeosphere(float x, float y, float z, float r)
{
    drawGeosphere(geosphereModel, x, y, z, r);
}

void Game::drawGeosphere(QVector<GLfloat> const& modelGeometry, float x, float y, float z, float r)
{
    QMatrix4x4 modelMat;
    modelMat.setToIdentity();
    modelMat.translate(x, y, z);
    modelMat.scale(r);

    planetVbo.bind();
    planetVbo.allocate(modelGeometry.data(),modelGeometry.size()*sizeof(GLfloat));

    planetsProgram->setUniformValue("model",modelMat);
    planetsProgram->setUniformValue("view",viewMat);
    planetsProgram->setUniformValue("projection",projectionMat);

    this->glEnableVertexAttribArray(0);
    this->glEnableVertexAttribArray(2);
    this->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    this->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(3*sizeof(GLfloat)));

    glDrawArrays(GL_TRIANGLES, 0, modelGeometry.count());
    planetVbo.release();
}

void Game::drawSkybox(float radius)
{
    QMatrix4x4 modelMat;
    modelMat.setToIdentity();
    modelMat.translate(camPos);
    modelMat.scale(radius);

    planetVbo.bind();
    planetVbo.allocate(geosphereModel.data(),geosphereModel.size()*sizeof(GLfloat));

    planetsProgram->setUniformValue("model",modelMat);
    planetsProgram->setUniformValue("view",viewMat);
    planetsProgram->setUniformValue("projection",projectionMat);

    this->glEnableVertexAttribArray(0);
    this->glEnableVertexAttribArray(2);
    this->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    this->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(3*sizeof(GLfloat)));

    skyboxTexture->bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glDrawArrays(GL_TRIANGLES, 0, geosphereModel.count());
    planetVbo.release();
}

void Game::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    viewMat.setToIdentity();
    viewMat.lookAt(camPos, camPos + camFront, camUp);

    QOpenGLVertexArrayObject::Binder vaoBinder(&Vao);

//    drawSphere(1.0f, 0.0f, 0.0f, 0.0f);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    tex->bind();
    drawGeosphere(0.0f, 0.0f, 0.0f);
    tex->release();
    atmo->bind();
    drawGeosphere(0.0f, 0.0f, 0.0f, 1.02f);
    atmo->release();
    // EXPERIMENTAL
    tex->bind();
    drawGeosphere(titanModel, 2.0f, 2.0f, -2.0f);
    tex->release();
    //drawSphere(0.5f, sinf(qDegreesToRadians(float((timer.elapsed()-oldTime)/50))), cosf(qDegreesToRadians(float((timer.elapsed()-oldTime)/50))), 0.0f);
    //oldTime=timer.elapsed();
//    drawSphere(2.0f, 5.0f, 3.0f, 0.0f);
    drawSkybox(50.0f);
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
        camXRot = camXRot + rotationSpeed * dy;
        camYRot = camYRot + rotationSpeed * dx;
        qNormalizeAngle(camYRot);
        if (camXRot > 89.8f)
            camXRot = 89.8f;
        else if (camXRot < -89.8f)
            camXRot = -89.8f;
    }
//    else if (event->buttons() & Qt::RightButton) { // Hey I just met you
//        setXRotation(m_xRot + 8 * dy);                and this is crazy
//        setZRotation(m_zRot + 8 * dx);                here is my event
//    }                                                 so fix it maybe
    lastCursorPos = event->pos();
    QVector3D front;
    front.setX(cosf(qDegreesToRadians(camYRot)) * cosf(qDegreesToRadians(camXRot)));
    front.setY(sinf(qDegreesToRadians(camXRot)));
    front.setZ(sinf(qDegreesToRadians(camYRot)) * cosf(qDegreesToRadians(camXRot)));
    camFront = front.normalized();
}

void Game::wheelEvent(QWheelEvent *event)
{
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta();
    if (!numPixels.isNull())
    {
        camPos += camSpeed*float(numPixels.y()) * camFront;
    }
    else
    {
        camPos += camSpeed*float(numDegrees.y())/100.0f * camFront;
    }
    event->accept();
}

void Game::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        event->accept();
        emit exitToMenu();
        return;
    }
    if (event->key() == Qt::Key_W)
        keys += Qt::Key_W;
    if (event->key() == Qt::Key_S)
        keys += Qt::Key_S;
    if (event->key() == Qt::Key_D)
        keys += Qt::Key_D;
    if (event->key() == Qt::Key_A)
        keys += Qt::Key_A;
    if (event->key() == Qt::Key_Shift)
        keys += Qt::Key_Shift;
    if (event->key() == Qt::Key_R)
    {
        camPos   = QVector3D(0.0f, 0.0f,  4.5f);
        camFront = QVector3D(0.0f, 0.0f, -1.0f);
        camUp    = QVector3D(0.0f, 1.0f,  0.0f);
        camXRot=0;
        camYRot=-90.0f;
        camZRot=0;
    }
}

void Game::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_W)
        keys -= Qt::Key_W;
    if (event->key() == Qt::Key_S)
        keys -= Qt::Key_S;
    if (event->key() == Qt::Key_D)
        keys -= Qt::Key_D;
    if (event->key() == Qt::Key_A)
        keys -= Qt::Key_A;
    if (event->key() == Qt::Key_Shift)
        keys -= Qt::Key_Shift;
}

void Game::parseInput(float dT)
{
    float camV = camSpeed * dT;
    if (keys.find(Qt::Key_W) != keys.end())
        camPos += camV * camFront;
    if (keys.find(Qt::Key_S) != keys.end())
        camPos -= camV * camFront;
    if (keys.find(Qt::Key_D) != keys.end())
        camPos += QVector3D::normal(camFront, camUp) * camV;
    if (keys.find(Qt::Key_A) != keys.end())
        camPos -= QVector3D::normal(camFront, camUp) * camV;
    if (keys.find(Qt::Key_Shift) != keys.end())
        camSpeed = 0.1f; // default = 0.05
    else
        camSpeed = 0.05f;
}

void GameWorker::onTick()
{
    qint64 nse = et.nsecsElapsed();
    float dT = (float)(nse-lastTime)/10e8f;
    lastTime = nse;
    g->parseInput(dT*25.0f);
    emit frameReady();
}
