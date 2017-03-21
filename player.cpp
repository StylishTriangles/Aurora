#include "player.h"

Player::Player(QString _name, QVector3D _color)
{
    name=_name; color=_color;idx=std::hash<std::string>{}(name.toStdString());
}

void Player::setName(QString newName){
    name=newName;
}

void Player::setColor(QVector3D newColor){
    color=newColor;
}

QString Player::getName(){
    return name;
}

QVector3D Player::getColor(){
    return color;
}
