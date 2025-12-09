#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <vector>
// #include "physics/rigidbody.h"
#include "utils/scenedata.h"
#include "utils/sceneparser.h"

class Projectile
{
public:
    Projectile();

    RenderShapeData getProjectile();

    void draw();

private:
    std::vector<ScenePrimitive> projectiles;

    // RigidBody m_rigidBody;
};

#endif // PROJECTILE_H
