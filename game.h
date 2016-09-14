#ifndef GAME_H
#define GAME_H

#include <QOpenGLWidget>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLShaderProgram>
#include <QResizeEvent>
#include <QMatrix4x4>
#include <QVector>
#include <QKeyEvent>
#include <QElapsedTimer>
#include "glsphere.h"
#include "geometryprovider.h"

namespace Ui {
class Game;
}

class Game : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Core
{
    Q_OBJECT

public:
    explicit Game(QWidget *parent = 0);
    ~Game();

public slots:
    void setXRotation(float angle);
    void setYRotation(float angle);
    void setZRotation(float angle);

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    virtual void resizeGL(int w ,int h) Q_DECL_OVERRIDE;
    virtual void paintGL() Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent* event);
    void drawSphere(float radius, float x, float y, float z);
    void drawGeosphere(float x, float y, float z);

private:
    void setupVBOAttribute();

    QMatrix4x4 projectionMat, viewMat;
    QOpenGLBuffer planetVbo;
    QOpenGLVertexArrayObject Vao;
    QOpenGLShaderProgram *planetsProgram;
    QPoint lastCursorPos;
    bool shadersCompiled;
    // current camera rotation and position
    float camXRot, camYRot, camZRot;
    float camSpeed, rotationSpeed;
    QVector3D camPos, camFront, camUp;
    // temp
    QOpenGLTexture* tex;
    QElapsedTimer timer;
    long long int oldTime;

signals:
    void exitToMenu();
};

#endif // GAME_H
