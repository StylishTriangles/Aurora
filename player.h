#ifndef PLAYER_H
#define PLAYER_H

#include<QVector3D>
#include<QVector>
#include<QString>

class Player
{
private:
    QVector3D color;
    QString name;
    unsigned long long int idx;
    double energy;
    QVector<int> ownedSystems;

public:
    Player() = default;
    Player(QString _name, QVector3D _color);
    void setName(QString newName);
    void setColor(QVector3D newColor);
    void ownSystem(int sys);
    void looseSystem(int sys);
    QString getName();
    QVector3D getColor();
};

#endif // PLAYER_H
