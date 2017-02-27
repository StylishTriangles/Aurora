#ifndef ARKANOID_H
#define ARKANOID_H

#include <QColor>
#include <QElapsedTimer>
#include <QHash>
#include <QKeyEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QPair>
#include <QPoint>
#include <QSize>
#include <QTimer>
#include <QVector>
#include <QWidget>

namespace Aurora {
struct ArkanoidBrick
{
    ArkanoidBrick() = default;
    ArkanoidBrick(QColor bc, QColor fc, QSize sz, QPoint position, int toughness) :
        borderCol(bc), faceCol(fc),
        size(sz), pos(position),
        tgh(toughness), destroyed(false) {}
    QColor borderCol;
    QColor faceCol;
    QSize size;
    QPoint pos;
    short tgh;
    bool destroyed;

    QRect asRect() {
        return QRect(pos, size);
    }
};

class ArkanoidWidget final : public QWidget
{
    Q_OBJECT
public:
    ArkanoidWidget(QWidget* parent = 0);
    ~ArkanoidWidget();
    const int DEF_WIDTH = 960;
    const int DEF_HEIGHT = 720;
    const int BUCKET_WIDTH = (DEF_WIDTH+4)/8;
    const int BUCKET_HEIGHT = (DEF_HEIGHT+4)/8;
    void start();

protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *qke) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent *qke) Q_DECL_OVERRIDE;

private:
    void levelGen(int id = 0);

private slots:
    void onTick();
    void reset();

private:
    QVector<ArkanoidBrick*> vec;
    QElapsedTimer elt;
    QTimer tle;
    qint64 tx;
    QPoint ballPos;
    QPoint vausPos;
    QSize vausSize;
    QSize defBrickSize;
    int ballRadius;
    int lives;
    int vx;
    int ballVy, ballVx;
    bool keyLT, keyRT;
    bool gameOver;

};

} // Aurora
#endif // ARKANOID_H
