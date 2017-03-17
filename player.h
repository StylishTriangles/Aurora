#ifndef PLAYER_H
#define PLAYER_H

#include<QVector3D>
#include<QString>

class Player
{
private:
    QVector3D color;
    QString name;
    unsigned long long int idx;

public:
    Player() = default;
    Player(QString _name, QVector3D _color);
    void setName(QString newName);
    void setColor(QVector3D newColor);
    QString getName();
    QVector3D getColor();
};

#endif // PLAYER_H
