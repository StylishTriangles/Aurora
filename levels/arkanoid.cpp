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
    p.drawRoundedRect(QRect(vausPos,vausSize),9,9);
    p.drawEllipse(ballPos,ballRadius,ballRadius);
    if (debugNeuralInputs) {
        QPen pen(QColor(255,0,0));
        pen.setWidth(1);
        p.setPen(pen);
        p.setBrush(QColor(0,0,0,0));
        for (int i = 0; i < DEF_WIDTH; i += NEURAL_NET_INPUT_RADIUS)
        {
            for (int j = 0; j < DEF_HEIGHT; j+= NEURAL_NET_INPUT_RADIUS)
            {
                p.drawRect(i,j,NEURAL_NET_INPUT_RADIUS,NEURAL_NET_INPUT_RADIUS);
            }
        }
    }
}

void ArkanoidWidget::keyPressEvent(QKeyEvent *qke)
{
    if (qke->key() == Qt::Key_Left)
        keyLT = true;
    if (qke->key() == Qt::Key_Right)
        keyRT = true;
    if (qke->key() == Qt::Key_Escape)
        userTerminate = true;
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
                totalBricks++;
            }
        }
    }


}

void ArkanoidWidget::onTick() // 10 msec ticks for best gameplay
{
    tickCount++;
    qint64 tempT = elt.elapsed();
    int dt = (tempT - tx);
    if (neuralMode)
        dt = constTickTime;
    tx = tempT;
    int actualVx = 0;
    if (keyLT)
        actualVx -= dt*vx/1000;
    if (keyRT)
        actualVx += dt*vx/1000;
    vausPos.rx() += actualVx;

    int ballTop = ballPos.y()-ballRadius;
    int ballLeft = ballPos.x()-ballRadius;
    int ballRight = ballPos.x()+ballRadius;
    int ballBottom = ballPos.y()+ballRadius;
    for (ArkanoidBrick* b: vec) {
        if (!b->destroyed) {
            if(ballPos.x() > b->pos.x() and ballPos.x() < b->pos.x()+b->size.width() and
                    ((ballTop < b->pos.y()+b->size.height() and ballTop > b->pos.y()) or
                        (ballBottom > b->pos.y() and ballBottom < b->pos.y()+b->size.height()))){
                ballVy *= -1;
                if (--(b->tgh) == 0)
                    b->destroyed = true;
                score+=scoreMult;
            }
            if (ballPos.y() > b->pos.y() and ballPos.y() < b->pos.y()+b->size.height() and
                    ((ballRight > b->pos.x() and ballRight < b->pos.x()+b->size.width()) or
                     (ballLeft > b->pos.x() and ballLeft < b->pos.x()+b->size.width()))) {
                ballVx *= -1;
                if (--(b->tgh) == 0)
                    b->destroyed = true;
                score+=scoreMult;
            }
        }
    }
    // game ending conditions
    if (ballPos.y() > vausPos.y() or score == totalBricks*scoreMult or elt.elapsed() > msTimeLimit or (tickCount >= tickCountLimit and neuralMode))
        gameOver = true;

    if (ballVy > 0 and ballBottom > vausPos.y() and ballRight > vausPos.x() and ballLeft < vausPos.x()+vausSize.width()) {
        ballVy*=-1;
        ballVx += (ballPos.x()-(vausPos.x()+vausSize.width()/2))*5;
    }
    if (ballVx > maxVx)
        ballVx = maxVx;
    if (ballVx < -maxVx)
        ballVx = -maxVx;

    ballPos.rx() += dt*ballVx/1000;
    ballPos.ry() += dt*ballVy/1000;

    if (vausPos.x() > DEF_WIDTH-40-vausSize.width())
        vausPos.setX(DEF_WIDTH-40-vausSize.width());
    if (vausPos.x() < 40)
        vausPos.setX(40);
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
    maxVx = 240;
    maxVy = 240;
    lives = 1;
    keyLT = false;
    keyRT = false;
    gameOver = false;
    neuralMode = false;
    userTerminate = false;
    debugNeuralInputs = true;
    score = 0;
    scoreMult = 10000;
    totalBricks = 0;
    msTimeLimit = 420000; // 7 minutes
    constTickTime = 10;
    tickCount = 0;
    tickCountLimit = msTimeLimit/constTickTime;
    levelGen(0);
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
    ballVx = rng()%maxVx-maxVx/2;
    ballVy = -maxVy;
}

void ArkanoidWidget::runAsNeuralNetwork()
{
    this->setFocus();
    std::mt19937 rng(elt.nsecsElapsed());
    tle.start(10); // now only used for refreshing the window
    //QObject::connect(&tle, SIGNAL(timeout()), this, SLOT(onTick()));
    QObject::connect(&tle, SIGNAL(timeout()), this, SLOT(update()));
    neuralMode = true;
    // le loop
    while (!userTerminate) {
        reset();
        ballVx = rng()%maxVx-maxVx/2;
        ballVy = -maxVy;
        elt.restart();
        while (!gameOver) {
        }
    }
}

}
