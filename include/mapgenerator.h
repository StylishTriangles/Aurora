#ifndef MAPGENERATOR_H
#define MAPGENERATOR_H

#include<QVector>

#include<algorithm>

#include "modelcontainer.h"
#include "details.h"
#include "../include/ray_intersect.h"

void generateSolarSystems(QVector<ModelContainer*>& solarSystems, QVector<Details *> &solarDetails);

void addYellowDwarf(QVector<ModelContainer*>& solarSystems, QVector<Details*>& solarDetails, std::mt19937& rng, std::uniform_real_distribution<float> &eps, float x, float y);
void addWhiteDwarf(QVector<ModelContainer*>& solarSystems, QVector<Details*>& solarDetails, std::mt19937& rng, std::uniform_real_distribution<float> &eps, float x, float y);
void addBlueDwarf(QVector<ModelContainer*>& solarSystems, QVector<Details*>& solarDetails, std::mt19937& rng, std::uniform_real_distribution<float> &eps, float x, float y);
void addRedDwarf(QVector<ModelContainer*>& solarSystems, QVector<Details*>& solarDetails, std::mt19937& rng, std::uniform_real_distribution<float> &eps, float x, float y);
void addRedGiant(QVector<ModelContainer*>& solarSystems, QVector<Details*>& solarDetails, std::mt19937& rng, std::uniform_real_distribution<float> &eps, float x, float y);
void addBlueGiant(QVector<ModelContainer*>& solarSystems, QVector<Details*>& solarDetails, std::mt19937& rng, std::uniform_real_distribution<float> &eps, float x, float y);
void addRedSuperGiant(QVector<ModelContainer*>& solarSystems, QVector<Details*>& solarDetails, std::mt19937& rng, std::uniform_real_distribution<float> &eps, float x, float y);
void addBlueSuperGiant(QVector<ModelContainer*>& solarSystems, QVector<Details *>& solarDetails, std::mt19937& rng, std::uniform_real_distribution<float> &eps, float x, float y);

void generateEdges(QVector<ModelContainer *> &solarSystems, QVector<QVector<int>>& edges, QVector<GLfloat> &geom, QVector<GLfloat> &star, int edgesMax);

void addMoon(ModelContainer* mod, std::mt19937& rng, std::uniform_real_distribution<float> &eps, std::uniform_real_distribution<float> &alfa);
void addAtmo(ModelContainer* mod);

void addTitan(ModelContainer* mod, std::mt19937& rng, std::uniform_real_distribution<float> &eps, std::uniform_real_distribution<float> &alfa, int idx);
void addTerranPlanet(ModelContainer* mod, std::mt19937& rng, std::uniform_real_distribution<float>& eps, std::uniform_real_distribution<float>& alfa, int idx);
void addVolcanicPlanet(ModelContainer* mod, std::mt19937& rng, std::uniform_real_distribution<float> &eps, std::uniform_real_distribution<float> &alfa, int idx);

int posToIdx(QVector3D pos);

#endif // MAPGENERATOR_H
