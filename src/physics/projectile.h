#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "physics/rigidbody.h"
#include "physics/box.h"
#include "utils/scenedata.h"
#include "physics/collision.h"
#include <string>

enum class FruitType {
    TOMATO,
    CABBAGE,
    CARROT,
    APPLE,
    ONION,
    SWEET_POTATO
};

struct FruitData {
    std::string meshFile;
    float density;
};

class Projectile {
public:
    Projectile(FruitType type,
               const glm::vec3& startPos,
               const glm::vec3& initialVelocity,
               const RenderShapeData& templateShape);

    // Update render data transform from RigidBody (call after physics update)
    void updateRenderData();

    bool shouldRemove() const;

    // Getters
    RigidBody& getRigidBody() { return m_rigidBody; }
    const RigidBody& getRigidBody() const { return m_rigidBody; }
    Collision& getCollision() { return m_collision; }
    const Collision& getCollision() const { return m_collision; }
    const RenderShapeData& getRenderData() const { return m_renderData; }
    const Box& getBox() const { return m_collision.getBox(); }
    FruitType getType() const { return m_type; }

    void markHit() { m_hitTarget = true; }
    bool wasHit() const { return m_hitTarget; }

    static std::string getMeshFileForType(FruitType type);
    static FruitData getFruitData(FruitType type);

private:
    RigidBody m_rigidBody;
    Collision m_collision;
    RenderShapeData m_renderData;
    FruitType m_type;
    bool m_hitTarget = false;

    bool isOffScreen() const;
    bool hasStoppedMoving() const;
};

#endif // PROJECTILE_H
