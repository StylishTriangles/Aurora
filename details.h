#ifndef DETAILS_H
#define DETAILS_H

#include <QDomDocument>
#include<QOpenGLBuffer>
#include<QString>
#include<QVector3D>
#include<QVector2D>
#include<QVector>

class Details
{
private:
    int owner;
    QString starType, name;
    QVector3D pos;
    QVector<int> population;
    QVector<QString*> type;
    QVector<QVector<double>> production, basicProduction;
    double food, energy, science, industry, foodToNext, indInQueue;
    QVector<QString> buildinds;
    QVector<QPair<QString, int> > buildQueue;
    QVector<int> buildTime;

public:
    ~Details();
    Details()=default;
    Details(int _owner, QVector<int> _population, QString _starType, QVector3D _pos);
    void setName(QString& newName);
    QString getName();
    void setOwner(int newOwner, QOpenGLBuffer &mData, QVector3D ownerCol, QVector2D pos);
    int getOwner();
    void setPlanetCount(int planetsInSystem);
    void setPlanetType(int planet, QString *typ);
    void setColonized(int planetNum);
    int isColonized(int planetNum);
    QPair<double, double> calculateSystem(const QDomDocument &mData, int flag);
    void calculateFood();
    void calculateEnergy();
    void calculateScience();
    void calculateIndustry();
    void calculateQueue(const QDomDocument &mData, int flag);
    void addPopulation();
    void addSystemBuilding(const QDomDocument &mData, QString buildName);
    void addToQueue(const QDomDocument &mData, QString s);

};

#endif // DETAILS_H
