#include "details.h"

Details::Details(int _owner, QVector<int> _colonizated, QString _starType, QVector3D _pos)
{
    owner=_owner;colonized=_colonizated;starType=_starType;pos=_pos;
}

void Details::setOwner(int newOwner, QOpenGLBuffer& mData, QVector3D ownerCol, QVector2D pos){
    owner=newOwner;
    float a, b;
    mData.bind();
    for(int i=0; i<mData.size(); i+=5) {
        mData.read(sizeof(GLfloat)*i, &a, sizeof(GLfloat));
        mData.read(sizeof(GLfloat)*(i+1), &b, sizeof(GLfloat));
        if(a>=pos.x()-0.01f && a<=pos.x()+0.01f && b>=pos.y()-0.01f && b<=pos.y()+0.01f) {
            mData.write(sizeof(GLfloat)*(i+2), &ownerCol, 3*sizeof(GLfloat));
        }
    }
}

int Details::getOwner(){
    return owner;
}

void Details::setPlanetCount(int planetsInSystem){
    colonized.resize(planetsInSystem);
}

void Details::setColonized(int planetNum){
    colonized[planetNum]=1;
}

int Details::isColonized(int planetNum){
    return colonized[planetNum];
}

QPair<double, double> Details::calculateSystem(int flag){
    calculateFood();
    if(flag==1){
        foodToNext-=food;
        if(foodToNext<=0){
            addPopulation();
        }
    }
    calculateEnergy();
    calculateScience();
    calculateIndustry();
    qDebug()<<food<<" "<<energy<<" "<<science<<" "<<industry;
    return {energy, science};
}

void Details::calculateFood(){
    for(int i=0; i<colonized.size(); i++){
        if(colonized[i]>0){
            food+=5+2*colonized[i]; //very very tmp
        }
        food-=colonized[i];
    }
}

void Details::calculateEnergy(){
    for(int i=0; i<colonized.size(); i++){
        if(colonized[i]>0){
            energy+=5+2*colonized[i]; //very very tmp
        }
    }
}

void Details::calculateScience(){
    for(int i=0; i<colonized.size(); i++){
        if(colonized[i]>0){
            science+=5+2*colonized[i]; //very very tmp
        }
    }

}

void Details::calculateIndustry(){
    for(int i=0; i<colonized.size(); i++){
        if(colonized[i]>0){
            industry+=5+2*colonized[i]; //very very tmp
        }
    }
}

void Details::addPopulation(){
    int idx=-1, pop;
    double sum=0, tmp;
    for(int i=0; i<colonized.size(); i++){
        if(colonized[i]==0)
            continue;
        tmp=0;
        for(int j=0; j<4; j++){
            tmp+=production[i][j];
        }
        if(tmp>sum){
            idx=i;
            sum=tmp;
        }
    }
    if(idx!=-1)
        colonized[i]++;
    foodToNext=pop*30;
}

