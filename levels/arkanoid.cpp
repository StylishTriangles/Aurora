#include <algorithm>
#include <cmath>
#include <random>
#include <QDebug>
#include <QPainter>
#include "arkanoid.h"

namespace Aurora {

void ArkanoidWorker::onTick()
{
    onTick(myThreadID);
}

void ArkanoidWorker::onTick(int threadID)
{
    if (!initialized)
        return;
    if (iter >= p->workerData[threadID].size())
        return;
    if (gameOver) {
        p->vnn[p->workerData[threadID][iter]].setFitness(score-tickCount*constTickTime);
        reset();
        //setNeuralInputs();
        //std::uniform_int_distribution<int> uid(maxVx/2,maxVx);
        //ballVx = uid(rng)*(rng()%2?-1:1);
        //ballVy = -maxVy;
        iter++;
        qlIndex.setText(QString::number(p->workerData[threadID][iter]));
    }
    tickCount++;
    int dt = constTickTime;
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
    for (ArkanoidBrick& b: levelData) {
        if (!b.destroyed and b.tgh > 0) {
            if(ballPos.x() > b.pos.x() and ballPos.x() < b.pos.x()+b.size.width() and
                    ((ballTop < b.pos.y()+b.size.height() and ballTop > b.pos.y()) or
                     (ballBottom > b.pos.y() and ballBottom < b.pos.y()+b.size.height()))){
                ballVy *= -1;
                if (--(b.tgh) == 0)
                    b.destroyed = true;
                score+=scoreMult;
            }
            else if (ballPos.y() > b.pos.y() and ballPos.y() < b.pos.y()+b.size.height() and
                    ((ballRight > b.pos.x() and ballRight < b.pos.x()+b.size.width()) or
                     (ballLeft > b.pos.x() and ballLeft < b.pos.x()+b.size.width()))) {
                ballVx *= -1;
                if (--(b.tgh) == 0)
                    b.destroyed = true;
                score+=scoreMult;
            }
        }
    }
    // game ending conditions
    if (ballPos.y() > vausPos.y() or score == totalBricks*scoreMult or tickCount >= tickCountLimit)
        gameOver = true;

    if (ballVy > 0 and ballBottom > vausPos.y() and ballRight > vausPos.x() and ballLeft < vausPos.x()+vausSize.width()) {
        ballVy*=-1;
        ballVx += (ballPos.x()-(vausPos.x()+vausSize.width()/2))*5;
        ballVx += actualVx*10;
    }
    if (ballVx > maxVx)
        ballVx = maxVx;
    if (ballVx < -maxVx)
        ballVx = -maxVx;

    ballPos.rx() += dt*ballVx/1000;
    ballPos.ry() += dt*ballVy/1000;

    if (vausPos.x() > p->DEF_WIDTH-40-vausSize.width())
        vausPos.setX(p->DEF_WIDTH-40-vausSize.width());
    if (vausPos.x() < 40)
        vausPos.setX(40);
//        setNeuralInputs(); // !TODO
//    qlScore.setText(QString::number(score));
//    p->vnn[p->workerData[threadID][iter]].run();
//    if (vnn[nst.index].binaryOutput(0) and vnn[nst.index].getOutput(0) > vnn[nst.index].getOutput(1))
//        keyLT = true;
//    else
//        keyLT = false;
//    if (vnn[nst.index].binaryOutput(1) and vnn[nst.index].getOutput(0) < vnn[nst.index].getOutput(1))
//        keyRT = true;
//    else
//        keyRT = false;
}

void ArkanoidWorker::restart()
{
    iter = 0;
    reset();
}

void ArkanoidWorker::reset()
{
    vausPos.setY(p->height()-100);
    vausPos.setX(p->width()/2);
    vausPos = QPoint(p->DEF_WIDTH/2-40, p->DEF_HEIGHT-120);
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
    msTimeLimit = 300000; // 5 minutes
    constTickTime = 10;
    tickCount = 0;
    tickCountLimit = msTimeLimit/constTickTime;
    keyLT = false;
    keyRT = false;
    gameOver = false;
    if (!initialized) {
        userTerminate = false;
        debugNeuralInputs = true;
        normalSpeed = true;
        neuralInputs.resize(p->DEF_HEIGHT/p->NEURAL_NET_INPUT_RADIUS);
        for (auto& vtop: neuralInputs)
            vtop.resize(p->DEF_WIDTH/p->NEURAL_NET_INPUT_RADIUS);
    }
    std::uniform_int_distribution<int> uid(maxVx/2,maxVx);
    ballVx = -180;//uid(rng)*(rng()%2?-1:1);
    ballVy = -maxVy;
    //levelGen(0); // !TODO
    initialized = true;
}


ArkanoidWidget::ArkanoidWidget(QWidget *parent) :
    QWidget(parent), rng((long long)this),
    qlGen(this), qlIndex(this), qlScore(this),
    threadCount(1)
{
    parent->resize(DEF_WIDTH, DEF_HEIGHT);
    resize(parent->size());
    elt.start();
    defBrickSize = QSize(80, 40);
    qlGen.setGeometry(10,10,200,20);
    qlIndex.setGeometry(260,10,200,20);
    qlScore.setGeometry(800,10,200,20);
}

ArkanoidWidget::~ArkanoidWidget()
{
    for (ArkanoidBrick* x: levelData)
        delete x;
}

void ArkanoidWidget::paintEvent(QPaintEvent *)
{
    if (!initialized)
        return;
    QPainter p(this);
    QSize deltaSz = QSize(4,4);
    QPoint deltaPos = QPoint(2,2);
    for (ArkanoidBrick* ab: levelData) {
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
                    p.setBrush(QColor(128,0,0));
                else if (neuralInputs[i/NNIR][j/NNIR] == 1.0)
                    p.setBrush(QColor(255,255,255));
                else if (neuralInputs[i/NNIR][j/NNIR] == 0.08)
                    p.setBrush(QColor(0,0,0,255));
                else
                    p.setBrush(QColor(0,0,0,0));
                p.drawRect(j,i,NEURAL_NET_INPUT_RADIUS,NEURAL_NET_INPUT_RADIUS);
            }
        }

    }
    if (keyLT)
        p.setBrush(QColor(0,255,0));
    else
        p.setBrush(QColor(255,0,0));
    p.drawRect(900,700,20,20);
    if (keyRT)
        p.setBrush(QColor(0,255,0));
    else
        p.setBrush(QColor(255,0,0));
    p.drawRect(940,700,20,20);
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
    for (ArkanoidBrick* b: levelData) {
        if (!b->destroyed) {
            for (int i = 0; i < b->size.height(); i+= NEURAL_NET_INPUT_RADIUS)
                for (int j = 0; j < b->size.width(); j+= NEURAL_NET_INPUT_RADIUS)
                    neuralInputs[(b->pos.y()+i)/NEURAL_NET_INPUT_RADIUS][(b->pos.x()+j)/NEURAL_NET_INPUT_RADIUS] = (b->tgh<0)?0.0:.08;
        }
    }
    neuralInputs[ballPos.y()/NEURAL_NET_INPUT_RADIUS][ballPos.x()/NEURAL_NET_INPUT_RADIUS] = 2.0;
    for (int i = 0; i < vausSize.width(); i+=NEURAL_NET_INPUT_RADIUS) {
        neuralInputs[vausPos.y()/NEURAL_NET_INPUT_RADIUS][(vausPos.x()+i+NEURAL_NET_INPUT_RADIUS/2)/NEURAL_NET_INPUT_RADIUS] = 1.0;
    }
    for (int i = 0; i < neuralInputs.size(); i++) {
        for (int j = 0; j < neuralInputs[i].size(); j++) {
           vnn[nst.index].setInput(i*neuralInputs.size()+j, neuralInputs[i][j]);
        }
    }
}

void ArkanoidWidget::levelGen(int id)
{
    // [DEBUG]
    static int l0bricks;
    if (initialized) {
        for (ArkanoidBrick* x: levelData)
            if (x->destroyed) {
                x->tgh = 1;
                x->destroyed = false;
            }
        totalBricks = l0bricks;
        return;
    }
    // [/DEBUG]
    for (ArkanoidBrick* x: levelData)
        delete x;
    levelData.clear();
    // generate frame
    for (int i = 0; i < DEF_WIDTH; i+= 120)
        levelData.push_back(new ArkanoidBrick(
                          QColor(50,50,50,255), QColor(128,128,128,255),
                          QSize(120,40), QPoint(i,0), -1));
    for (int i = 40; i < DEF_HEIGHT-120; i+= 120)
        levelData.push_back(new ArkanoidBrick(
                          QColor(50,50,50,255), QColor(128,128,128,255),
                          QSize(40,120), QPoint(0,i), -1));
    for (int i = 40; i < DEF_HEIGHT-120; i+= 120)
        levelData.push_back(new ArkanoidBrick(
                          QColor(50,50,50,255), QColor(128,128,128,255),
                          QSize(40,120), QPoint(DEF_WIDTH-40,i), -1));
    if (id == 0) {
        int depth = 6;
        int hbegin = 160;
        std::uniform_int_distribution<int> uid(0,255);
        for (int i = 0; i < depth; i++) {
            QColor brush(uid(rng),uid(rng),uid(rng));
            QColor pen(uid(rng),uid(rng),uid(rng));
            for (int j = 40; j < DEF_WIDTH-40; j+=80) {
                levelData.push_back(new ArkanoidBrick(
                                  brush, pen,
                                  QSize(80,40), QPoint(j,hbegin+i*40), 1));
                totalBricks++;
            }
        }
        l0bricks = totalBricks; // ALSO [DEBUG]
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
    for (ArkanoidBrick* b: levelData) {
        if (!b->destroyed) {
            if(ballPos.x() > b->pos.x() and ballPos.x() < b->pos.x()+b->size.width() and
                    ((ballTop < b->pos.y()+b->size.height() and ballTop > b->pos.y()) or
                        (ballBottom > b->pos.y() and ballBottom < b->pos.y()+b->size.height()))){
                ballVy *= -1;
                if (--(b->tgh) == 0) {
                    b->destroyed = true;
                score+=scoreMult;
                }
            }
            else if (ballPos.y() > b->pos.y() and ballPos.y() < b->pos.y()+b->size.height() and
                    ((ballRight > b->pos.x() and ballRight < b->pos.x()+b->size.width()) or
                     (ballLeft > b->pos.x() and ballLeft < b->pos.x()+b->size.width()))) {
                ballVx *= -1;
                if (--(b->tgh) == 0) {
                    b->destroyed = true;
                score+=scoreMult;
                }
            }
        }
    }
    // game ending conditions
    if (ballPos.y() > vausPos.y() or score == totalBricks*scoreMult or elt.elapsed() > msTimeLimit or (tickCount >= tickCountLimit and neuralMode))
        gameOver = true;

    if (ballVy > 0 and ballBottom > vausPos.y() and ballRight > vausPos.x() and ballLeft < vausPos.x()+vausSize.width()) {
        ballVy*=-1;
        ballVx += (ballPos.x()-(vausPos.x()+vausSize.width()/2))*5;
        ballVx += actualVx*10;
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
    if (neuralMode or debugNeuralInputs)
        setNeuralInputs();
    qlScore.setText(QString::number(score));
}

void ArkanoidWidget::neuroTick()
{
    if (!initialized)
        return;
    if (gameOver) {
        vnn[nst.index].setFitness(score/*-tickCount*constTickTime*/);
        reset();
        //setNeuralInputs();
        std::uniform_int_distribution<int> uid(vausSize.width(),maxVx*3/4);
        ballVx = uid(rng)*(rng()%2?-1:1);
        //ballVy = -maxVy;
        nst.index++;
        qlIndex.setText(QString::number(nst.index));
    }
    if (nst.index >= nst.population)
    {
        /// REPRODUCE NEURAL NETS
        std::sort(vnn.begin(), vnn.end(), Aurora::NeuralNetwork::fitnessCompare);
        int killLim = nst.population/2;
        int killed = 0;
        int i = 0;
        /// RAM inefficient version
//        QVector<Aurora::NeuralNetwork> newVnn;
//        QVector<Aurora::NeuralNetwork> children;
//        std::uniform_real_distribution<double> urd(0.0,1.0);
//        for (; i < nst.population and killed < killLim; i++) {
//            if (tanh((double)(nst.population-i)/(nst.population/2)) < urd(rng))
//                killed++;
//            else
//                newVnn.push_back(vnn[i]);
//        }
//        for (; i < nst.population; i++)
//            newVnn.push_back(vnn[i]);
//        while (newVnn.size()+children.size() < nst.population) {
//            children.push_back(newVnn[rng()%newVnn.size()].breedS(newVnn[rng()%newVnn.size()]));
//        }
//        vnn.clear();
//        vnn = std::move(newVnn);
//        vnn << children;
        ///

//        while (it != vnn.end() and killed < killLim) {
//            i++;
//            if (tanh((double)(nst.population-i)/(nst.population/2)) < urd(rng)) {
//                killed++;
//                delete (*it);
//                it = vnn.erase(it);
//                continue;
//            }
//            it++;
//        }
//        QVector<Aurora::NeuralNetwork*> children;
//        while (children.size() < nst.population) {
//            NeuralNetwork n = vnn[rng()%vnn.size()].breedS(*vnn[rng()%vnn.size()]);
//            children.push_back(new Aurora::NeuralNetwork);
//            (*children.back()) = n;
//        }
//        for (int i = 0; i < vnn.size(); i++)
//            delete vnn[i];
//        vnn.clear();
//        vnn = children;
//        children.clear();
       // qDebug() << "rep success";
        std::uniform_real_distribution<double> urd(0.0,1.0);
        QVector<int> toKill;
        QVector<int> git;
        for (; i < vnn.size() and killed < killLim and i < (int)(0.9*nst.population); i++) {
            if (tanh((double)(nst.population-i)/(nst.population/2)) > urd(rng)) {
                killed++;
                toKill.push_back(i);
            }
            else {
                git.push_back(i);
            }
        }
        for (; i < vnn.size(); i++)
            git.push_back(i);
        for (int j = 0; j < toKill.size(); j+=2) {
            std::pair<Aurora::NeuralNetwork,Aurora::NeuralNetwork> ntmp = vnn[git[rng()%git.size()]].breedS(vnn[git[rng()%git.size()]]);
            vnn[toKill[j]] = ntmp.first;
            if (j+1 < toKill.size()) {
                vnn[toKill[j+1]] = ntmp.second;
            }
        }
        /// ******
        nst.index = 0;
        nst.generation++;
        qlGen.setText(QString::number(nst.generation));
        qlIndex.setText(QString::number(nst.index));
        //return;
    }
    onTick();
    vnn[nst.index].run();
//    qDebug() << vnn[nst.index].getOutput(0) << vnn[nst.index].getOutput(1);
    double keyEps = 0.0001;
    auto abs = [](double d) -> double {return d<0?-d:d;};
    if (abs(vnn[nst.index].getOutput(0) - vnn[nst.index].getOutput(1)) < keyEps) {
        keyRT = false; keyLT = false;
    }
    else {
        if (/*vnn[nst.index].binaryOutput(0) and*/ vnn[nst.index].getOutput(0) > vnn[nst.index].getOutput(1))
            keyLT = true;
        else
            keyLT = false;
        if (/*vnn[nst.index].binaryOutput(1) and*/ vnn[nst.index].getOutput(0) < vnn[nst.index].getOutput(1))
            keyRT = true;
        else
            keyRT = false;
    }
    //qDebug()  << vnn[nst.index].getOutput(0) << vnn[nst.index].getOutput(1);
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
    msTimeLimit = 300000; // 5 minutes
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
        normalSpeed = true;
        neuralInputs.resize(DEF_HEIGHT/NEURAL_NET_INPUT_RADIUS);
        for (auto& vtop: neuralInputs)
            vtop.resize(DEF_WIDTH/NEURAL_NET_INPUT_RADIUS);
        vnn.resize(nst.population);
//        for (int i = 0; i < nst.population; i++)
//            vnn.push_back(new Aurora::NeuralNetwork);
        for (auto& rnn: vnn)
            rnn.construct(neuralInputs.size()*DEF_WIDTH/NEURAL_NET_INPUT_RADIUS,2,1,150);
    }
    std::uniform_int_distribution<int> uid(maxVx/2,maxVx);
    ballVx = -180;//uid(rng)*(rng()%2?-1:1);
    ballVy = -maxVy;
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
}

void ArkanoidWidget::runAsNeuralNetwork()
{
    this->setFocus();
    reset();
    neuralMode = true;
    //QObject::connect(&tle, SIGNAL(timeout()), this, SLOT(onTick()));
    QObject::connect(&tle, SIGNAL(timeout()), this, SLOT(update()));
    QObject::connect(&tle, SIGNAL(timeout()), this, SLOT(neuroTick()));
    tle.start(10); // now only used for refreshing the window
    vft.start(0);
}

}
