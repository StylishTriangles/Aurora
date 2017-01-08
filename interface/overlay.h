#ifndef OVERLAY_H
#define OVERLAY_H

#include <QWidget>

namespace Ui {
class Overlay;
}

class Overlay : public QWidget
{
    Q_OBJECT

public:
    explicit Overlay(QWidget *parent = 0);
    ~Overlay();

private:
    Ui::Overlay *ui;
};

#endif // OVERLAY_H
