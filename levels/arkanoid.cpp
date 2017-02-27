#include <random>
#include <QDebug>
#include <QPainter>
#include "arkanoid.h"

namespace Aurora {
ArkanoidWidget::ArkanoidWidget(QWidget *parent) :
    QWidget(parent)
{
    parent->resize(DEF_WIDTH, DEF_HEIGHT);
    resize(parent->size());
    elt.start();
    defBrickSize = QSize(80, 40);
}

ArkanoidWidget::~ArkanoidWidget()
{
    for (ArkanoidBrick* x: vec)
        delete x;
}

void ArkanoidWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QSize deltaSz = QSize(4,4);
    QPoint deltaPos = QPoint(2,2);
    for (ArkanoidBrick* ab: vec) {
        if (!ab->destroyed) {
            p.setBrush(ab->faceCol);
            QPen pen(ab->borderCol);
            pen.setWidth(4);
            p.setPen(pen);
            p.drawRect(QRect(ab->pos+deltaPos,ab->size-deltaSz));
        }
    }
    p.drawRect(QRect(vausPos,vausSize));
    p.drawEllipse(ballPos,ballRadius,ballRadius);
}

void ArkanoidWidget::keyPressEvent(QKeyEvent *qke)
{
    if (qke->key() == Qt::Key_Left)
        keyLT = true;
    if (qke->key() == Qt::Key_Right)
        keyRT = true;
}

void ArkanoidWidget::keyReleaseEvent(QKeyEvent *qke)
{
    if (qke->key() == Qt::Key_Left)
        keyLT^=keyLT;
    if (qke->key() == Qt::Key_Right)
        keyRT^=keyRT;
}

void ArkanoidWidget::levelGen(int id)
{
    for (auto x: vec)
        delete x;
    vec.clear();
    // generate frame
    for (int i = 0; i < DEF_WIDTH; i+= 120)
        vec.push_back(new ArkanoidBrick(
                          QColor(50,50,50,255), QColor(128,128,128,255),
                          QSize(120,40), QPoint(i,0), -1));
    for (int i = 40; i < DEF_HEIGHT-120; i+= 120)
        vec.push_back(new ArkanoidBrick(
                          QColor(50,50,50,255), QColor(128,128,128,255),
                          QSize(40,120), QPoint(0,i), -1));
    for (int i = 40; i < DEF_HEIGHT-120; i+= 120)
        vec.push_back(new ArkanoidBrick(
                          QColor(50,50,50,255), QColor(128,128,128,255),
                          QSize(40,120), QPoint(DEF_WIDTH-40,i), -1));
    if (id == 0) {
        int depth = 6;
        int hbegin = 160;
        std::mt19937 rng((long long)this);
        std::uniform_int_distribution<int> uid(0,255);
        for (int i = 0; i < depth; i++) {
            QColor brush(uid(rng),uid(rng),uid(rng));
            QColor pen(uid(rng),uid(rng),uid(rng));
            for (int j = 40; j < DEF_WIDTH-40; j+=80) {
                vec.push_back(new ArkanoidBrick(
                                  brush, pen,
                                  QSize(80,40), QPoint(j,hbegin+i*40), 1));
            }
        }
    }
}

void ArkanoidWidget::onTick()
{
    qint64 tempT = elt.elapsed();
    int dt = (tempT - tx);
    tx = tempT;
    if (keyLT)
        vausPos.rx() -= dt*vx/1000;
    if (keyRT)
        vausPos.rx() += dt*vx/1000;

    ballPos.rx() += dt*ballVx/1000;
    ballPos.ry() += dt*ballVy/1000;
    if (ballPos.y() > DEF_HEIGHT)
        gameOver = true;
}

void ArkanoidWidget::reset()
{
    vausPos.setY(this->height()-100);
    vausPos.setX(this->width()/2);
    vausPos = QPoint(DEF_WIDTH/2-40, DEF_HEIGHT-120);
    vausSize = QSize(120, 40);
    ballRadius = 20;
    ballPos = vausPos;
    ballPos.ry() -= ballRadius;
    ballPos.rx() += vausSize.width()/2;
    vx = 360;
    levelGen(0);
    lives = 1;
    keyLT = false;
    keyRT = false;
    gameOver = false;
    tx = elt.elapsed();
}

void ArkanoidWidget::start()
{
    this->setFocus();
    std::mt19937 rng(elt.nsecsElapsed());
    reset();
    QObject::connect(&tle, SIGNAL(timeout()), this, SLOT(onTick()));
    QObject::connect(&tle, SIGNAL(timeout()), this, SLOT(update()));
    tle.start(10);
    elt.restart();
    ballVx = (rng()%5-2)*200;
    ballVy = -120;
}

}
