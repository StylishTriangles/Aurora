#ifndef GAME_H
#define GAME_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions_4_3_Core>
#include <QResizeEvent>
#include <QMatrix4x4>
#include <QDebug>
#include <vector>


namespace Ui {
class Game;
}

class Game : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Core
{
    Q_OBJECT

public:
    explicit Game(QWidget *parent = 0);
    ~Game();

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    virtual void resizeGL(int w ,int h) Q_DECL_OVERRIDE;
    virtual void paintGL() Q_DECL_OVERRIDE;
    void drawTriangle();
    void drawSquare();
    void drawCircle(float cx, float cy, float scale);

private:
    void setupVBOAttribute();

    QMatrix4x4 mainCamera;
    QOpenGLBuffer Vbo;
    QOpenGLVertexArrayObject m_vao;
};

#endif // GAME_H
