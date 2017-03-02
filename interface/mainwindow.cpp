#include "mainwindow.h"
#include "options.h"
#include "game.h"
#include "ui_mainwindow.h"
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    gameScr(nullptr),
    wHUD(nullptr),
    paintCounter((long long)this)
{
    ui->setupUi(this);
    opt = new Options(this);
    opt->hide();
    wololoTimer.start(10);
    // polaczenie sygnalu wyjscia submenuExit() klasy opt ze slotem show() klasy centralWidget
    QObject::connect(opt, SIGNAL(submenuExit(void)), this, SLOT(reload(void)));
    QObject::connect(&wololoTimer, SIGNAL(timeout()), this, SLOT(update()));

    loadGameSettings();
    QDir::setCurrent(qApp->applicationDirPath());
    // debug
#ifdef QT_DEBUG
    QDir::setCurrent("..");
//    qDebug() << QDir::currentPath();
    //on_newGameButton_clicked(); // skip menu
#endif
}

MainWindow::~MainWindow()
{
    unloadGame();
    delete opt;
    delete ui;
}
// public
void MainWindow::resizeEvent(QResizeEvent *)
{
    if (gameScr!=nullptr)
        gameScr->resize(this->size());
}

void MainWindow::loadGameSettings()
{
    tickDelayMs = 8;
}

// protected slots
void MainWindow::paintEvent(QPaintEvent *)
{
    if (ui->centralWidget->isHidden())
        return;
    paintCounter++;
    paintCounter%=2513;
    this->setStyleSheet(QString("MainWindow:enabled {background-color:rgb(%1,%2,%3);}").arg(
                            QString::number(36+abs(12+cosf(paintCounter/400.0f)* 45)),
                            QString::number(abs(42+sinf(paintCounter/400.0f)*40)),
                            QString::number(60+abs(sinf(-paintCounter/400.0f)*50))));
}

// private slots
void MainWindow::on_exitButton_clicked()
{
    close();
}

void MainWindow::on_newGameButton_clicked()
{
    gameScr = new Game(this);
    wHUD = new HUD(gameScr);
    gameWorker = new GameWorker(gameScr);
    workerThread = new QThread;
    actionTimer = new QTimer;
    actionTimer->setTimerType(Qt::TimerType::PreciseTimer);
    // prepare gameWorker thread
    QObject::connect(actionTimer, SIGNAL(timeout(void)), gameWorker, SLOT(onTick(void)));
    QObject::connect(gameWorker, SIGNAL(frameReady(void)), gameScr, SLOT(update(void)));
    // connect signals from Game widget
    QObject::connect(gameScr, SIGNAL(escPressed(void)), wHUD, SLOT(togglePauseMenu(void)));
    QObject::connect(gameScr, SIGNAL(paintCompleted(void)), gameWorker, SLOT(acceptFrame(void)));
    QObject::connect(gameScr, SIGNAL(paintCompleted(void)), wHUD, SLOT(acceptFrame(void)));
    // connect signals from HUD
    QObject::connect(wHUD, SIGNAL(quitAll(void)), this, SLOT(close(void)));
    QObject::connect(wHUD, SIGNAL(quitGame(void)), this, SLOT(reload(void)));
    QObject::connect(wHUD, SIGNAL(enterSettings(void)), opt, SLOT(show()));

    actionTimer->start(tickDelayMs);
    actionTimer->moveToThread(workerThread);
    gameWorker->moveToThread(workerThread);
    workerThread->start();
    wololoTimer.stop();

    ui->centralWidget->hide();
    gameScr->show();
}

void MainWindow::on_optionsButton_clicked()
{
    ui->centralWidget->hide();
    opt->show();
    opt->setFocus();
}

void MainWindow::reload()
{
    unloadGame();
    wololoTimer.start();
    ui->centralWidget->show();
    opt->hide();
    this->setFocus();
}

void MainWindow::unloadGame()
{
    if (gameScr != nullptr) {
        actionTimer->deleteLater();
        workerThread->exit();
        workerThread->wait();
        delete gameWorker;
        delete workerThread;
        delete gameScr; // also deletes mHUD
        gameScr = nullptr;
    }

}

void MainWindow::on_minigameButton_clicked()
{
    wArkanoid = new ArkanoidWidget(this);
    wololoTimer.stop();
    wArkanoid->show();
    ui->centralWidget->hide();
    wArkanoid->setThreadCount(4);
    wArkanoid->runAsNeuralNetwork();
}
