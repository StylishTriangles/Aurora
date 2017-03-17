#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QTimer>
#include "options.h"
#include "game.h"
#include "hud.h"
#include "levels/arkanoid.h"

using Aurora::ArkanoidWidget;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void resizeEvent(QResizeEvent*) Q_DECL_OVERRIDE;

    void loadGameSettings();

protected slots:
    void paintEvent(QPaintEvent* ) Q_DECL_OVERRIDE;

private slots:
    void on_newGameButton_clicked();
    void on_optionsButton_clicked();
    void on_exitButton_clicked();

    void reload();

    void on_minigameButton_clicked();

signals:
    void sigLoadGame();

private:
    void unloadGame(); // free Game's memory

    Ui::MainWindow *ui;
    Options* opt;
    Game* gameScr;
    HUD* wHUD;
    ArkanoidWidget* wArkanoid;

    // game related variables
    QThread *workerThread, *loaderThread;
    GameWorker *gameWorker, *gameLoader;
    QTimer* actionTimer;
    QTimer wololoTimer;
    int tickDelayMs;
    int paintCounter;
};

#endif // MAINWINDOW_H
