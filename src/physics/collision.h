#ifndef COLLISION_H
#define COLLISION_H

#include "box.h"
#include "utils/sceneparser.h"

class Collision
{
public:
    Collision(const RenderShapeData& shape);

    const PrimitiveType getType() const;

    const Box& getBox() const;

    void updateBox(const glm::mat4& ctm);

    bool detect(const Collision& that) const;

private:
    PrimitiveType type;
    glm::vec3 center, height;
    float radius;

    // object space min, max
    glm::vec3 min, max;

    // world space AABB
    Box box;

    bool boxBox(const Box& b0, const Box& b1) const;
    bool cubeBox(const Collision& cube, const Box& box) const;
};

#endif // COLLISION_H
