#include "modelcontainer.h"

ModelContainer::ModelContainer(QVector3D _pos, QVector3D _rot, QString _model, QOpenGLTexture* _tex, QOpenGLShaderProgram* _shader, Type _t)
{
    pos=_pos;rot=_rot;tex=_tex;shader=_shader;model=_model;t=_t;scale=1.0f;
}

ModelContainer::~ModelContainer()
{
    for (auto p:children)
        delete p;
}

void ModelContainer::addChild(const ModelContainer &m)
{
    ModelContainer* nmc = new ModelContainer();
    *nmc = m;
    children.push_back(nmc);
}
