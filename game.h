#ifndef GAME_H
#define GAME_H

#include <QElapsedTimer>
#include <QHash>
#include <QKeyEvent>
#include <QMap>
#include <QMatrix4x4>
#include <QOpenGLWidget>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QResizeEvent>
#include <QSet>
#include <QVector>
#include <QPair>

#include <algorithm>

#include "geometryprovider.h"
#include "modelcontainer.h"
#include "include/fileops.h"
#include "include/mapgenerator.h"
#include "include/ray_intersect.h"
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
    void drawEdges();

    void loadTextures();
    void loadShaders();
    void loadSettings();
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
    QVector<QVector<int> > edges;
    ModelContainer* galaxyMap;
    // current camera rotation and position
    float camSpeed, rotationSpeed;
    float camFov, camNear, camFar;
    const QVector3D camPosDef = QVector3D(0.0f, 0.0f,  4.5f), camFrontDef = QVector3D(0.0f, 0.0f, -1.0f), camUpDef = QVector3D(0.0f, 1.0f,  0.0f);
    QVector3D camPos, camFront, camUp;
    const QVector3D camRotDef = QVector3D(0.0f, -90.0f, 0.0f);
    QVector3D camRot;
    QVector3D lightPos;
    // input handling
    const qint32 mouseXor = -1;
    QSet<qint32> keys;
    // settings
    QHash<QString, QString> mSettings;
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
