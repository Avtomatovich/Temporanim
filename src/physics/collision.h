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
    Box box;

    bool boxBox(const Box& b0, const Box& b1);
};

#endif // COLLISION_H
