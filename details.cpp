#include "details.h"

#include<QDebug>

Details::~Details(){
    for(auto i:type)
        delete i;
}

Details::Details(int _owner, QVector<int> _population, QString _starType, QVector3D _pos)
{
    owner=_owner;population=_population;starType=_starType;pos=_pos;
}

void Details::setName(QString& newName){
    name=newName;
}

QString Details::getName(){
    return name;
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
        production[i].resize(5);
    }
    basicProduction.resize(planetsInSystem);
    for(int i=0; i<production.size(); i++){
        basicProduction[i].resize(4);
    }
    type.resize(planetsInSystem);
}

void Details::setPlanetType(int planet, QString * typ){
    type[planet]=typ;
    if(*typ=="terran"){
        production[planet][0]=4;//food
        production[planet][1]=2;//energy
        production[planet][2]=2;//science
        production[planet][3]=4;//industry
        production[planet][4]=8;//max pop
    }
    if(*typ=="titan"){
        production[planet][0]=0;//food
        production[planet][1]=0;//energy
        production[planet][2]=10;//science
        production[planet][3]=5;//industry
        production[planet][4]=4;//max pop
    }
    if(*typ=="volcanic"){
        production[planet][0]=1;//food
        production[planet][1]=6;//energy
        production[planet][2]=2;//science
        production[planet][3]=2;//industry
        production[planet][4]=6;//max pop
    }
    basicProduction[planet][0]=5;//food
    basicProduction[planet][1]=0;//energy
    basicProduction[planet][2]=0;//science
    basicProduction[planet][3]=5;//industry
}

void Details::setColonized(int planetNum){
    population[planetNum]=1;
}

int Details::isColonized(int planetNum){
    return population[planetNum];
}

QPair<double, double> Details::calculateSystem(int flag){
    if(owner==-1){
        food=0;
        energy=0;
        science=0;
        industry=0;
        return {0, 0};
    }
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
    qDebug()<<food<<" "<<energy<<" "<<science<<" "<<industry<<" "<<basicProduction[0][0];
    return {energy, science};
}

void Details::calculateFood(){
    food=0;
    for(int i=0; i<population.size(); i++){
        if(population[i]>0){
            food+=basicProduction[i][0]+production[i][0]*population[i];
        }
        food-=population[i];
    }
}

void Details::calculateEnergy(){
    energy=0;
    for(int i=0; i<population.size(); i++){
        if(population[i]>0){
            energy+=basicProduction[i][1]+production[i][1]*population[i];
        }
    }
}

void Details::calculateScience(){
    science=0;
    for(int i=0; i<population.size(); i++){
        if(population[i]>0){
            science+=basicProduction[i][2]+production[i][2]*population[i];
        }
    }

}

void Details::calculateIndustry(){
    industry=0;
    for(int i=0; i<population.size(); i++){
        if(population[i]>0){
            industry+=basicProduction[i][3]+production[i][3]*population[i];
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
        if(population[i]<production[i][4]){
            for(int j=0; j<4; j++){
                tmp+=production[i][j];
            }
            if(tmp>sum){
                idx=i;
                sum=tmp;
            }
        }
    }
    if(idx!=-1)
        population[idx]++;
    foodToNext=pop*30;
}

void Details::addSystemBuilding(const QDomDocument& mData, QString buildName){
    if(buildinds.contains(buildName))
        return;
    buildinds.push_back(buildName);
    QDomElement root=mData.documentElement(), building, effect, resource;
    building=root.firstChildElement();
    double foo, ene, sci, ind;
    QString restr;
    while(!building.isNull()){
        effect=building.firstChildElement();
        while(!effect.isNull()){
            resource=effect.firstChildElement();
            while(!resource.isNull()){
                restr=resource.attribute("Restrictions", "");
                foo=resource.attribute("Food", "0").toDouble();
                ene=resource.attribute("Energy", "0").toDouble();
                sci=resource.attribute("Science", "0").toDouble();
                ind=resource.attribute("Industry", "0").toDouble();
                if(effect.tagName()=="resourcesPerCapita"){
                    if(restr.size()==0){
                        for(int i=0; i<production.size(); i++){
                            production[i][0]+=foo;
                            production[i][1]+=ene;
                            production[i][2]+=sci;
                            production[i][3]+=ind;
                        }
                    }
                    else{
                        for(int i=0; i<type.size(); i++){
                            if(restr.contains(*type[i])){
                                production[i][0]+=foo;
                                production[i][1]+=ene;
                                production[i][2]+=sci;
                                production[i][3]+=ind;
                            }
                        }
                    }
                }
                else if(effect.tagName()=="resourcesPerPlanet"){
                    if(restr.size()==0){
                        for(int i=0; i<production.size(); i++){
                            basicProduction[i][0]+=foo;
                            basicProduction[i][1]+=ene;
                            basicProduction[i][2]+=sci;
                            basicProduction[i][3]+=ind;
                        }
                    }
                    else{
                        for(int i=0; i<type.size(); i++){
                            if(restr.contains(*type[i])){
                                basicProduction[i][0]+=foo;
                                basicProduction[i][1]+=ene;
                                basicProduction[i][2]+=sci;
                                basicProduction[i][3]+=ind;
                            }
                        }
                    }
                }
                resource = resource.nextSiblingElement();
            }
            effect = effect.nextSiblingElement();
        }
        building = building.nextSiblingElement();
    }
}
