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

    void on_resumeButton_clicked();
    void on_exitToMenuButton_clicked();
    void on_exitToSystemButton_clicked();

    void on_optionsButton_clicked();

public slots:
    void acceptFrame();
    void showPauseMenu();
    void hidePauseMenu();
    void togglePauseMenu();

signals:
    void enterSettings();
    void quitGame();
    void quitAll();
    void unpause();

private:
    Ui::HUD *ui;
    QTimer fpsRefreshTimer;
    int deltaTms;
    int frameCount;
    int prevFrameCount;
};

#endif // OVERLAY_H
