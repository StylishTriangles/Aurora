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
#include "glsphere.h"

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
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    virtual void resizeGL(int w ,int h) Q_DECL_OVERRIDE;
    virtual void paintGL() Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent* event);
    void drawTriangle();
    void drawCircle(float cx, float cy, float scale);
    void drawSphere(GLfloat radius);

private:
    void setupVBOAttribute();

    QMatrix4x4 projectionMat, viewMat;
    QOpenGLBuffer Vbo;
    QOpenGLBuffer planetVbo;
    QOpenGLVertexArrayObject Vao;
    QOpenGLShaderProgram *sProgram;
    QOpenGLShaderProgram *planetsProgram;
    QPoint lastCursorPos;
    // current camera rotation and position
    int camXRot, camYRot, camZRot;
    float viewDist;
    // temp
    QOpenGLTexture* tex;

signals:
    void exitToMenu();
};

#endif // GAME_H
