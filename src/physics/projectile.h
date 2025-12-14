#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <random>
#include <vector>
#include "utils/sceneparser.h"

class Projectile
{
public:
    Projectile() {}

    Projectile(const RenderData& metaData);

    RenderShapeData spawn();

private:
    std::vector<RenderShapeData> m_shapes;

    std::default_random_engine gen;
    std::uniform_int_distribution<int> idx;
};

#endif // PROJECTILE_H
