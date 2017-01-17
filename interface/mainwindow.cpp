#include "mainwindow.h"
#include "options.h"
#include "game.h"
#include "ui_mainwindow.h"
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    gameScr(nullptr),
    mHUD(nullptr)
{
    ui->setupUi(this);
    opt = new Options(this);
    opt->hide();
    // polaczenie sygnalu wyjscia submenuExit() klasy opt ze slotem show() klasy centralWidget
    QObject::connect(opt, SIGNAL(submenuExit(void)), this, SLOT(reload(void)));
    loadGameSettings();
    // debug
#ifdef QT_DEBUG
    QDir::setCurrent(qApp->applicationDirPath());
    QDir::setCurrent("..");
//    qDebug() << QDir::currentPath();
    on_newGameButton_clicked(); // skip menu
#endif
}

MainWindow::~MainWindow()
{
    reload();
    delete opt;
    delete gameScr;
    delete ui;
    delete mHUD;
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

// private slots
void MainWindow::on_exitButton_clicked()
{
    close();
}

void MainWindow::on_newGameButton_clicked()
{
    gameScr = new Game(this);
    mHUD = new HUD(gameScr);
    gameWorker = new GameWorker(gameScr);
    workerThread = new QThread;
    actionTimer = new QTimer;
    actionTimer->setTimerType(Qt::TimerType::PreciseTimer);
    // prepare gameWorker thread
    QObject::connect(actionTimer, SIGNAL(timeout(void)), gameWorker, SLOT(onTick(void)));
    QObject::connect(gameWorker, SIGNAL(frameReady(void)), gameScr, SLOT(update(void)));
    // connect signals from Game widget
    QObject::connect(gameScr, SIGNAL(exitToMenu(void)), this, SLOT(reload(void)));
    QObject::connect(gameScr, SIGNAL(paintCompleted(void)), gameWorker, SLOT(acceptFrame(void)));
    QObject::connect(gameScr, SIGNAL(paintCompleted(void)), mHUD, SLOT(acceptFrame(void)));
    // connect signals from HUD
    QObject::connect(mHUD, SIGNAL(exit(void)), this, SLOT(reload(void)));

    actionTimer->start(tickDelayMs);
    actionTimer->moveToThread(workerThread);
    gameWorker->moveToThread(workerThread);
    workerThread->start();

    ui->centralWidget->hide();
    gameScr->show();
}

void MainWindow::on_optionsButton_clicked()
{
    ui->centralWidget->hide();
    opt->show();
}

void MainWindow::reload()
{
    if (gameScr != nullptr) {
        actionTimer->deleteLater();
        workerThread->exit();
        workerThread->wait();
        delete gameWorker;
        delete workerThread;
        delete gameScr;
        gameScr = nullptr;
    }
    this->setFocus();
    ui->centralWidget->show();
}
