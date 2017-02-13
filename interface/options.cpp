#include "options.h"
#include "ui_options.h"
#include <QDebug>
#include <QMessageBox>
#include <QTextStream>

const QString Options::SETTING_GRAPHICS_AA = QString("ANTI_ALIASING");

Options::Options(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Options)
{
    ui->setupUi(this);
    loadDefaultSettings();
}

Options::~Options()
{
    delete ui;
}

void Options::showEvent(QShowEvent *)
{
    if (!fOpt.isOpen()) {
        fOpt.setFileName("settings.cfg");
        if (!fOpt.exists()) {
            qDebug() << "File settings.cfg doesn't exist, attemting to create one...";
            if (!fOpt.open(QIODevice::WriteOnly)) {
                QMessageBox err;
                err.critical(0, "Error","Could not create settings.cfg!");
                err.setFixedSize(640, 480);
                err.show();
            }
            else {
                qDebug() << "File settings.cfg created successfully!";
                // settings will be saved when hideEvent is called
            }
        }
        else {
            fOpt.open(QIODevice::ReadOnly);
            readSettings(fOpt);
        }
    }
    else // fOpt already opened
        qDebug() << "This wasn't supposed to happen... @Options::showEvent()";
    updateUI();
}

void Options::hideEvent(QHideEvent *)
{
    if (!fOpt.isOpen()) {
        qDebug() << "Could not save settings, file settings.cfg is not open.";
    }
    else {
        fOpt.close();
        if (!fOpt.open(QIODevice::WriteOnly)) {
            QMessageBox err;
            err.critical(0, "Error","Could not write to settings.cfg!");
            err.setFixedSize(640, 480);
            err.show();
        }
        else {
            QTextStream out(&fOpt);
            auto it = mData.begin();
            while (it != mData.end()) {
                out << it.key() << '=' << it.value() << '\n';
                ++it;
            }
            fOpt.close();
        }
    }
}

void Options::loadDefaultSettings()
{
    mData[SETTING_GRAPHICS_AA] = "No AA";
}

void Options::readSettings(QFile &file) {
    QTextStream in(&file);
    while (!file.atEnd()) {
        QString line = in.readLine();
        if (line[0] == '#' or line.size() < 3)
            continue;
        bool eq = false;
        QString left, right;
        for (QChar c: line) {
            if (eq)
                right += c;
            else {
                if (c == '=')
                    eq = true;
                else
                    left += c;
            }
        }
        mData[left] = right;
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
    mData[SETTING_GRAPHICS_AA] = arg1;
}

void Options::changePage(int laPage)
{
    ui->stackedWidget->setCurrentIndex(laPage);
}

void Options::updateUI()
{
    ui->AAbox->setCurrentText(mData[SETTING_GRAPHICS_AA]);
}
