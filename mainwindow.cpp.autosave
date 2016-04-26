#include "mainwindow.h"
#include "options.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    opt = new Options(this);
    opt->hide();
}

MainWindow::~MainWindow()
{
    delete opt;
    delete ui;
}

void MainWindow::on_exitButton_clicked()
{
    close();
}

void MainWindow::on_newGameButton_clicked()
{
    //Game g;
    //g.show();
}

void MainWindow::on_optionsButton_clicked()
{
    setCentralWidget(opt);
    opt->show();
}
