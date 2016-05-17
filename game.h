#ifndef GAME_H
#define GAME_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QResizeEvent>
#include <QMatrix4x4>

namespace Ui {
class Game;
}

class Game : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit Game(QWidget *parent = 0);
    ~Game();
    void initializeGL() Q_DECL_OVERRIDE;

public slots:
    virtual void resizeEvent(QResizeEvent*) Q_DECL_OVERRIDE;

private:
    QMatrix4x4 mainCamera;
};

#endif // GAME_H
