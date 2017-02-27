#ifndef MODELCONTAINER_H
#define MODELCONTAINER_H

#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMatrix4x4>
#include <QVector3D>

class ModelContainer
{
public:
    enum Type{
        Generic,
        Skybox,
        Titan,
        Planet,
        Moon,
        Star,
        Spaceship
    };

    explicit ModelContainer(ModelContainer* _parent = nullptr):parent(_parent){}
    ModelContainer(QVector3D _relativePos, QVector3D _relativeRot, QString _model, QString _tex, Type _t=Generic);
    ~ModelContainer();

    void addChild(ModelContainer & m);
    void addChild(ModelContainer * m);
    inline void setScale(float factor) {scale.setX(factor); scale.setY(factor); scale.setZ(factor);}
    inline void setScale(float x, float y, float z) {scale.setX(x); scale.setY(y); scale.setZ(z);}
    QVector3D getPos() const;
    QVector3D getRot() const;
    QVector3D getScale() const;
    QMatrix4x4 getModelMat() const;

    ModelContainer* parent;
    QVector3D position, rotation;
    QString model;
    QString tex;
    Type type;
    QVector3D scale;
    QVector<ModelContainer*> children;

};

#endif // MODELCONTAINER_H
