#include "game.h"
#include <QDebug>
#include <QtMath>
#include <QResource>
#include <random>

Game::Game(QWidget *parent) :
    QOpenGLWidget(parent),
    shadersCompiled(false), initComplete(false),
    camXRot(0), camYRot(-90.0f), camZRot(0),
    camFov(60.0f), camNear(0.01f), camFar(100.0f),
    lightPos(0.0f, -0.0f, 0.0f),
    stage(2), actSystem(0),
    cnt(0)
{
    this->resize(parent->size());
    QSurfaceFormat format;
    format.setSamples(4);
    format.setAlphaBufferSize(8);
    setFormat(format);
    //this->setMinimumSize(800, 450);
//    this->resize(parent->size());
    this->setFocus();
    qDebug() << "ModelContainer:" << sizeof(ModelContainer) << "|" << "Game:" << sizeof(Game);
}

Game::~Game()
{
    planetsProgram->release();
    for(auto p:mGeometry)
        delete p;
    for(auto p:solarSystems)
        delete p;
    for(auto p:textures)
        delete p;
    delete skyboxTexture;
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
//    glEnable(GL_DITHER);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_CULL_FACE);
    //glEnable(GL_POLYGON_SMOOTH);
    //glEnable(GL_TEXTURE_2D);

    Vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&Vao);
    planetsProgram = new QOpenGLShaderProgram;
    lightsProgram = new QOpenGLShaderProgram;

    projectionMat.setToIdentity();
    projectionMat.perspective(camFov, float(this->width()) / float(this->height()), camNear, camFar);
//    projectionMat.frustum(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f); // orthographic projection mode

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
//        GeometryProvider::cube(*tmp);
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
    solarSystems.push_back(new ModelContainer({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, "geosphere", "earth", lightsProgram, ModelContainer::Star));
    ModelContainer* tmpM = new ModelContainer({0,0,0}, {0,0,0}, "titan", "skybox", planetsProgram, ModelContainer::Skybox);
    tmpM->setScale(50.0f);
    solarSystems.back()->addChild(tmpM);
    tmpM = new ModelContainer({3.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, "geosphere", "earth", planetsProgram, ModelContainer::Planet);
    tmpM->setScale(0.1f);
    ModelContainer* tmpM2 = new ModelContainer({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, "geosphere", "atmosphere", planetsProgram, ModelContainer::Planet);
    tmpM2->setScale(1.01f);
    tmpM->addChild(tmpM2);
    tmpM2 = new ModelContainer({10.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, "geosphere", "moon", planetsProgram, ModelContainer::Moon);
    tmpM2->setScale(0.1f);
    tmpM->addChild(tmpM2);
    solarSystems.back()->addChild(tmpM);
    tmpM = new ModelContainer({4.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, "titan", "earth", planetsProgram, ModelContainer::Titan);
    tmpM->setScale(0.1f);
    solarSystems.back()->addChild(tmpM);
    initComplete = true;

    allocateVbos();
}

void Game::resizeGL(int w, int h)
{
    projectionMat.setToIdentity();
    projectionMat.perspective(camFov, w / float(h), camNear, camFar);
    findChild<HUD*>()->resize(w,h);
}

inline int Game::bindModel(ModelContainer* mod, int detail)
{
    return bindModel(mod->model, detail);
}

inline int Game::bindModel(QString const & name, int detail)
{
    auto it = mVbo.find(name + QString::number(detail));
    Q_ASSERT_X(it != mVbo.end(), "Game::bindModel()", (const char*)(name + QString::number(detail)).data());
    it.value().bind();
    return it.value().size();
}

void Game::drawModel(ModelContainer* mod)
{
    if(mod->type==ModelContainer::Skybox)
        glDisable(GL_CULL_FACE);
    QMatrix4x4 modelMat = mod->getModelMat();
    QMatrix4x4 vp = projectionMat * viewMat;
    static auto distance = [](QVector3D const& camPos, QVector3D const& modPos) -> float {
        return (camPos-modPos).length();
    };

    float d = distance(camPos, mod->getPos()) / mod->getScale().x();
    int detailLevel = (d > 60.f)?1:(d > 30.f)?2:(d > 15.f)?3:4;
    if (mod->type == ModelContainer::Skybox or mod->type == ModelContainer::Titan)
        detailLevel = 3;
    int geomSize = bindModel(mod, detailLevel);
    // init variables
    struct Light{
        QVector3D ambient;
        QVector3D diffuse;
        QVector3D specular;
        GLfloat shininess;
    };
    Light light = {QVector3D(0.1f, 0.1f, 0.1f),QVector3D(0.5f, 0.5f, 0.5f),QVector3D(0.7f, 0.7f, 0.7f),32.0f};

    // set type specific data
    if (mod->type == ModelContainer::Skybox) {
        light.ambient = QVector3D(1.0f,1.0f,1.0f);
        light.diffuse = QVector3D(0.0f,0.0f,0.0f);
        light.specular = light.diffuse;
    }
    if (mod->type == ModelContainer::Star) {
        lightsProgram->bind();
        lightsProgram->setUniformValue("vp",vp);
        lightsProgram->setUniformValue("modelMat",modelMat);
    }
    else {
        planetsProgram->bind();
        planetsProgram->setUniformValue("vp",vp);
        planetsProgram->setUniformValue("modelMat",modelMat);
        planetsProgram->setUniformValue("modelNorm",modelMat.inverted().transposed());
        planetsProgram->setUniformValue("light.position",lightPos);
        planetsProgram->setUniformValue("light.ambient", light.ambient); //można zmieniac te wartosci i patrzec, co sie stanie
        planetsProgram->setUniformValue("light.diffuse", light.diffuse); //-||-
        planetsProgram->setUniformValue("light.specular", light.specular); //-||-
        planetsProgram->setUniformValue("shininess", light.shininess);
        planetsProgram->setUniformValue("viewPos", camPos);
        planetsProgram->setUniformValue("diffuseMap", 0);
        planetsProgram->setUniformValue("specularMap", 1);
    }
    textures[mod->tex]->bind(0);
    textures[(mod->tex+"Spec")]->bind(1);

    this->glEnableVertexAttribArray(0);
    this->glEnableVertexAttribArray(1);
    this->glEnableVertexAttribArray(2);
    this->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    this->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
    this->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6*sizeof(GLfloat)));

    glDrawArrays(GL_TRIANGLES, 0, geomSize);
    for (int i = 0; i < mod->children.size(); i++)
        drawModel(mod->children[i]);
    if(mod->type==ModelContainer::Skybox)
        glEnable(GL_CULL_FACE);
}

void Game::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    viewMat.setToIdentity();
    viewMat.lookAt(camPos, camPos + camFront, camUp);

//    QOpenGLVertexArrayObject::Binder vaoBinder(&Vao);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);


    if(stage==0){ //mapa galaktyki

    }
    else{
        if(stage==1){ //bitwa

        }
        else if(stage==2){
            drawModel(solarSystems[actSystem]);
        }
    }
    emit paintCompleted();
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
    tex = new QOpenGLTexture(QImage(QString("../Aurora/atmosphere.png")));
    textures.insert("atmosphereSpec", tex);
    tex = new QOpenGLTexture(QImage(QString("../Aurora/moon.png")));
    textures.insert("moon", tex);
    tex = new QOpenGLTexture(QImage(QString("../Aurora/moon.png")));
    textures.insert("moonSpec", tex);
    tex = new QOpenGLTexture(QImage(QString(":/misc/skybox.png")));
    textures.insert("skybox", tex);
    tex = new QOpenGLTexture(QImage(QString(":/misc/skybox.png")));
    textures.insert("skyboxSpec", tex);
    tex =  new QOpenGLTexture(QImage(QString("../Aurora/textures/earthSpec.png")));
    textures.insert("earthSpec", tex);
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
    shadersCompiled = true;
    if (!noerror) {qDebug() << planetsProgram->log();}

    // light source shader
    noerror = true;
    noerror |= lightsProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/lightVertex.vert");
    noerror |= lightsProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/lightFragment.frag");
    lightsProgram->bindAttributeLocation("position", 0);
    lightsProgram->bindAttributeLocation("normal", 1);
    lightsProgram->bindAttributeLocation("texCoord", 2);
    noerror |= lightsProgram->link();
    lightsProgram->setUniformValue("tex", 0);
    shadersCompiled = true;
    if (!noerror) {qDebug() << lightsProgram->log();}
}

void Game::allocateVbos() {
    auto it = mGeometry.begin();
    while (it != mGeometry.end()) {
        auto it2 = mVbo.insert(it.key(), QOpenGLBuffer());
        it2.value().create();
        it2.value().bind();
        it2.value().allocate(it.value()->data(),it.value()->size()*sizeof(GLfloat));
        ++it;
    }
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
        dx -= dT*rotationSpeed*100.0f;
    if (keys.find(Qt::Key_E) != keys.end())
        dx += dT*rotationSpeed*100.0f;
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
    qint64 nse = et.nsecsElapsed();
    float dT = (float)(nse-lastTick)/1e9f;
    lastTick = nse;
    g->parseInput(dT*25.0f);
    // gaym logic
    if (!g->initComplete)
        return;
    if(g->actSystem != -1){
        orbit(g->solarSystems[g->actSystem], nse);
    }
//    g->lightPos = g->camPos;
    emit frameReady();
}

void GameWorker::orbit(ModelContainer* m, qint64 nse){
    if(m->type==ModelContainer::Planet || m->type==ModelContainer::Titan || m->type==ModelContainer::Moon){
        float rad=sqrt((m->position.x())*(m->position.x())+(m->position.y())*(m->position.y()));
        m->position[0]=sinf(nse*0.0000000001f+rad)*rad;
        m->position[1]=cosf(nse*0.0000000001f+rad)*rad;
    }
    for(auto p:(m->children)){
        orbit(p, nse);
    }
}

void GameWorker::acceptFrame() {
//    qint64 nse = et.nsecsElapsed();
//    float dT = (float)(nse-lastFrame)/1e9f;
//    lastFrame = nse;
}
