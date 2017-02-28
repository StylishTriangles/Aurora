#include <random>
#include <QDebug>
#include <QPainter>
#include <QtAlgorithms>
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
        for (int i = 0; i < DEF_HEIGHT; i += NEURAL_NET_INPUT_RADIUS)
        {
            for (int j = 0; j < DEF_WIDTH; j+= NEURAL_NET_INPUT_RADIUS)
            {
                const int &NNIR = NEURAL_NET_INPUT_RADIUS;
                if (neuralInputs[i/NNIR][j/NNIR] == 2.0)
                    p.setBrush(QColor(0,0,0));
                else if (neuralInputs[i/NNIR][j/NNIR] == 1.0)
                    p.setBrush(QColor(255,255,255));
                else
                    p.setBrush(QColor(0,0,0,0));
                p.drawRect(j,i,NEURAL_NET_INPUT_RADIUS,NEURAL_NET_INPUT_RADIUS);
            }
        }
    }
}

void ArkanoidWidget::keyPressEvent(QKeyEvent *qke)
{
    if (qke->key() == Qt::Key_Left and !neuralMode)
        keyLT = true;
    if (qke->key() == Qt::Key_Right and !neuralMode)
        keyRT = true;
    if (qke->key() == Qt::Key_Escape)
        userTerminate = true;
    if (qke->key() == Qt::Key_N and neuralMode) {
        if (normalSpeed) {
            QObject::disconnect(&tle, SIGNAL(timeout()), this, SLOT(neuroTick()));
            QObject::connect(&vft, SIGNAL(timeout()), this, SLOT(neuroTick()));
        }
        else {
            QObject::disconnect(&vft, SIGNAL(timeout()), this, SLOT(neuroTick()));
            QObject::connect(&tle, SIGNAL(timeout()), this, SLOT(neuroTick()));
        }
        normalSpeed ^= 1;
    }
    if (qke->key() == Qt::Key_D)
        debugNeuralInputs ^= 1;
}

void ArkanoidWidget::keyReleaseEvent(QKeyEvent *qke)
{
    if (qke->key() == Qt::Key_Left)
        keyLT^=keyLT;
    if (qke->key() == Qt::Key_Right)
        keyRT^=keyRT;
}

void ArkanoidWidget::setNeuralInputs()
{
    for (auto& vtop: neuralInputs)
        for (double& rval: vtop)
            rval = 0.0;
    for (ArkanoidBrick* b: vec) {
        if (!b->destroyed) {
            for (int i = 0; i < b->size.height(); i+= NEURAL_NET_INPUT_RADIUS)
                for (int j = 0; j < b->size.width(); j+= NEURAL_NET_INPUT_RADIUS)
                    neuralInputs[(b->pos.y()+i)/NEURAL_NET_INPUT_RADIUS][(b->pos.x()+j)/NEURAL_NET_INPUT_RADIUS] = (b->tgh<0)?2.0:1.0;
        }
    }
    neuralInputs[ballPos.y()/NEURAL_NET_INPUT_RADIUS][ballPos.x()/NEURAL_NET_INPUT_RADIUS] = 2.0;
    for (int i = 0; i < vausSize.width(); i+=NEURAL_NET_INPUT_RADIUS) {
        neuralInputs[vausPos.y()/NEURAL_NET_INPUT_RADIUS][(vausPos.x()+i+NEURAL_NET_INPUT_RADIUS/2)/NEURAL_NET_INPUT_RADIUS] = 2.0;
    }
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
    if (neuralMode or true)
        setNeuralInputs();
}

void ArkanoidWidget::neuroTick()
{
    std::mt19937 rng(elt.nsecsElapsed());
    if (gameOver) {
        reset();
        setNeuralInputs();
        ballVx = rng()%maxVx-maxVx/2;
        ballVy = -maxVy;
        nst.index++;
    }
    if (nst.index >= nst.population)
    {
        /// REPRODUCE NEURAL NETS ETC
        ///
        ///
        /// ******
        nst.index = 0;
        nst.generation++;
    }

    onTick();
}

void ArkanoidWidget::reset()
{
    Q_ASSERT(!(DEF_WIDTH%NEURAL_NET_INPUT_RADIUS));
    Q_ASSERT(!(DEF_HEIGHT%NEURAL_NET_INPUT_RADIUS));
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
    score = 0;
    scoreMult = 10000;
    totalBricks = 0;
    msTimeLimit = 420000; // 7 minutes
    constTickTime = 10;
    tickCount = 0;
    tickCountLimit = msTimeLimit/constTickTime;
    keyLT = false;
    keyRT = false;
    gameOver = false;
    if (!initialized) {
        neuralMode = false;
        userTerminate = false;
        debugNeuralInputs = true;
        normalSpeed = false;
        neuralInputs.resize(DEF_HEIGHT/NEURAL_NET_INPUT_RADIUS);
        for (auto& vtop: neuralInputs)
            vtop.resize(DEF_WIDTH/NEURAL_NET_INPUT_RADIUS);
    }
    levelGen(0);
    elt.restart();
    tx = elt.elapsed();
    initialized = true;
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
    neuralMode = true;
    this->setFocus();
    tle.start(10); // now only used for refreshing the window
    vft.start(0);
    //QObject::connect(&tle, SIGNAL(timeout()), this, SLOT(onTick()));
    QObject::connect(&tle, SIGNAL(timeout()), this, SLOT(update()));
    QObject::connect(&tle, SIGNAL(timeout()), this, SLOT(neuroTick()));
}

}
