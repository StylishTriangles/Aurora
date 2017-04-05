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
#include <QPair>
#include <QResizeEvent>
#include <QSet>
#include <QtXml/QtXml>
#include <QVector>

#include <algorithm>

#include "geometryprovider.h"
#include "modelcontainer.h"
#include "player.h"
#include "details.h"
#include "include/fileops.h"
#include "include/helper_classes.h"
#include "include/mapgenerator.h"
#include "include/ray_intersect.h"
#include "interface/hud.h"
#include "interface/osd.h"

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
    void initGame();
signals:
    void frameReady();
    void initComplete();
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
    explicit Game(QWidget *parent = 0);
    ~Game();
    friend class GameWorker;

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w ,int h) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void applyChanges();
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;
    void drawModel(ModelContainer* mod);
    void drawOrbit(ModelContainer* mod);
    void drawEdges();
    void drawLoadingScreen();

    void allocateVbos();
    void initializeEnvRemote();
    void initializeEnvLocal();
    void loadTextures();
    void loadShaders();
    void loadSettings();
    void loadPrototypes();
    void loadXml();
    void setLightTypes();
    void setupLS();
    void setupTextures();

    void parseInput(float dT);

public slots:
    void initializeEnv();
    void receiveButton(int keyID);
    
private:
    int bindModel(ModelContainer* mod, int detail);
    int bindModel(QString const & name, int detail = -1);

    QMatrix4x4 projectionMat, viewMat;
    QOpenGLBuffer planetVbo;
    QOpenGLVertexArrayObject Vao;
    QOpenGLShaderProgram *planetsProgram, *lightsProgram, *planeGeoProgram, *loadingMainProgram, *edgesProgram, *coronaProgram;
    QPoint lastCursorPos;
    bool shadersCompiled, initComplete, preInitComplete;
    // game world objects
    QHash<QString, QVector<GLfloat>*> mGeometry;
    QHash<QString, QOpenGLBuffer> mVbo;
    QOpenGLBuffer lsVbo;

    QHash<QString, QOpenGLTexture*> mTextures;
    QHash<QString, QImage> mTempImageData;
    QHash<QString, Light> mLights;
    QVector<ModelContainer*> solarSystems;
    QVector<Details*> solarDetails;
    QVector<QVector<int> > edges;
    ModelContainer* galaxyMap, *spaceShip;

    QVector<Player*> mPlayers;
    QVector<QPair<QVector3D, int>> solarChanges;
    int stageChange;
    // buildings
    QDomDocument allBuildings;
    // current camera rotation and position
    float camSpeed, rotationSpeed;
    float camFov, camNear, camFar;
    const QVector3D camPosDef = QVector3D(0.0f, 0.0f,  12.0f), camFrontDef = QVector3D(0.0f, 0.0f, -1.0f), camUpDef = QVector3D(0.0f, 1.0f,  0.0f);
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
    // timers
    QElapsedTimer et;
    // temp
    long long int cnt;

signals:
    void escPressed();
    void exitToMenu();
    void paintCompleted();
    void toggleHUD();
};

#endif // GAME_H
