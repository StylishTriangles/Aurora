#include "modelcontainer.h"

ModelContainer::ModelContainer(QVector3D _pos, QVector3D _rot, QString _model, QString _tex, Type _t) :
    parent(nullptr),
    scale(1.0f,1.0f,1.0f)
{
    position=_pos;rotation=_rot;tex=_tex;model=_model;type=_t;
}

ModelContainer::~ModelContainer()
{
    for (auto p:children)
        delete p;
}

void ModelContainer::addChild(ModelContainer &m)
{
    m.parent = this;
    ModelContainer* nmc = new ModelContainer();
    *nmc = m;
    children.push_back(nmc);
}

void ModelContainer::addChild(ModelContainer* m)
{
    m->parent = this;
    children.push_back(m);
}

QVector3D ModelContainer::getPos() const
{
    if (parent == nullptr)
        return position;
    else
        return position+parent->getPos();
}

QVector3D ModelContainer::getRot() const
{
    if (parent == nullptr)
        return rotation;
    else
        return rotation+parent->getRot();
}

QVector3D ModelContainer::getScale() const
{
    if (parent == nullptr)
        return scale;
    else
        return scale*parent->getScale();
}

QMatrix4x4 ModelContainer::getModelMat() const
{
    QMatrix4x4 ret;
    ret.translate(position);
    ret.rotate(rotation.x(),1.0f,0.0f);
    ret.rotate(rotation.y(),0.0f,1.0f);
    ret.rotate(rotation.z(),0.0f,0.0f,1.0f);
    ret.scale(scale);
    if (type == Skybox) {
        return ret;
    }
    else {
        if (parent == nullptr)
            return ret;
        else
            return parent->getModelMat()*ret;
    }
}
