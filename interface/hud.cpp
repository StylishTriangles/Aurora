#include "hud.h"
#include "ui_hud.h"
#include <QDebug>

HUD::HUD(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HUD),
    deltaTms(1000),
    frameCount(0), prevFrameCount(0)
{
    ui->setupUi(this);
    fpsRefreshTimer.start(deltaTms);
    QObject::connect(&fpsRefreshTimer, SIGNAL(timeout()), this, SLOT(updateFPS()));
    hidePauseMenu();
}

HUD::~HUD()
{
    delete ui;
}

// protected
void HUD::resizeEvent(QResizeEvent *r)
{
    // !TODO resize all ui elements
}

// private slots
void HUD::on_exitButton_clicked()
{
    togglePauseMenu();
}

void HUD::updateFPS()
{
    ui->FPSCounter->display(((80*frameCount+20*prevFrameCount)+50)/100);
    prevFrameCount = frameCount;
    frameCount = 0;
}

void HUD::on_resumeButton_clicked()
{
    hidePauseMenu();
    emit unpause();
}

void HUD::on_exitToMenuButton_clicked()
{
    emit quitGame();
}

void HUD::on_exitToSystemButton_clicked()
{
    emit quitAll();
}

// public slots
void HUD::acceptFrame()
{
    frameCount++;
}

void HUD::showPauseMenu()
{
    ui->pauseMenu->show();
}

void HUD::hidePauseMenu()
{
    ui->pauseMenu->hide();
}

void HUD::togglePauseMenu()
{
    if (ui->pauseMenu->isHidden())
        ui->pauseMenu->show();
    else
        ui->pauseMenu->hide();
}
