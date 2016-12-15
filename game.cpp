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
    qDebug() << "ModelContainer:" << sizeof(ModelContainer) << "|" << "Game:" << sizeof(Game);
}

Game::~Game()
{
    planetsProgram->release();
    for(auto p:mGeometry)
    {
        delete p;
    }
    for(auto p:obj)
        delete p;
    delete planetsProgram;
    // temp
    delete tex;
    delete skyboxTexture;
    delete atmo;
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
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);

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
    QVector<GLfloat>* tmp;
    for (int i = 1; i <= 4; i++)
    {
        tmp = new QVector<GLfloat>;
        GeometryProvider::geosphere(*tmp, GeometryProvider::SubdivisionCount(i), 5, 0, 3);
        mGeometry[QStringLiteral("geosphere%1").arg(i)]=tmp;
    }
    for (int i = 1; i <= 4; i++)
    {
        tmp=new QVector<GLfloat>;
        GeometryProvider::titan(*tmp, GeometryProvider::SubdivisionCount(i+1), 5, 0, 3, -1, 420);
        mGeometry[QStringLiteral("titan%1").arg(i)]=tmp;
    }

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
    tex = new QOpenGLTexture(QImage(QString(":/planets/earth.png")));
    obj.push_back(new ModelContainer(QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 0.0f, 0.0f), "geosphere", tex, planetsProgram, ModelContainer::Planet));
#ifdef QT_DEBUG
    atmo = new QOpenGLTexture(QImage(QString("../Aurora/atmosphere.png")));
#else
    QResource::registerResource("textures/textures.rcc");
    atmo = new QOpenGLTexture(QImage(QString(":/planets/atmosphere.png")));
#endif
    obj.push_back(new ModelContainer(QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 0.0f, 0.0f), "geosphere", atmo, planetsProgram, ModelContainer::Planet));
    obj.back()->scale=1.02f;
    obj.push_back(new ModelContainer(QVector3D(2.0f, 2.0f, 2.0f), QVector3D(0.0f, 0.0f, 0.0f), "titan", tex, planetsProgram, ModelContainer::Planet));
}

void Game::resizeGL(int w, int h)
{
    projectionMat.setToIdentity();
    projectionMat.perspective(45.0f, w / float(h), 0.01f, 100.0f);
}

inline QVector<GLfloat>* Game::getModel(QString const & name, int detail)
{
    auto ret = mGeometry.find(name + QString::number(detail));
    Q_ASSERT_X(ret != mGeometry.end(), "Game::getModel()", (const char*)(name + QString::number(detail)).data());
    return *ret;
}

void Game::drawModel(ModelContainer* mod)
{
    QMatrix4x4 modelMat;
    modelMat.setToIdentity();
    modelMat.translate(mod->pos);
    modelMat.scale(mod->scale);

    planetVbo.bind();
    static auto distance = [](QVector3D const& camPos, QVector3D const& modPos) -> float {
        return (camPos-modPos).length();
    };
    float d = distance(camPos, mod->pos);
    int detailLevel = (d > 60.f)?1:(d > 30.f)?2:(d > 15.f)?3:4;
    planetVbo.allocate(getModel(mod->model,detailLevel)->data(),getModel(mod->model,detailLevel)->size()*sizeof(GLfloat));

    planetsProgram->setUniformValue("model",modelMat);
    planetsProgram->setUniformValue("view",viewMat);
    planetsProgram->setUniformValue("projection",projectionMat);

    this->glEnableVertexAttribArray(0);
    this->glEnableVertexAttribArray(2);
    this->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    this->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(3*sizeof(GLfloat)));

    glDrawArrays(GL_TRIANGLES, 0, getModel(mod->model,detailLevel)->count());
    planetVbo.release();
    for (ModelContainer* m:mod->children)
        drawModel(m);
}

void Game::drawSkybox(float radius)
{
    QMatrix4x4 modelMat;
    modelMat.setToIdentity();
    modelMat.translate(camPos);
    modelMat.scale(radius);

    QVector<GLfloat>* geosphereModel=*mGeometry.find("geosphere4");
    planetVbo.bind();
    planetVbo.allocate(geosphereModel->data(),geosphereModel->size()*sizeof(GLfloat));

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
    glDrawArrays(GL_TRIANGLES, 0, geosphereModel->count());
    planetVbo.release();
}

void Game::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    viewMat.setToIdentity();
    viewMat.lookAt(camPos, camPos + camFront, camUp);

    QOpenGLVertexArrayObject::Binder vaoBinder(&Vao);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    for(int i=0; i<obj.size(); i++){
        obj[i]->tex->bind();
        drawModel(obj[i]);
        obj[i]->tex->release();
    }
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
