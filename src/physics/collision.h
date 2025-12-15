#ifndef COLLISION_H
#define COLLISION_H

#include "box.h"
#include "utils/sceneparser.h"

struct Contact {
    glm::vec3 p{0.f};
    glm::vec3 n{0.f};
    float overlap;
};

class Collision
{
public:
    Collision() {}

    Collision(const RenderShapeData& shape);

    const Box& getBox() const;

    void updateBox(const glm::mat4& ctm);

    std::optional<Contact> detect(const Collision& that) const;

    void scaleBox(float factor);

private:
    PrimitiveType type;
    glm::vec3 center, height;
    float radius;

    // object space min, max
    glm::vec3 min, max;

    // world space AABB
    Box box;

    std::optional<Contact> cubeBox(const Collision& cube, const Box& box) const;
    std::optional<Contact> boxBox(const Box& b0, const Box& b1) const;
};

#endif // COLLISION_H
