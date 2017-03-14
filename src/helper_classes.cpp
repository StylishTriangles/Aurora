#include "../include/helper_classes.h"

Light::Light(QVector3D amb, QVector3D dif, QVector3D sp, GLfloat sh)
{
    ambient=amb;diffuse=dif;specular=sp;shininess=sh;
}
