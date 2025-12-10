#ifndef COLLISION_H
#define COLLISION_H

#include "box.h"
#include "utils/sceneparser.h"

class Collision
{
public:
    Collision(const RenderShapeData& shape);

    const Box& getBox() const;

    void updateBox(const glm::mat4& ctm);

    bool detect(const Collision& collider);

private:
    PrimitiveType type;
    glm::vec3 center, height;
    float radius;

    // world space AABB
    Box box;

    // object space min, max
    glm::vec3 min, max;

    bool boxBox(const Box& b0, const Box& b1);
    bool cubeBox(const Collision& cube, const Box& box);
};

#endif // COLLISION_H
