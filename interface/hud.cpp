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
void HUD::resizeEvent(QResizeEvent *event)
{
    QPoint tmp = QPoint(event->size().width(), event->size().height());
    tmp.rx()-=ui->tools->width();
    tmp.setY(0);
    ui->tools->move(tmp);
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

void HUD::on_empire_clicked()
{
    emit buttonClicked(Button_Empire);
}

void HUD::on_fleets_clicked()
{
    emit buttonClicked(Button_Fleets);
}

void HUD::on_research_clicked()
{
    emit buttonClicked(Button_Research);
}

void HUD::on_diplomacy_clicked()
{
    emit buttonClicked(Button_Diplomacy);
}
