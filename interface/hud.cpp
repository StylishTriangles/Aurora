#include "hud.h"
#include "ui_hud.h"

HUD::HUD(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HUD),
    deltaTms(1000),
    frameCount(0)
{
    ui->setupUi(this);
    fpsRefreshTimer.start(deltaTms);
    QObject::connect(&fpsRefreshTimer, SIGNAL(timeout()), this, SLOT(updateFPS()));
}

HUD::~HUD()
{
    delete ui;
}

// private slots
void HUD::on_exitButton_clicked()
{
    emit exit();
}

void HUD::updateFPS()
{
    ui->FPSCounter->display(frameCount);
    frameCount = 0;
}

// public slots
void HUD::acceptFrame()
{
    frameCount++;
}
