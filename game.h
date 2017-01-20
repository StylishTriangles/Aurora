#ifndef GAME_H
#define GAME_H

#include <QOpenGLWidget>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QResizeEvent>
#include <QMatrix4x4>
#include <QVector>
#include <QKeyEvent>
#include <QElapsedTimer>
#include <QSet>
#include <QMap>
#include "geometryprovider.h"
#include "modelcontainer.h"
#include "interface/hud.h"

namespace Ui {
class Game;
}
class Game;
class GameWorker;

class GameWorker : public QObject // multithread
{
    Q_OBJECT
public:
    GameWorker(Game* parent) : g(parent), lastTick(0), lastFrame(0)
    {et.start();}
public slots:
    void onTick();
    void acceptFrame();
signals:
    void frameReady();
private:
    void orbit(ModelContainer* m, qint64 nse);
    Game* g;
    QElapsedTimer et;
    qint64 lastTick;
    qint64 lastFrame;
};

class Game : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Core
{
    Q_OBJECT
public:

public:
    explicit Game(QWidget *parent = 0);
    ~Game();
    friend class GameWorker;

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w ,int h) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
    void drawModel(ModelContainer* mod);
    void drawOrbit(ModelContainer* mod);

    void loadTextures();
    void loadShaders();
    void allocateVbos();
    void parseInput(float dT);

private:
    int bindModel(ModelContainer* mod, int detail);
    int bindModel(QString const & name, int detail);

    QMatrix4x4 projectionMat, viewMat;
    QOpenGLBuffer planetVbo;
    QOpenGLVertexArrayObject Vao;
    QOpenGLShaderProgram *planetsProgram, *lightsProgram, *planeGeoProgram;
    QPoint lastCursorPos;
    bool shadersCompiled, initComplete;
    // game world objects
    QHash<QString, QVector<GLfloat>*> mGeometry;
    QHash<QString, QOpenGLBuffer> mVbo;

    QHash<QString, QOpenGLTexture*> textures;
    QVector<ModelContainer*> solarSystems;
    ModelContainer* galaxyMap;
    // current camera rotation and position
    float camXRot, camYRot, camZRot;
    float camSpeed, rotationSpeed;
    float camFov, camNear, camFar;
    QVector3D camPos, camFront, camUp;
    QVector3D lightPos;
    // input handling
    QSet<qint32> keys;
    // active scene
    int stage;
    int actSystem;
    // temp
    long long int cnt;

signals:
    void exitToMenu();
    void escPressed();
    void paintCompleted();
};

#endif // GAME_H
