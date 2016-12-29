#include "game.h"
#include <QDebug>
#include <QtMath>
#include <QResource>
#include <random>

Game::Game(QWidget *parent) :
    QOpenGLWidget(parent),
    shadersCompiled(false), initComplete(false),
    camXRot(0), camYRot(-90.0f), camZRot(0),
    camFov(60.0f), camNear(0.01f), camFar(100.0f)
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
    delete lightsProgram;
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
    lightsProgram = new QOpenGLShaderProgram;

    projectionMat.setToIdentity();
    projectionMat.perspective(camFov, float(this->width()) / float(this->height()), camNear, camFar);
//    projectionMat.frustum(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f); // orthographic projection mode

    planetVbo.create();

    camPos   = QVector3D(0.0f, 0.0f,  4.5f);
    camFront = QVector3D(0.0f, 0.0f, -1.0f);
    camUp    = QVector3D(0.0f, 1.0f,  0.0f);
    camSpeed = 0.05f;
    rotationSpeed = 0.1f;

    // initialize models
    QVector<GLfloat>* tmp;
    for (int i = 1; i <= 4; i++)
    {
        tmp = new QVector<GLfloat>;
        GeometryProvider::geosphere(*tmp, GeometryProvider::SubdivisionCount(i));
        mGeometry[QStringLiteral("geosphere%1").arg(i)]=tmp;
    }
    for (int i = 3; i <= 3; i++)
    {
        tmp=new QVector<GLfloat>;
        GeometryProvider::titan(*tmp, GeometryProvider::SubdivisionCount(i), 420);
        mGeometry[QStringLiteral("titan%1").arg(i)]=tmp;
    }

    // load and compile shaders
    loadShaders();

    // load textures
    loadTextures();

    // make models
    obj.push_back(new ModelContainer(QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 0.0f, 0.0f), "geosphere", "earth", planetsProgram, ModelContainer::Planet));
    ModelContainer* tmpM = new ModelContainer(QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 0.0f, 0.0f), "geosphere", "atmosphere", planetsProgram, ModelContainer::Planet);
    tmpM->scale=1.02f;
    obj.back()->addChild(tmpM); // add atmosphere to earth
    tmpM = new ModelContainer(QVector3D(0.0f, 0.0f, 2.1f), QVector3D(360.0f, 0.0f, 0.0f), "geosphere", "moon", planetsProgram, ModelContainer::Moon);
    tmpM->scale=0.250f;
    obj.back()->addChild(tmpM); // add moon to earth
    obj.push_back(new ModelContainer(QVector3D(2.0f, 2.0f, 2.0f), QVector3D(0.0f, 0.0f, 0.0f), "titan", "earth", planetsProgram, ModelContainer::Titan));
    tmpM = new ModelContainer(QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 0.0f, 0.0f), "titan", "atmosphere", planetsProgram, ModelContainer::Titan);
    tmpM->scale=1.01f;
    obj.back()->addChild(tmpM); // add atmosphere to titan
    obj.push_back(new ModelContainer({0,0,0}, {0,0,0}, "geosphere", "skybox", planetsProgram, ModelContainer::Skybox));
    obj.back()->scale=50.0f;

    // test
    std::mt19937 r(420);
    for (int i = 0; i < 600; i++) {
        obj.push_back(new ModelContainer(QVector3D((float)r()*6/r.max()-3, (float)r()*6/r.max()-3, (float)r()*6/r.max()-3), QVector3D(0.0f, 0.0f, 0.0f), "geosphere", "earth", planetsProgram, ModelContainer::Planet));
        obj.back()->scale=0.25f;
    }
    initComplete = true;
}

void Game::resizeGL(int w, int h)
{
    projectionMat.setToIdentity();
    projectionMat.perspective(camFov, w / float(h), camNear, camFar);
}

inline QVector<GLfloat>* Game::getModel(QString const & name, int detail)
{
    auto ret = mGeometry.find(name + QString::number(detail));
    Q_ASSERT_X(ret != mGeometry.end(), "Game::getModel()", (const char*)(name + QString::number(detail)).data());
    return *ret;
}

void Game::drawModel(ModelContainer* mod)
{
    if (mod->t != ModelContainer::Planet)
        return;

    QMatrix4x4 modelMat;
    modelMat.setToIdentity();
    modelMat.translate(mod->getPos());
    modelMat.scale(mod->getScale());
    QMatrix4x4 vp = projectionMat * viewMat;
    textures[mod->tex]->bind();
    planetVbo.bind();
    static auto distance = [](QVector3D const& camPos, QVector3D const& modPos) -> float {
        return (camPos-modPos).length();
    };
    float d = distance(camPos, mod->pos) / mod->getScale();
    int detailLevel = (d > 60.f)?1:(d > 30.f)?2:(d > 15.f)?3:4;
    if (mod->t == ModelContainer::Skybox or mod->t == ModelContainer::Titan)
        detailLevel = 3;
    static auto allocated = false;
    if (!allocated)
        planetVbo.allocate(getModel(mod->model,detailLevel)->data(),getModel(mod->model,detailLevel)->size()*sizeof(GLfloat));

    // git gud
    if (mod->t == ModelContainer::Star) {
        lightsProgram->bind();
        lightsProgram->setUniformValue("vp",vp);
        lightsProgram->setUniformValue("modelMat",modelMat);
    }
    else {
        planetsProgram->bind();
        planetsProgram->setUniformValue("vp",vp);
        planetsProgram->setUniformValue("modelMat",modelMat);
    }

    this->glEnableVertexAttribArray(0);
    this->glEnableVertexAttribArray(1);
    this->glEnableVertexAttribArray(2);
    this->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    this->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
    this->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6*sizeof(GLfloat)));

    glDrawArrays(GL_TRIANGLES, 0, getModel(mod->model,detailLevel)->count());
    planetVbo.release();
    textures[mod->tex]->release();
    for (int i = 0; i < mod->children.size(); i++)
        drawModel(mod->children[i]);
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
        drawModel(obj[i]);
    }
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
    if (event->key() == Qt::Key_Q)
        keys += Qt::Key_Q;
    if (event->key() == Qt::Key_E)
        keys += Qt::Key_E;
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
    if (event->key() == Qt::Key_Q)
        keys -= Qt::Key_Q;
    if (event->key() == Qt::Key_E)
        keys -= Qt::Key_E;
    if (event->key() == Qt::Key_Shift)
        keys -= Qt::Key_Shift;
}

void Game::loadTextures()
{
    QOpenGLTexture* tex;
    tex = new QOpenGLTexture(QImage(QString(":/planets/earth.png")));
    textures.insert("earth", tex);

#ifdef QT_DEBUG
    tex = new QOpenGLTexture(QImage(QString("../Aurora/atmosphere.png")));
    textures.insert("atmosphere", tex);
    tex = new QOpenGLTexture(QImage(QString("../Aurora/moon.png")));
    textures.insert("moon", tex);
    tex= new QOpenGLTexture(QImage(QString(":/misc/skybox.png")));
    textures.insert("skybox", tex);
#else
    QResource::registerResource("textures/textures.rcc");
    tex = new QOpenGLTexture(QImage(QString(":/planets/atmosphere.png")));
    textures.insert("atmosphere", tex);
#endif
}

void Game::loadShaders()
{
    // planet shader
    bool noerror = true;
    noerror = planetsProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/sphereshaderV.vert");
    if (!noerror) {qDebug() << planetsProgram->log();}
    noerror = planetsProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/sphereshader.frag");
    if (!noerror) {qDebug() << planetsProgram->log();}
    planetsProgram->bindAttributeLocation("position", 0);
    planetsProgram->bindAttributeLocation("normal", 1);
    planetsProgram->bindAttributeLocation("texCoord", 2);
    noerror = planetsProgram->link();
    if (!noerror) {qDebug() << planetsProgram->log();}
    planetsProgram->setUniformValue("diffuseMap", 0);
    shadersCompiled = true;
    if (!noerror) {qDebug() << planetsProgram->log();}

    // light source shader
    noerror = true;
    noerror = lightsProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/lightVertex.vert");
    noerror = lightsProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/lightFragment.frag");
    lightsProgram->bindAttributeLocation("position", 0);
    lightsProgram->bindAttributeLocation("normal", 1);
    lightsProgram->bindAttributeLocation("texCoord", 2);
    noerror = lightsProgram->link();
    lightsProgram->setUniformValue("tex", 0);
    shadersCompiled = true;
    if (!noerror) {qDebug() << lightsProgram->log();}
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
        camSpeed = 0.1f;
    else
        camSpeed = 0.05f; // default
    // roll
    float dx = 0.0f;
    if (keys.find(Qt::Key_Q) != keys.end())
        dx -= dT;
    if (keys.find(Qt::Key_E) != keys.end())
        dx += dT;
    if (dx != 0.0f)
    {
        camYRot = camYRot + rotationSpeed * dx;
        qNormalizeAngle(camYRot);
        if (camXRot > 89.8f)
            camXRot = 89.8f;
        else if (camXRot < -89.8f)
            camXRot = -89.8f;
        QVector3D front;
        front.setX(cosf(qDegreesToRadians(camYRot)) * cosf(qDegreesToRadians(camXRot)));
        front.setY(sinf(qDegreesToRadians(camXRot)));
        front.setZ(sinf(qDegreesToRadians(camYRot)) * cosf(qDegreesToRadians(camXRot)));
        camFront = front.normalized();
    }
}

// GameWorker::
void GameWorker::onTick()
{
    static int cnt = 0;
    qint64 nse = et.nsecsElapsed();
    float dT = (float)(nse-lastTime)/1e9f;
    if (nse%1000000000LL < 8100000LL) {
        qDebug() << "FPS:" << cnt;
        cnt=0;
    }
    else
        cnt++;
    lastTime = nse;
    g->parseInput(dT*25.0f);
    // gaym logic
    if (!g->initComplete)
        return;
    g->obj[0]->pos.setZ(g->obj[0]->pos.z()+5e-3);
    emit frameReady();
}
