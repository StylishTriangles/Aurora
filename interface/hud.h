#ifndef OVERLAY_H
#define OVERLAY_H

#include <QWidget>
#include <QTimer>
#include <QResizeEvent>

namespace Ui {
class HUD;
}

class HUD;

class HUD : public QWidget
{
    Q_OBJECT

public:
    explicit HUD(QWidget *parent = 0);
    ~HUD();
    enum Buttons {
        Button_Empire,
        Button_Fleets,
        Button_Research,
        Button_Diplomacy
    };

protected:
    void resizeEvent(QResizeEvent* ) Q_DECL_OVERRIDE;

private slots:
    void on_exitButton_clicked();
    void updateFPS();
    void on_empire_clicked();
    void on_fleets_clicked();
    void on_research_clicked();
    void on_diplomacy_clicked();

public slots:
    void acceptFrame();
    void toggle();

signals:
    void enterSettings();
    void togglePauseMenu();
    void buttonClicked(int buttonID);

private:
    Ui::HUD *ui;
    QTimer fpsRefreshTimer;
    int deltaTms;
    int frameCount;
    int prevFrameCount;
};

#endif // OVERLAY_H
