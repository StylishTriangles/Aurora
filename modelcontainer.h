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
        Moon,
        Star,
        Ship
    };

    ModelContainer(ModelContainer* _parent = nullptr):parent(_parent){}
    ModelContainer(QVector3D _relativePos, QVector3D _relativeRot, QString _model, QString _tex,  QOpenGLShaderProgram* _shader=nullptr, Type _t=Generic);
    ~ModelContainer();

    void addChild(ModelContainer & m);
    void addChild(ModelContainer * m);
    QVector3D getPos();
    QVector3D getRot();
    float getScale();

    ModelContainer* parent;
    QVector3D pos, rot;
    QString model;
    QString tex;
    QOpenGLShaderProgram* shader;
    Type t;
    GLfloat scale;
    QVector<ModelContainer*> children;

};

#endif // MODELCONTAINER_H
