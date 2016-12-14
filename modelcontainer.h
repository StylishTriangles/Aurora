#ifndef MODELCONTAINER_H
#define MODELCONTAINER_H

#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMatrix4x4>

class Modelcontainer
{
public:
    enum Type{
        Generic,
        Planet,
        Star,
        Ship
    };

    Modelcontainer()=default;
    Modelcontainer(QVector3D _pos, QVector3D _rot, QString _model, QOpenGLTexture* _tex=nullptr,  QOpenGLShaderProgram* _shader=nullptr, Type _t=Generic);
    ~Modelcontainer()=default;
    QVector3D pos, rot;
    QOpenGLTexture* tex;
    QOpenGLShaderProgram* shader;
    QString model;
    Type t;
    GLfloat scale;
};

#endif // MODELCONTAINER_H
