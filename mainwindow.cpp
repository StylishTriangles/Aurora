#include "mainwindow.h"
#include "options.h"
#include "game.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    opt = new Options(this);
    opt->hide();
    gameScr = new Game(this);
    gameScr->hide();
    // polaczenie sygnalu wyjscia submenuExit() klasy opt ze slotem show() klasy centralWidget
    QObject::connect(opt,SIGNAL(submenuExit(void)),ui->centralWidget,SLOT(show()));
}

MainWindow::~MainWindow()
{
    delete opt;
    delete ui;
}

// private slots
void MainWindow::on_exitButton_clicked()
{
    close();
}

void MainWindow::on_newGameButton_clicked()
{
    ui->centralWidget->hide();
    gameScr->show();
}

void MainWindow::on_optionsButton_clicked()
{
    ui->centralWidget->hide();
    opt->show();
}
