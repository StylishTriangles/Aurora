#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QTimer>
#include "options.h"
#include "game.h"

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

private slots:
    void on_newGameButton_clicked();
    void on_optionsButton_clicked();
    void on_exitButton_clicked();

    void reload();

private:
    Ui::MainWindow *ui;
    Options* opt;
    Game* gameScr;

    // game related variables
    QThread* workerThread;
    GameWorker* gameWorker;
    QTimer* actionTimer;
    int tickDelayMs;
};

#endif // MAINWINDOW_H
