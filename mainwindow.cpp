#include "mainwindow.h"
#include "options.h"
#include "game.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    gameScr(nullptr)
{
    ui->setupUi(this);
    opt = new Options(this);
    opt->hide();
    // polaczenie sygnalu wyjscia submenuExit() klasy opt ze slotem show() klasy centralWidget
    QObject::connect(opt, SIGNAL(submenuExit(void)), this, SLOT(reload(void)));
}

MainWindow::~MainWindow()
{
    delete opt;
    delete gameScr;
    delete ui;
}
// public
void MainWindow::resizeEvent(QResizeEvent *)
{
    if (gameScr!=nullptr)
        gameScr->resize(this->size());
}

// private slots
void MainWindow::on_exitButton_clicked()
{
    close();
}

void MainWindow::on_newGameButton_clicked()
{
    gameScr = new Game(this);
    QObject::connect(gameScr, SIGNAL(exitToMenu(void)), this, SLOT(reload(void)));
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
        QObject::disconnect(gameScr, SIGNAL(exitToMenu(void)), this, SLOT(reload(void)));
        delete gameScr;
        gameScr = nullptr;
    }
    this->setFocus();
    ui->centralWidget->show();
}
