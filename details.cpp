#include "details.h"

#include<QDebug>

Details::~Details(){
    for(auto i:type)
        delete i;
}

Details::Details(int _owner, QVector<int> _colonizated, QString _starType, QVector3D _pos)
{
    owner=_owner;population=_colonizated;starType=_starType;pos=_pos;
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
    population.resize(planetsInSystem);
    production.resize(planetsInSystem);
    for(int i=0; i<production.size(); i++){
        production[i].resize(4);
    }
    type.resize(planetsInSystem);
}

void Details::setPlanetType(int planet, QString * typ){
    type[planet]=typ;
    if(*typ=="earth"){
        production[planet][0]=4;//food
        production[planet][1]=2;//energy
        production[planet][2]=2;//science
        production[planet][3]=4;//industry
    }
    if(*typ=="titan"){
        production[planet][0]=0;//food
        production[planet][1]=0;//energy
        production[planet][2]=10;//science
        production[planet][3]=5;//industry
    }
    if(*typ=="venus"){
        production[planet][0]=1;//food
        production[planet][1]=6;//energy
        production[planet][2]=2;//science
        production[planet][3]=2;//industry
    }
}

void Details::setColonized(int planetNum){
    population[planetNum]=1;
}

int Details::isColonized(int planetNum){
    return population[planetNum];
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
    food=0;
    for(int i=0; i<population.size(); i++){
        if(population[i]>0){
            food+=5+production[i][0]*population[i]; //very very tmp
        }
        food-=population[i];
    }
}

void Details::calculateEnergy(){
    energy=0;
    for(int i=0; i<population.size(); i++){
        if(population[i]>0){
            energy+=5+production[i][1]*population[i]; //very very tmp
        }
    }
}

void Details::calculateScience(){
    science=0;
    for(int i=0; i<population.size(); i++){
        if(population[i]>0){
            science+=5+production[i][2]*population[i]; //very very tmp
        }
    }

}

void Details::calculateIndustry(){
    industry=0;
    for(int i=0; i<population.size(); i++){
        if(population[i]>0){
            industry+=5+production[i][3]*population[i]; //very very tmp
        }
    }
}

void Details::addPopulation(){
    int idx=-1, pop = 0;
    double sum=0, tmp;
    for(int i=0; i<population.size(); i++){
        pop+=population[i];
        if(population[i]==0)
            continue;
        tmp=0;
        for(int j=0; j<4; j++){
            //tmp+=production[i][j];
        }
        if(tmp>sum){
            idx=i;
            sum=tmp;
        }
    }
    if(idx!=-1)
        population[idx]++;
    foodToNext=pop*30;
}

