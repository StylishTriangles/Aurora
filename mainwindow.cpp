#include "mainwindow.h"
#include "options.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    close();
}

void MainWindow::on_pushButton_2_clicked()
{
    //Game g;
    //g.show();
}

void MainWindow::on_pushButton_3_clicked()
{
    Options O(this);
    O.show();
    //this->hide();
}
