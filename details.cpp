#include "details.h"

Details::Details(int _owner, QVector<int> _colonizated, QString _starType, QVector3D _pos)
{
    owner=_owner;colonizated=_colonizated;starType=_starType;pos=_pos;
}

void Details::setOwner(int newOwner){
    owner=newOwner;
}

int Details::getOwner(){
    return owner;
}

void Details::setPlanetNumber(int planetsInSystem){
    colonizated.resize(planetsInSystem);
}
