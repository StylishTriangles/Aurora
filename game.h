#ifndef GAME_H
#define GAME_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QResizeEvent>

namespace Ui {
class Game;
}

class Game : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit Game(QWidget *parent = 0);
    ~Game();

public slots:
    virtual void resizeEvent(QResizeEvent*) Q_DECL_OVERRIDE;
};

#endif // GAME_H
