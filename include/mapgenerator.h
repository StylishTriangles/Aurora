#ifndef MAPGENERATOR_H
#define MAPGENERATOR_H

#include<QVector>

#include<algorithm>

#include "modelcontainer.h"
#include "../include/ray_intersect.h"

void generateSolarSystems(QVector<ModelContainer*>& solarSystems);

void generateEdges(QVector<ModelContainer *> &solarSystems, QVector<QVector<int>>& edges, QVector<GLfloat> &geom, QVector<GLfloat> &star, int edgesMax);
void addMoon(ModelContainer* mod, std::mt19937& rng, std::uniform_real_distribution<float> &eps, std::uniform_real_distribution<float> &alfa);
void addAtmo(ModelContainer* mod);
void addTitan(ModelContainer* mod, std::mt19937& rng, std::uniform_real_distribution<float> &eps, std::uniform_real_distribution<float> &alfa, int idx);
void addTerranPlanet(ModelContainer* mod, std::mt19937& rng, std::uniform_real_distribution<float>& eps, std::uniform_real_distribution<float>& alfa, int idx);

#endif // MAPGENERATOR_H
