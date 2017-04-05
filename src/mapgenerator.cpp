#include "./include/mapgenerator.h"
#include "./include/fileops.h"

#include<random>

void generateSolarSystems(QVector<ModelContainer *> &solarSystems, QVector<Details*> &solarDetails){
    int * a=new int(75192);
    long long int b=(long long int)a;
    qDebug()<<"Random Seed: "<<b;
    std::mt19937 rng(b);
    std::uniform_real_distribution<float> ufd(-15.0f, 15.0f), eps(-0.1f, 0.1f), alfa(0.0f, 2*M_PI);
    std::uniform_int_distribution<int> uid(1, 6), luid(1, 1000);
    GLfloat x, y;
    int planetsInSystem, p;
    QVector<QString> names;
    Aurora::readNames("../Aurora/data/systemnames.txt", names, rng); //read solar system names from file
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
        p=luid(rng);
        if(p<=800){
            if(p<=400){
                if(p<=200)
                    addWhiteDwarf(solarSystems, solarDetails, rng, eps, x, y);
                else
                    addYellowDwarf(solarSystems, solarDetails, rng, eps, x, y);
            }
            else{
                if(p<=600)
                    addBlueDwarf(solarSystems, solarDetails, rng, eps, x, y);
                else
                    addRedDwarf(solarSystems, solarDetails, rng, eps, x, y);
            }
        }
        else{
            if(p<=950){
                if(p<=875)
                    addRedGiant(solarSystems, solarDetails, rng, eps, x, y);
                else
                    addBlueGiant(solarSystems, solarDetails, rng, eps, x, y);
            }
            else{
                if(p<=975)
                    addRedSuperGiant(solarSystems, solarDetails, rng, eps, x, y);
                else
                    addBlueSuperGiant(solarSystems, solarDetails, rng, eps, x, y);
            }
        }
        solarDetails[i]->setName(names[i]);
    }
    for(int i=0; i<10; i++){ //generate planets in systems
        planetsInSystem=uid(rng);
        solarDetails[i]->setPlanetCount(planetsInSystem);
        for(int j=0; j<planetsInSystem; j++){
            p=luid(rng);
            if(p<=600)
            addTerranPlanet(solarSystems[i], *solarDetails[i], rng, eps, alfa, j);
            else{
                if(p>=800)
                    addVolcanicPlanet(solarSystems[i], *solarDetails[i], rng, eps, alfa, j);
                else
                    addTitan(solarSystems[i], *solarDetails[i], rng, eps, alfa, j);
            }
            //tmp
            p=luid(rng);
            if(p<=500)
                solarDetails[i]->setColonized(j);
        }
    }
}

void addYellowDwarf(QVector<ModelContainer*>& solarSystems, QVector<Details*>& solarDetails, std::mt19937& rng, std::uniform_real_distribution<float> &eps, float x, float y){
    ModelContainer* tmpM, *tmpM2;
    tmpM = new ModelContainer({x, y, 0.0f}, {0.0f, 0.0f, 0.0f}, "geosphere", "yellow_dwarf", ModelContainer::Star);
    tmpM->setScale(eps(rng)+0.3f);
    solarSystems.push_back(tmpM);
    tmpM2=new ModelContainer({0,0,0}, {0,0,0}, "geosphere", "skybox", ModelContainer::Skybox);
    tmpM2->setScale(50.0f);
    solarSystems.back()->addChild(tmpM2);
    tmpM2 = new ModelContainer({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, "sprite", "yellow_dwarf", ModelContainer::StarCorona);
    solarSystems.back()->addChild(tmpM2);
    Details* tmpD = new Details(-1, QVector<int>(), "yellow_dwarf", tmpM->position);
    solarDetails.push_back(tmpD);
}

void addWhiteDwarf(QVector<ModelContainer*>& solarSystems, QVector<Details*>& solarDetails, std::mt19937& rng, std::uniform_real_distribution<float> &eps, float x, float y){
    ModelContainer* tmpM, *tmpM2;
    tmpM = new ModelContainer({x, y, 0.0f}, {0.0f, 0.0f, 0.0f}, "geosphere", "white_dwarf", ModelContainer::Star);
    tmpM->setScale(eps(rng)+0.3f);
    solarSystems.push_back(tmpM);
    tmpM2=new ModelContainer({0,0,0}, {0,0,0}, "geosphere", "skybox", ModelContainer::Skybox);
    tmpM2->setScale(50.0f);
    solarSystems.back()->addChild(tmpM2);
    tmpM2 = new ModelContainer({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, "sprite", "white_dwarf", ModelContainer::StarCorona);
    solarSystems.back()->addChild(tmpM2);
    Details* tmpD = new Details(-1, QVector<int>(), "white_dwarf", tmpM->position);
    solarDetails.push_back(tmpD);
}

void addBlueDwarf(QVector<ModelContainer*>& solarSystems, QVector<Details *> &solarDetails, std::mt19937& rng, std::uniform_real_distribution<float> &eps, float x, float y){
    ModelContainer* tmpM, *tmpM2;
    tmpM = new ModelContainer({x, y, 0.0f}, {0.0f, 0.0f, 0.0f}, "geosphere", "blue_dwarf", ModelContainer::Star);
    tmpM->setScale(eps(rng)+0.3f);
    solarSystems.push_back(tmpM);
    tmpM2=new ModelContainer({0,0,0}, {0,0,0}, "geosphere", "skybox", ModelContainer::Skybox);
    tmpM2->setScale(50.0f);
    solarSystems.back()->addChild(tmpM2);
    tmpM2 = new ModelContainer({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, "sprite", "blue_dwarf", ModelContainer::StarCorona);
    solarSystems.back()->addChild(tmpM2);
    Details* tmpD = new Details(-1, QVector<int>(), "blue_dwarf", tmpM->position);
    solarDetails.push_back(tmpD);
}

void addRedDwarf(QVector<ModelContainer*>& solarSystems, QVector<Details*>& solarDetails, std::mt19937& rng, std::uniform_real_distribution<float> &eps, float x, float y){
    ModelContainer* tmpM, *tmpM2;
    tmpM = new ModelContainer({x, y, 0.0f}, {0.0f, 0.0f, 0.0f}, "geosphere", "red_dwarf", ModelContainer::Star);
    tmpM->setScale(eps(rng)+0.3f);
    solarSystems.push_back(tmpM);
    tmpM2=new ModelContainer({0,0,0}, {0,0,0}, "geosphere", "skybox", ModelContainer::Skybox);
    tmpM2->setScale(50.0f);
    solarSystems.back()->addChild(tmpM2);
    tmpM2 = new ModelContainer({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, "sprite", "red_dwarf", ModelContainer::StarCorona);
    solarSystems.back()->addChild(tmpM2);
    Details* tmpD = new Details(-1, QVector<int>(), "red_dwarf", tmpM->position);
    solarDetails.push_back(tmpD);
}

void addRedGiant(QVector<ModelContainer*>& solarSystems, QVector<Details*>& solarDetails, std::mt19937& rng, std::uniform_real_distribution<float> &eps, float x, float y){
    ModelContainer* tmpM, *tmpM2;
    tmpM = new ModelContainer({x, y, 0.0f}, {0.0f, 0.0f, 0.0f}, "geosphere", "red_giant", ModelContainer::Star);
    tmpM->setScale(eps(rng)+0.6f);
    solarSystems.push_back(tmpM);
    tmpM2=new ModelContainer({0,0,0}, {0,0,0}, "geosphere", "skybox", ModelContainer::Skybox);
    tmpM2->setScale(50.0f);
    solarSystems.back()->addChild(tmpM2);
    tmpM2 = new ModelContainer({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, "sprite", "red_giant", ModelContainer::StarCorona);
    solarSystems.back()->addChild(tmpM2);
    Details* tmpD = new Details(-1, QVector<int>(), "red_giant", tmpM->position);
    solarDetails.push_back(tmpD);
}

void addBlueGiant(QVector<ModelContainer*>& solarSystems, QVector<Details*>& solarDetails, std::mt19937& rng, std::uniform_real_distribution<float> &eps, float x, float y){
    ModelContainer* tmpM, *tmpM2;
    tmpM = new ModelContainer({x, y, 0.0f}, {0.0f, 0.0f, 0.0f}, "geosphere", "blue_giant", ModelContainer::Star);
    tmpM->setScale(eps(rng)+0.6f);
    solarSystems.push_back(tmpM);
    tmpM2=new ModelContainer({0,0,0}, {0,0,0}, "geosphere", "skybox", ModelContainer::Skybox);
    tmpM2->setScale(50.0f);
    solarSystems.back()->addChild(tmpM2);
    tmpM2 = new ModelContainer({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, "sprite", "blue_giant", ModelContainer::StarCorona);
    solarSystems.back()->addChild(tmpM2);
    Details* tmpD = new Details(-1, QVector<int>(), "blue_giant", tmpM->position);
    solarDetails.push_back(tmpD);
}

void addRedSuperGiant(QVector<ModelContainer*>& solarSystems, QVector<Details*>& solarDetails, std::mt19937& rng, std::uniform_real_distribution<float> &eps, float x, float y){
    ModelContainer* tmpM, *tmpM2;
    tmpM = new ModelContainer({x, y, 0.0f}, {0.0f, 0.0f, 0.0f}, "geosphere", "red_super_giant", ModelContainer::Star);
    tmpM->setScale(eps(rng)+1.0f);
    solarSystems.push_back(tmpM);
    tmpM2=new ModelContainer({0,0,0}, {0,0,0}, "geosphere", "skybox", ModelContainer::Skybox);
    tmpM2->setScale(50.0f);
    solarSystems.back()->addChild(tmpM2);
    tmpM2 = new ModelContainer({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, "sprite", "red_super_giant", ModelContainer::StarCorona);
    solarSystems.back()->addChild(tmpM2);
    Details* tmpD = new Details(-1, QVector<int>(), "red_super_giant", tmpM->position);
    solarDetails.push_back(tmpD);
}

void addBlueSuperGiant(QVector<ModelContainer*>& solarSystems, QVector<Details*>& solarDetails, std::mt19937& rng, std::uniform_real_distribution<float> &eps, float x, float y){
    ModelContainer* tmpM, *tmpM2;
    tmpM = new ModelContainer({x, y, 0.0f}, {0.0f, 0.0f, 0.0f}, "geosphere", "blue_super_giant", ModelContainer::Star);
    tmpM->setScale(eps(rng)+1.0f);
    solarSystems.push_back(tmpM);
    tmpM2=new ModelContainer({0,0,0}, {0,0,0}, "geosphere", "skybox", ModelContainer::Skybox);
    tmpM2->setScale(50.0f);
    solarSystems.back()->addChild(tmpM2);
    tmpM2 = new ModelContainer({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, "sprite", "blue_super_giant", ModelContainer::StarCorona);
    solarSystems.back()->addChild(tmpM2);
    Details* tmpD = new Details(-1, QVector<int>(), "blue_super_giant", tmpM->position);
    solarDetails.push_back(tmpD);
}

bool comp(QVector3D a, QVector3D b){
    return a.x() < b.x() or ((a.x()==b.x() and a.y()<b.y()) or (a.y() == b.y() and a.z() < b.z()));
}

void generateEdges(QVector<ModelContainer *> &solarSystems, QVector<QVector<int>>& edges, QVector<GLfloat>& geom, QVector<GLfloat>& star, int edgesMax){
    edges.resize(solarSystems.size());
    QVector<QVector3D> v;
    QMatrix4x4 tmp;
    v.reserve(solarSystems.size()*(solarSystems.size()-1));
    for(int i=0; i<solarSystems.size()-1; i++){
        for(int j=i+1; j<solarSystems.size(); j++){
            v.push_back(QVector3D((solarSystems[i]->position-solarSystems[j]->position).length(), i, j));
        }
    }
    std::sort(v.begin(), v.end(), comp);
    int rep[solarSystems.size()];
    for(int i=0; i<solarSystems.size(); i++)
        rep[i]=i;
    auto fin = [&rep](int a) -> int {int a1=a;while(rep[a1]!=a1) a1=rep[a1]; while(a!=a1){rep[a]=a1; a=rep[a];} return a;};
    auto unio = [&rep, fin](int a, int b) -> void {rep[fin(a)]=fin(b);};
    auto cr_pr = [](QVector2D a, QVector2D b) -> float {return a.x()*b.y()-a.y()*b.x();};
    for(int i=0; i<v.size(); i++){ // minimalne drzewo rozpinajace (MST)
        if(fin(v[i].y())!=fin(v[i].z())){
            unio(v[i].y(), v[i].z());
            edges[v[i].y()].push_back(v[i].z());
            edges[v[i].z()].push_back(v[i].y());
        }
    }
    auto checkIfCrossing = [&tmp, &star, &edges, &solarSystems, cr_pr](int a, int b) -> bool {QVector2D p1, p2, p3, p4;
        for(int i=0; i<edges.size(); i++){
            tmp=solarSystems[i]->getModelMat();
            tmp.scale(1.1f);
            if(i!=a && i!=b && testRayPreciselyIntersection(star, solarSystems[a]->position, (solarSystems[b]->position-solarSystems[a]->position).normalized(), tmp, 8, 0)!=-10e6)
                return true;
            for(int j=0; j<edges[i].size(); j++){
                p1=QVector2D(solarSystems[i]->position);
                p2=QVector2D(solarSystems[edges[i][j]]->position);
                p3=QVector2D(solarSystems[a]->position);
                p4=QVector2D(solarSystems[b]->position);
                if((a==i && b==edges[i][j]) || ((cr_pr(p2-p1, p3-p1)*cr_pr(p2-p1, p4-p1)<-0.001f) && ((cr_pr(p4-p3, p1-p3)*cr_pr(p4-p3, p2-p3))<-0.001f)))
                    return true;
            }
        }
        return false;};
    std::mt19937 rng((long long int)(new int(42369)));
    std::uniform_int_distribution<int> uid(0, solarSystems.size()-1);
    int a, b;
    for(int i=solarSystems.size(); i<edgesMax; i++){
        a=uid(rng);
        b=uid(rng);
        while(checkIfCrossing(a, b)){
            a=uid(rng);
            b=uid(rng);
        }
        edges[a].push_back(b);
        edges[b].push_back(a);
    }
    for(int i=0; i<edges.size(); i++){
        for(int j=0; j<edges[i].size(); j++){
            if(i<edges[i][j]){
                geom+=(solarSystems[i]->position.x());
                geom+=(solarSystems[i]->position.y());
                geom+=1.0f;
                geom+=1.0f;
                geom+=1.0f;
                geom+=(solarSystems[edges[i][j]]->position.x());
                geom+=(solarSystems[edges[i][j]]->position.y());
                geom+=1.0f;
                geom+=1.0f;
                geom+=1.0f;
            }
        }
    }
}

void addMoon(ModelContainer* mod, std::mt19937& rng, std::uniform_real_distribution<float> &eps, std::uniform_real_distribution<float> &alfa){
    float d, x, y;
    d=2.0f+eps(rng);
    x=alfa(rng);
    ModelContainer* tmp= new ModelContainer({d*sinf(x), d*cosf(x), 0.0f}, {0.0f, 0.0f, 0.0f}, "geosphere", "moon", ModelContainer::Moon);
    y=eps(rng)+0.2f;
    tmp->setScale(y);
    mod->addChild(tmp);
}

void addAtmo(ModelContainer* mod){
    ModelContainer* tmp= new ModelContainer({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, "geosphere", "atmosphere", ModelContainer::Atmo);
    tmp->setScale(1.01f);
    mod->addChild(tmp);
}

void addTitan(ModelContainer* mod, Details& det, std::mt19937& rng, std::uniform_real_distribution<float> &eps, std::uniform_real_distribution<float> &alfa, int idx){
    ModelContainer* tmp= new ModelContainer({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, "titan", "terran", ModelContainer::Titan);
    std::uniform_int_distribution<int> uid(1, 10);
    float d, x;
    d=2.0f*idx+3.0f+eps(rng);
    x=alfa(rng);
    tmp->position[0]=d*sinf(x);
    tmp->position[1]=d*cosf(x);
    x=alfa(rng);
    tmp->rotation[0]=x;
    x=alfa(rng);
    tmp->rotation[1]=x;
    tmp->setScale(eps(rng)+0.2f);
    int p=uid(rng);
    if(p>=8){
        addMoon(tmp, rng, eps, alfa);
    }
    mod->addChild(tmp);
    det.setPlanetType(idx, &tmp->model);
}

void addTerranPlanet(ModelContainer* mod, Details& det, std::mt19937& rng, std::uniform_real_distribution<float> &eps, std::uniform_real_distribution<float> &alfa, int idx){
    ModelContainer* tmp= new ModelContainer({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, "geosphere", "terran", ModelContainer::Planet);
    std::uniform_int_distribution<int> uid(1, 10);
    float d, x;
    d=2.0f*idx+3.0f+eps(rng);
    x=alfa(rng);
    tmp->position[0]=d*sinf(x);
    tmp->position[1]=d*cosf(x);
    x=alfa(rng);
    tmp->rotation[0]=x;
    x=alfa(rng);
    tmp->rotation[1]=x;
    tmp->setScale(eps(rng)+0.2f);
    addAtmo(tmp);
    int p=uid(rng);
    if(p>=8){
        addMoon(tmp, rng, eps, alfa);
    }
    mod->addChild(tmp);
    det.setPlanetType(idx, &tmp->tex);
}

void addVolcanicPlanet(ModelContainer* mod, Details& det, std::mt19937& rng, std::uniform_real_distribution<float> &eps, std::uniform_real_distribution<float> &alfa, int idx) {
    ModelContainer* tmp= new ModelContainer({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, "geosphere", "volcanic", ModelContainer::Planet);
    std::uniform_int_distribution<int> uid(1, 10);
    float d, x;
    d=2.0f*idx+3.0f+eps(rng);
    x=alfa(rng);
    tmp->position[0]=d*sinf(x);
    tmp->position[1]=d*cosf(x);
    x=alfa(rng);
    tmp->rotation[0]=x;
    x=alfa(rng);
    tmp->rotation[1]=x;
    tmp->setScale(eps(rng)+0.2f);
    addAtmo(tmp);
    int p=uid(rng);
    if(p>=8){
        addMoon(tmp, rng, eps, alfa);
    }
    mod->addChild(tmp);
    det.setPlanetType(idx, &tmp->tex);
}

int posToIdx(QVector3D pos){
    return int((roundf(pos.length())-3.0f)/2.0f);
}

float reduceScale(GLfloat sc){
    float minbefscale=0.1f, maxbefscale=3.0f, minaftscale=0.1f, maxaftscale=0.5f;
    return sc*(maxaftscale-minaftscale)/(maxbefscale-minbefscale)+maxaftscale-maxbefscale*(maxaftscale-minaftscale)/(maxbefscale-minbefscale);
}
