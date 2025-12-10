#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "physics/rigidbody.h"
#include "physics/box.h"
#include "utils/scenedata.h"
#include <string>
#include <vector>
#include <queue>
#include <random>
#include <memory>
#include "physics/collision.h"

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

// indivudal projectile instance
class Projectile {
public:
    Projectile(FruitType type,
               const glm::vec3& startPos,
               const glm::vec3& initialVelocity,
               const RenderShapeData& templateShape);

    void update(float dt);
    bool shouldRemove() const;

    // Getters
    glm::mat4 getTransformMatrix() const { return m_rigidBody.getTransformMatrix(); }
    glm::vec3 getPosition() const { return m_rigidBody.getPosition(); }
    FruitType getType() const { return m_type; }
    const Box& getBox() const { return m_box; }
    const RenderShapeData& getRenderData() const { return m_renderData; }

    // collision detection
    void markHit() { m_hitTarget = true; }
    bool wasHit() const { return m_hitTarget; }

    // helper for scene?
    static std::string getMeshFileForType(FruitType type);

private:
    RigidBody m_rigidBody;
    Box m_box;
    Collision m_collision;
    RenderShapeData m_renderData;
    FruitType m_type;
    bool m_hitTarget = false;
    
    // object-space bounds
    glm::vec3 m_objSpaceMin;
    glm::vec3 m_objSpaceMax;
    
    bool isOffScreen() const;
    bool hasStoppedMoving() const;
    static FruitData getFruitData(FruitType type);
    static Box computeBox(const RenderShapeData& shape);
    static float computeMass(const Box& box, float density);
    void updateBox(const glm::mat4& ctm);
};

class ProjectileManager {
public:
    ProjectileManager();
    
    void initialize(const std::vector<RenderShapeData>& fruitTemplates);
    
    // update all active projectiles
    void update(float dt, const glm::vec3& cameraPos, const glm::vec3& cameraLook);
    
    void startCharging(const glm::vec3& cameraPos, const glm::vec3& cameraLook);
    void throwProjectile(const glm::vec3& cameraPos, const glm::vec3& cameraLook, float chargeTime);
    
    const std::vector<std::unique_ptr<Projectile>>& getActiveProjectiles() const { return m_activeProjectiles; }
    
    // Get count for scene to track
    size_t getActiveCount() const { return m_activeProjectiles.size(); }
    
    // if currently charging a throw
    bool isCharging() const { return m_isCharging; }
    
    // Get the currently charging projectile (for preview rendering)
    const Projectile* getChargingProjectile() const { return m_chargingProjectile.get(); }

private:
    static constexpr size_t MAX_PROJECTILES = 5;
    std::vector<std::unique_ptr<Projectile>> m_activeProjectiles;
    
    std::vector<RenderShapeData> m_fruitTemplates;
    std::vector<FruitType> m_availableFruits;
    
    bool m_isCharging = false;
    std::unique_ptr<Projectile> m_chargingProjectile;
    glm::vec3 m_chargeStartPos;
    glm::vec3 m_chargeLookDir;
    float m_chargeStartTime = 0.f;
    
    std::mt19937 m_rng;
    
    void cleanupOldProjectiles();
    void spawnProjectile(const glm::vec3& pos, const glm::vec3& velocity);
    FruitType selectRandomFruit();
    RenderShapeData getTemplateForFruit(FruitType type) const;
    glm::vec3 computeSpawnPosition(const glm::vec3& cameraPos, const glm::vec3& cameraLook) const;
    glm::vec3 computeThrowVelocity(const glm::vec3& cameraLook, float chargeTime) const;
};

#endif // PROJECTILE_H
