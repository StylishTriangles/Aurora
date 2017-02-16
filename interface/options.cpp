#include "options.h"
#include "ui_options.h"
#include <QDebug>
#include <QMessageBox>
#include <QTextStream>
#include "include/fileops.h"


Options::Options(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Options)
{
    ui->setupUi(this);

    Aurora::loadDefaultSettings(mData);
    // read settings
    fOpt.setFileName("settings.cfg");
    if (!fOpt.exists()) {
        qDebug() << "[INFO] File settings.cfg doesn't exist, attemting to create one...";
        if (!fOpt.open(QIODevice::WriteOnly)) {
            QMessageBox err;
            err.critical(0, "Error","Could not create settings.cfg!");
            err.setFixedSize(640, 480);
            err.show();
        }
        else {
            qDebug() << "[INFO] File settings.cfg created successfully!";
            fOpt.close();
            // settings will be saved in destructor
        }
    }
    else {
        if (!fOpt.open(QIODevice::ReadOnly))
            qDebug() << "[WARNING] Could not open settings file for reading";
        else {
            Aurora::readSettings(fOpt, mData);
            fOpt.close();
        }
    }
}

Options::~Options()
{
    if (fOpt.exists()) {
        if (!fOpt.open(QIODevice::WriteOnly))
            qDebug() << "[WARNING] Could not open settings file for writing";
        else {
            Aurora::writeSettings(fOpt, mData);
            fOpt.close();
        }
    }

    delete ui;
}

void Options::showEvent(QShowEvent *)
{
    updateUI();
}

void Options::hideEvent(QHideEvent *)
{
    if (fOpt.exists()) {
        if (!fOpt.open(QIODevice::WriteOnly))
            qDebug() << "[WARNING] Could not open settings file for writing";
        else {
            Aurora::writeSettings(fOpt, mData);
            fOpt.close();
        }
    }
}

void Options::on_pushButton_clicked()
{
    changePage(0);
}

void Options::on_pushButton_2_clicked()
{
    changePage(1);
}

void Options::on_pushButton_3_clicked()
{
    changePage(2);
}

void Options::on_pushButton_4_clicked()
{
    this->hide();
    emit submenuExit();
}

//void Options::writeCurrentSettings(QFile &file)
//{

//}

void Options::on_AAbox_currentTextChanged(const QString &arg1)
{
    mData[Aurora::SETTING_GRAPHICS_AA] = arg1;
}

void Options::on_VSbox_currentTextChanged(const QString &arg1)
{
    mData[Aurora::SETTING_GRAPHICS_VSYNC] = arg1;
}

void Options::changePage(int laPage)
{
    ui->stackedWidget->setCurrentIndex(laPage);
}

void Options::updateUI()
{
    ui->AAbox->setCurrentText(mData[Aurora::SETTING_GRAPHICS_AA]);
    ui->VSbox->setCurrentText(mData[Aurora::SETTING_GRAPHICS_VSYNC]);
}
