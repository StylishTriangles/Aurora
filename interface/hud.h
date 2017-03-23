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

protected:
    void resizeEvent(QResizeEvent* ) Q_DECL_OVERRIDE;

private slots:
    void on_exitButton_clicked();
    void updateFPS();

public slots:
    void acceptFrame();
    void toggle();

signals:
    void enterSettings();
    void togglePauseMenu();

private:
    Ui::HUD *ui;
    QTimer fpsRefreshTimer;
    int deltaTms;
    int frameCount;
    int prevFrameCount;
};

#endif // OVERLAY_H
