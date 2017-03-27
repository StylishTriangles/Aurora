#ifndef HELPER_CLASSES_H
#define HELPER_CLASSES_H

#include<QVector3D>
#include<GL/gl.h>

class Light;

class Light{
public:
    Light()=default;
    constexpr Light(QVector3D amb, QVector3D dif, QVector3D sp):
        ambient(amb),diffuse(dif),specular(sp) {}
    Light(float temperature);

    void blackbody(float temperature);

    friend constexpr Light operator * (const Light& l, float m);
    friend constexpr Light operator * (float m, const Light& l);
    friend constexpr Light operator / (const Light& l, float m);
    friend constexpr Light operator * (const Light& l, const Light& r);

private:
    QVector3D getTempColorShift(float temperature);
    QVector3D kelvinToRGB(float temperature);

public: // maciekcotyrobisz
    QVector3D ambient;
    QVector3D diffuse;
    QVector3D specular;

};

constexpr Light operator * (const Light& l, float m)
{return Light(l.ambient*m, l.diffuse*m, l.specular*m);}

constexpr Light operator * (float m, const Light& l)
{return Light(l.ambient*m, l.diffuse*m, l.specular*m);}

constexpr Light operator * (const Light& l, const Light& r)
{return Light(l.ambient*r.ambient, l.diffuse*r.diffuse, l.specular*r.diffuse);}

constexpr Light operator / (const Light& l, float m)
{return Light(l.ambient/m, l.diffuse/m, l.specular/m);}

#endif // HELPER_CLASSES_H
