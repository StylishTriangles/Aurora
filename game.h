#ifndef GAME_H
#define GAME_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QResizeEvent>
#include <QMatrix4x4>
//#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_4_Core>
#include <QDebug>
#include <vector>


namespace Ui {
class Game;
}

class Game : public QOpenGLWidget, protected QOpenGLFunctions_4_4_Core
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
    QMatrix4x4 mainCamera;
};

#endif // GAME_H
