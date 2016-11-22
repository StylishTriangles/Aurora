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
#include "geometryprovider.h"

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
    void drawSphere(float radius, float x, float y, float z);
    void drawGeosphere(float x, float y, float z, float r = 1.0f);
    void drawGeosphere(QVector<GLfloat> const& modelGeometry, float x, float y, float z, float r = 1.0f);
    void drawSkybox(float radius = 50.0f);

    void parseInput(float dT);

private:
    void setupVBOAttribute();

    QMatrix4x4 projectionMat, viewMat;
    QOpenGLBuffer planetVbo;
    QOpenGLVertexArrayObject Vao;
    QOpenGLShaderProgram *planetsProgram;
    QPoint lastCursorPos;
    bool shadersCompiled;
    // game world objects
    QVector<GLfloat> geosphereModel;
    QVector<GLfloat> titanModel;
    QOpenGLTexture* skyboxTexture;
    // current camera rotation and position
    float camXRot, camYRot, camZRot;
    float camSpeed, rotationSpeed;
    QVector3D camPos, camFront, camUp;
    // input handling
    QSet<qint32> keys;
    // temp
    QOpenGLTexture* tex, *atmo;
    QElapsedTimer timer;
    long long int oldTime;

signals:
    void exitToMenu();
};

#endif // GAME_H
