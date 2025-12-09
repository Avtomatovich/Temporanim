#ifndef COLLISION_H
#define COLLISION_H

#include "box.h"
#include "utils/sceneparser.h"

class Collision
{
public:
    Collision(const RenderShapeData& shape);

    bool detect(const Collision& collider);

private:
    PrimitiveType type;
    glm::vec3 center, height;
    float radius;
    Box box;

    bool sphereBox(const Collision& sphere, const Collision& box);
};

#endif // COLLISION_H
