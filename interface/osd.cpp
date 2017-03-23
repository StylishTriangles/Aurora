#include "osd.h"
#include "ui_osd.h"
#include <QDebug>

OSD::OSD(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OSD)
{
    ui->setupUi(this);
}

OSD::~OSD()
{
    delete ui;
}

// protected
void OSD::resizeEvent(QResizeEvent *)
{
    // !TODO resize all ui elements
}

// private slots

void OSD::on_resumeButton_clicked()
{
    hidePauseMenu();
    emit unpause();
}

void OSD::on_exitToMenuButton_clicked()
{
    emit quitGame();
}

void OSD::on_exitToSystemButton_clicked()
{
    emit quitAll();
}

// public slots

void OSD::showPauseMenu()
{
    ui->pauseMenu->show();
}

void OSD::hidePauseMenu()
{
    ui->pauseMenu->hide();
}

void OSD::togglePauseMenu()
{
    if (ui->pauseMenu->isHidden() or this->isHidden()) {
        showPauseMenu();
        this->show();
    }
    else {
        hidePauseMenu();
        this->hide();
    }
}

void OSD::on_optionsButton_clicked()
{
    emit enterSettings();
}
