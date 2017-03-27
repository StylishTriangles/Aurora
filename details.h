#ifndef DETAILS_H
#define DETAILS_H

#include<QVector3D>
#include<QVector2D>
#include<QVector>
#include<QString>
#include<QOpenGLBuffer>

class Details
{
private:
    int owner;
    QString starType;
    QVector3D pos;
    QVector<int> colonized;
    QVector<QString*> type;
    QVector<QVector<double>> production;
    double food, energy, science, industry, foodToNext;

public:
    ~Details();
    Details()=default;
    Details(int _owner, QVector<int> _colonizated, QString _starType, QVector3D _pos);
    void setOwner(int newOwner, QOpenGLBuffer &mData, QVector3D ownerCol, QVector2D pos);
    int getOwner();
    void setPlanetCount(int planetsInSystem);
    void setPlanetType(int planet, QString *typ);
    void setColonized(int planetNum);
    int isColonized(int planetNum);
    QPair<double, double> calculateSystem(int flag);
    void calculateFood();
    void calculateEnergy();
    void calculateScience();
    void calculateIndustry();
    void addPopulation();
};

#endif // DETAILS_H
