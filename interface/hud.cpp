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
}

HUD::~HUD()
{
    delete ui;
}

// protected
void HUD::resizeEvent(QResizeEvent *)
{
    // !TODO resize all ui elements
}

// private slots
void HUD::on_exitButton_clicked()
{
    emit togglePauseMenu();
}

void HUD::updateFPS()
{
    ui->FPSCounter->display(((80*frameCount+20*prevFrameCount)+50)/100);
    prevFrameCount = frameCount;
    frameCount = 0;
}

// public slots
void HUD::acceptFrame()
{
    frameCount++;
}

void HUD::toggle()
{
    if (this->isHidden())
        show();
    else
        hide();
}
