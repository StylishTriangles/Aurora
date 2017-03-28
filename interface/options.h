#ifndef OPTIONS_H
#define OPTIONS_H

#include <QWidget>
#include <QFile>
#include <QHash>
#include <QShowEvent>
#include <QString>

namespace Ui {
class Options;
}

class Options : public QWidget
{
    Q_OBJECT

public:
    explicit Options(QWidget *parent = 0);
    ~Options();

protected:
    void showEvent(QShowEvent*) Q_DECL_OVERRIDE;
    void hideEvent(QHideEvent*) Q_DECL_OVERRIDE;

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();

    void on_AAbox_currentTextChanged(const QString &arg1);
    void on_VSbox_currentTextChanged(const QString &arg1);
    void on_LSbox_currentTextChanged(const QString &arg1);

protected:
    QFile fOpt;
    QHash<QString,QString> mData;

private:
    void changePage(int laPage);
    void updateUI();
    Ui::Options *ui;

signals:
    void submenuExit();
};

#endif // OPTIONS_H
