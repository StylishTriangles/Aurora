#include "modelcontainer.h"

ModelContainer::ModelContainer(QVector3D _pos, QVector3D _rot, QString _model, QString _tex, QOpenGLShaderProgram* _shader, Type _t) :
    parent(nullptr)
{
    pos=_pos;rot=_rot;tex=_tex;shader=_shader;model=_model;t=_t;scale=1.0f;
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

QVector3D ModelContainer::getPos()
{
    if (parent == nullptr)
        return pos;
    else
        return pos+parent->getPos();
}

QVector3D ModelContainer::getRot()
{
    if (parent == nullptr)
        return rot;
    else
        return rot+parent->getRot();
}

GLfloat ModelContainer::getScale()
{
    if (parent == nullptr)
        return scale;
    else
        return scale*parent->getScale();
}
