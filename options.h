#ifndef OPTIONS_H
#define OPTIONS_H

#include <QWidget>

namespace Ui {
class Options;
}

class Options : public QWidget
{
    Q_OBJECT

public:
    explicit Options(QWidget *parent = 0);
    ~Options();
signals:
    void submenuExit();

private slots:
    void on_returnButton_clicked();

private:
    Ui::Options *ui;
};

#endif // OPTIONS_H
