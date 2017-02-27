#include "./include/mapgenerator.h"

#include<random>

void generateSolarSystems(QVector<ModelContainer *> &solarSystems){
    std::mt19937 rng((long long int)(new int(75192)));
    std::uniform_real_distribution<float> ufd(-20.0f, 20.0f), eps(-0.1f, 0.1f), alfa(0.0f, 2*M_PI);
    std::uniform_int_distribution<int> uid(1, 6);
    GLfloat x, y, d;
    int planetsInSystem, p;
    ModelContainer* tmpM, *tmpM2;
    for(int i=0; i<10; i++){ //generate stars
        x=ufd(rng);
        y=ufd(rng);
        for(int j=0; j<i; j++){
            while(QVector2D(solarSystems[j]->getPos().x()-x, solarSystems[j]->getPos().y()-y).length()<3.0f){
                x=ufd(rng);
                y=ufd(rng);
                j=0;
            }
        }
        tmpM = new ModelContainer({x, y, 0.0f}, {0.0f, 0.0f, 0.0f}, "geosphere", "earth", ModelContainer::Star);
        solarSystems.push_back(tmpM);
        tmpM2=new ModelContainer({0,0,0}, {0,0,0}, "geosphere", "skybox", ModelContainer::Skybox);
        tmpM2->setScale(50.0f);
        solarSystems.back()->addChild(tmpM2);
    }
    for(int i=0; i<10; i++){ //generate planets in systems
        planetsInSystem=uid(rng);
        for(int j=0; j<planetsInSystem; j++){
            d=2*j+3.0f+eps(rng);
            x=alfa(rng);
            y=eps(rng)+0.2f;
            tmpM=new ModelContainer({d*sinf(x), d*cosf(x), 0.0f}, {0.0f, 0.0f, 0.0f}, "geosphere", "earth", ModelContainer::Planet);
            tmpM->setScale(y);
            tmpM2= new ModelContainer({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, "geosphere", "atmosphere", ModelContainer::Planet);
            tmpM2->setScale(1.01f); // atmosfera
            tmpM->addChild(tmpM2);
            p=uid(rng);
            if(p>=5){ // księżyc
                d=2.0f+eps(rng);
                x=alfa(rng);
                tmpM2= new ModelContainer({y*d*sinf(x), y*d*cosf(x), 0.0f}, {0.0f, 0.0f, 0.0f}, "geosphere", "moon", ModelContainer::Moon);
                y=eps(rng)+0.2f;
                tmpM2->setScale(y);
                tmpM->addChild(tmpM2);
            }
            solarSystems[i]->addChild(tmpM);
        }
    }
    /*solarSystems.push_back(new ModelContainer({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, "geosphere", "earth", ModelContainer::Star));
    ModelContainer* tmpM = new ModelContainer({0,0,0}, {0,0,0}, "geosphere", "skybox", ModelContainer::Skybox);
    tmpM->setScale(50.0f);
    solarSystems.back()->addChild(tmpM);
    tmpM = new ModelContainer({3.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, "geosphere", "earth", ModelContainer::Planet);
    tmpM->setScale(0.1f);
    ModelContainer* tmpM2 = new ModelContainer({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, "geosphere", "atmosphere", ModelContainer::Planet);
    tmpM2->setScale(1.01f);
    tmpM->addChild(tmpM2);
    tmpM2 = new ModelContainer({0.2f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, "geosphere", "moon", ModelContainer::Moon);
    tmpM2->setScale(0.1f);
    tmpM->addChild(tmpM2);
    solarSystems.back()->addChild(tmpM);
    tmpM = new ModelContainer({4.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, "titan", "earth", ModelContainer::Titan);
    tmpM->setScale(0.1f);
    solarSystems.back()->addChild(tmpM);

    solarSystems.push_back(new ModelContainer({3.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, "geosphere", "earth", ModelContainer::Star));
    tmpM = new ModelContainer({0,0,0}, {0,0,0}, "geosphere", "skybox", ModelContainer::Skybox);
    tmpM->setScale(50.0f);
    solarSystems.back()->addChild(tmpM);
    tmpM = new ModelContainer({3.5f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, "geosphere", "earth", ModelContainer::Planet);
    tmpM->setScale(0.1f);
    tmpM2 = new ModelContainer({0.3f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, "geosphere", "moon", ModelContainer::Moon);
    tmpM2->setScale(0.1f);
    tmpM->addChild(tmpM2);
    tmpM2 = new ModelContainer({0.4f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, "geosphere", "moon", ModelContainer::Moon);
    tmpM2->setScale(0.1f);
    tmpM->addChild(tmpM2);
    solarSystems.back()->addChild(tmpM);*/
}
