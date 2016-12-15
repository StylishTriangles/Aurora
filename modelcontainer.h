#ifndef MODELCONTAINER_H
#define MODELCONTAINER_H

#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMatrix4x4>

class ModelContainer
{
public:
    enum Type{
        Generic,
        Planet,
        Star,
        Ship
    };

    ModelContainer()=default;
    ModelContainer(QVector3D _pos, QVector3D _rot, QString _model, QOpenGLTexture* _tex=nullptr,  QOpenGLShaderProgram* _shader=nullptr, Type _t=Generic);
    ~ModelContainer();

    void addChild(ModelContainer const& m);

    QVector3D pos, rot;
    QOpenGLTexture* tex;
    QOpenGLShaderProgram* shader;
    QString model;
    Type t;
    GLfloat scale;
    QVector<ModelContainer*> children;
};

#endif // MODELCONTAINER_H
