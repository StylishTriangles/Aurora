#ifndef OVERLAY_H
#define OVERLAY_H

#include <QWidget>
#include <QTimer>

namespace Ui {
class HUD;
}

class HUD : public QWidget
{
    Q_OBJECT

public:
    explicit HUD(QWidget *parent = 0);
    ~HUD();

private slots:
    void on_exitButton_clicked();
    void updateFPS();

public slots:
    void acceptFrame();

signals:
    void exit();

private:
    Ui::HUD *ui;
    QTimer fpsRefreshTimer;
    int deltaTms;
    int frameCount;
};

#endif // OVERLAY_H
