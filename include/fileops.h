#ifndef AURORA_FILE_OPS
#define AURORA_FILE_OPS
#include <QFile>
#include <QHash>
#include <QString>

namespace Aurora {
    const QString DEFAULT_CONFIG_FILENAME = QString("settings.cfg");
    const QString SETTING_ALPHA_BUFFER_SIZE = QString("ALPHA_SIZE");
    const QString SETTING_GRAPHICS_AA = QString("ANTI_ALIASING");
    const QString SETTING_GRAPHICS_VSYNC = QString("VSYNC");

    bool readSettings(QFile &file, QHash<QString, QString> &mData);
    bool readSettings(QHash<QString, QString> &mData);
    bool writeSettings(QFile &file, const QHash<QString,QString> &mData);
    void loadDefaultSettings(QHash<QString,QString> &mData);
}

#endif // AURORA_FILE_OPS
