#ifndef AURORA_FILE_OPS
#define AURORA_FILE_OPS
#include <QDomDocument>
#include <QFile>
#include <QHash>
#include <QString>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <GL/gl.h>

namespace Aurora {
const QString DEFAULT_CONFIG_FILENAME = QString("settings.cfg");
const QString SETTING_ALPHA_BUFFER_SIZE = QString("ALPHA_SIZE");
const QString SETTING_GRAPHICS_AA = QString("ANTI_ALIASING");
const QString SETTING_GRAPHICS_LOADING_SCREEN = QString("LOAD_SCR");
const QString SETTING_GRAPHICS_VSYNC = QString("VSYNC");

bool readNames(const QString &filepath, QVector<QString> &mData, std::mt19937 &rng);
bool readSettings(QFile &file, QHash<QString,QString> &mData);
bool readSettings(QHash<QString, QString> &mData);
bool writeSettings(QFile &file, const QHash<QString,QString> &mData);
void loadDefaultSettings(QHash<QString,QString> &mData);
bool parseObj(const QString &filepath, QVector<GLfloat> &mData);
bool getXMLFile(const QString& filepath, QDomDocument& mData);
}

#endif // AURORA_FILE_OPS
