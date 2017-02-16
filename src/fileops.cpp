#include <QDebug>
#include <QTextStream>
#include "../include/fileops.h"

namespace Aurora {

bool readSettings(QFile &file, QHash<QString,QString> &mData)
{
    if (!file.isOpen())
        return true;
    QTextStream in(&file);
    while (!in.atEnd()) {
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
    return false;
}

bool readSettings(QHash<QString, QString> &mData)
{
    QFile f(DEFAULT_CONFIG_FILENAME);
    if (!f.open(QIODevice::ReadOnly))
        return true;
    return readSettings(f, mData);
}

bool writeSettings(QFile &file, const QHash<QString, QString> &mData)
{
    if (!file.isOpen())
        return true;
    else {
        QTextStream out(&file);
        auto it = mData.begin();
        while (it != mData.end()) {
            out << it.key() << '=' << it.value() << '\n';
            ++it;
        }
    }
    return false;
}

void loadDefaultSettings(QHash<QString,QString> &mData)
{
    mData[SETTING_ALPHA_BUFFER_SIZE] = "8";
    mData[SETTING_GRAPHICS_AA] = "No AA";
    mData[SETTING_GRAPHICS_VSYNC] = "Double Buffering";
}
} // namespace Aurora
