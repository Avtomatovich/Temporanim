#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <vector>
#include "physics/rigidbody.h"
#include "utils/sceneparser.h"

class Projectile
{
public:
    Projectile(const RigidBody& rigidBody);

    RenderShapeData getProjectile();

    void draw();

private:
    std::vector<RenderShapeData> projectiles;

    const RigidBody& m_rigidBody;
};

#endif // PROJECTILE_H
