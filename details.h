#ifndef DETAILS_H
#define DETAILS_H

#include<QVector3D>
#include<QVector>
#include<QString>
#include<QOpenGLBuffer>

class Details
{
private:
    int owner;
    QVector<int> colonizated;
    QString starType;
    QVector3D pos;

public:
    Details()=default;
    Details(int _owner, QVector<int> _colonizated, QString _starType, QVector3D _pos);
    void setOwner(int newOwner);
    int getOwner();
    void setPlanetNumber(int planetsInSystem);
};

#endif // DETAILS_H
