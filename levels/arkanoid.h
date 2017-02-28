#ifndef ARKANOID_H
#define ARKANOID_H

#include <QColor>
#include <QElapsedTimer>
#include <QHash>
#include <QKeyEvent>
#include <QLabel>
#include <QPainter>
#include <QPaintEvent>
#include <QPair>
#include <QPoint>
#include <QSize>
#include <QTimer>
#include <QVector>
#include <QWidget>
#include "include/neural.h"

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
    const int NEURAL_NET_INPUT_RADIUS = 40;
    void start();
    void runAsNeuralNetwork();

protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *qke) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent *qke) Q_DECL_OVERRIDE;
private:
    void setNeuralInputs();
    void levelGen(int id = 0);

private slots:
    void onTick();
    void neuroTick();
    void reset();

private:
    struct NeuralState {
        NeuralState() :
            generation(0), population(defPopulation), index(0) {}
        int generation;
        int population;
        int index;
        static const int defPopulation = 500;
    };
    NeuralState nst;
    QVector<ArkanoidBrick*> vec;
    QVector<QVector<double>> neuralInputs;
    QVector<Aurora::NeuralNetwork> vnn;
    QElapsedTimer elt;
    QTimer tle, vft; // VeryFastTimer
    qint64 tx;
    QPoint ballPos;
    QPoint vausPos;
    QSize vausSize;
    QSize defBrickSize;
    int ballRadius;
    int lives;
    int vx;
    int ballVy, ballVx;
    int maxVx, maxVy;
    int score;
    int scoreMult;
    int totalBricks;
    int msTimeLimit;
    int constTickTime; // only used in neural mode used to simulate 10 ms frames
    int tickCount;
    int tickCountLimit;
    bool neuralMode;
    bool keyLT, keyRT;
    bool gameOver;
    bool userTerminate;
    bool debugNeuralInputs;
    bool normalSpeed;
    bool initialized;
    bool resetting;
};

} // Aurora
#endif // ARKANOID_H
