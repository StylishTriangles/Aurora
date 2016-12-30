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

namespace Ui {
class Game;
}
class Game;
class GameWorker;

class GameWorker : public QObject // multithread
{
    Q_OBJECT
public:
    GameWorker(Game* parent) : g(parent), lastTime(0)
    {et.start();}
public slots:
    void onTick();
signals:
    void frameReady();
private:
    Game* g;
    QElapsedTimer et;
    qint64 lastTime;
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

    void loadTextures();
    void loadShaders();
    void parseInput(float dT);

private:
    QVector<GLfloat>* getModel(QString const & name, int detail);
    void setupVBOAttribute();

    QMatrix4x4 projectionMat, viewMat;
    QOpenGLBuffer planetVbo;
    QOpenGLVertexArrayObject Vao;
    QOpenGLShaderProgram *planetsProgram, *lightsProgram;
    QPoint lastCursorPos;
    bool shadersCompiled, initComplete;
    // game world objects
    QHash<QString, QVector<GLfloat>*> mGeometry;

    QHash<QString, QOpenGLTexture*> textures;
    QVector<ModelContainer*> obj;
    QOpenGLTexture* skyboxTexture;
    // current camera rotation and position
    float camXRot, camYRot, camZRot;
    float camSpeed, rotationSpeed;
    float camFov, camNear, camFar;
    QVector3D camPos, camFront, camUp;
    QVector3D lightPos;
    // input handling
    QSet<qint32> keys;
    // temp
    long long int cnt;

signals:
    void exitToMenu();
};

#endif // GAME_H
