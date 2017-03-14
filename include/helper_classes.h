#ifndef HELPER_CLASSES_H
#define HELPER_CLASSES_H

#include<QVector3D>
#include<GL/gl.h>

class Light{
public:
    Light()=default;
    Light(QVector3D amb, QVector3D dif, QVector3D sp, GLfloat sh);
    QVector3D ambient;
    QVector3D diffuse;
    QVector3D specular;
    GLfloat shininess;
};

#endif // HELPER_CLASSES_H
