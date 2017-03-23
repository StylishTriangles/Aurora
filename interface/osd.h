#ifndef OSD_H
#define OSD_H

#include <QWidget>
#include <QTimer>
#include <QResizeEvent>

namespace Ui {
class OSD;
}

class OSD;

class OSD : public QWidget
{
    Q_OBJECT

public:
    explicit OSD(QWidget *parent = 0);
    ~OSD();

protected:
    void resizeEvent(QResizeEvent* ) Q_DECL_OVERRIDE;

private slots:
    void on_resumeButton_clicked();
    void on_exitToMenuButton_clicked();
    void on_exitToSystemButton_clicked();

    void on_optionsButton_clicked();

public slots:
    void showPauseMenu();
    void hidePauseMenu();
    void togglePauseMenu();

signals:
    void enterSettings();
    void quitGame();
    void quitAll();
    void unpause();

private:
    Ui::OSD *ui;
};

#endif // OSD_H
