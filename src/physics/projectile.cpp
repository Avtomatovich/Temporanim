#include "projectile.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cfloat>
#include <random>
#include "physics/collision.h"

Projectile::Projectile(FruitType type,
                       const glm::vec3& startPos,
                       const glm::vec3& initialVelocity,
                       const RenderShapeData& templateShape)
    : m_type(type), m_renderData(templateShape), m_collision(templateShape)
{
    FruitData data = getFruitData(type);

    // object-space box from template's vertex data
    Box objSpaceBox = computeBox(templateShape);
    m_objSpaceMin = objSpaceBox.min;  // Store object-space bounds
    m_objSpaceMax = objSpaceBox.max;

    // mass from box volume and density
    float mass = computeMass(objSpaceBox, data.density);

    // transform at spawn position
    glm::mat4 ctm = glm::translate(glm::mat4(1.0f), startPos);
    ctm = glm::scale(ctm, glm::vec3(0.3f));

    // rigid body with object-space box
    m_rigidBody = RigidBody(PrimitiveType::PRIMITIVE_MESH, mass, ctm, objSpaceBox);
    m_rigidBody.setVelocity(initialVelocity);

    m_renderData.ctm = ctm;
    updateBox(ctm);  //  object-space to world-space
    m_collision.updateBox(ctm);  // uopdate collision box
}

void Projectile::update(float dt) {
    // grabvity and integrate physics
    m_rigidBody.clearForces();
    m_rigidBody.applyForce();
    m_rigidBody.integrate(dt);

    //  current transform from rigid body
    glm::mat4 ctm = m_rigidBody.getTransformMatrix();
    
    // udate world-space box using rigid body transform
    updateBox(ctm);
    m_collision.updateBox(ctm);  // Keep collision box in sync

    // update render data transform
    m_renderData.ctm = ctm;
}

bool Projectile::shouldRemove() const {
    if (m_hitTarget) return true;
    if (isOffScreen()) return true;
    if (hasStoppedMoving()) return true;
    return false;
}

bool Projectile::isOffScreen() const {
    // Remove if fallen way below ground
    return m_rigidBody.getPosition().y < -50.0f;
}

bool Projectile::hasStoppedMoving() const {
    // remove if nearly stopped
    glm::vec3 vel = m_rigidBody.getVelocity();
    return glm::length(vel) < 0.01f;
}

Box Projectile::computeBox(const RenderShapeData& shape) {
    // Compute AABB from vertex data in OBJECT SPACE (like collision does)
    auto x_cmp = [](const Vertex& a, const Vertex& b) {
        return a.pos.x < b.pos.x;  // Use .pos, not .position
    };
    auto y_cmp = [](const Vertex& a, const Vertex& b) {
        return a.pos.y < b.pos.y;
    };
    auto z_cmp = [](const Vertex& a, const Vertex& b) {
        return a.pos.z < b.pos.z;
    };

    const auto& vertices = shape.vertexData;

    float x_min = std::min_element(vertices.begin(), vertices.end(), x_cmp)->pos.x;
    float x_max = std::max_element(vertices.begin(), vertices.end(), x_cmp)->pos.x;
    float y_min = std::min_element(vertices.begin(), vertices.end(), y_cmp)->pos.y;
    float y_max = std::max_element(vertices.begin(), vertices.end(), y_cmp)->pos.y;
    float z_min = std::min_element(vertices.begin(), vertices.end(), z_cmp)->pos.z;
    float z_max = std::max_element(vertices.begin(), vertices.end(), z_cmp)->pos.z;

    // object-space bounds (don't transform yet)
    return Box{
        {x_min, y_min, z_min},
        {x_max, y_max, z_max}
    };
}

float Projectile::computeMass(const Box& box, float density) {
    // mass = density x volume
    glm::vec3 dims = box.max - box.min;
    float volume = dims.x * dims.y * dims.z;
    return density * volume;
}

std::string Projectile::getMeshFileForType(FruitType type) {
    return getFruitData(type).meshFile;
}

FruitData Projectile::getFruitData(FruitType type) {
    switch (type) {
    case FruitType::TOMATO:
        return {"models/potato/scene.gltf", 950.0f};
    case FruitType::CABBAGE:
        return {"models/cabbage/scene.gltf", 600.0f};
    case FruitType::CARROT:
        return {"models/carrot/scene.gltf", 1000.0f};
    case FruitType::APPLE:
        return {"models/apple/scene.gltf", 850.0f};
    case FruitType::ONION:
        return {"models/onion/scene.gltf", 900.0f};
    case FruitType::SWEET_POTATO:
        return {"models/potato/scene.gltf", 950.0f};
    }
}

void Projectile::updateBox(const glm::mat4& ctm) {
    //  center and scale from transform
    glm::vec3 center = ctm[3];
    glm::vec3 scale = {
        glm::length(glm::vec3{ctm[0]}),
        glm::length(glm::vec3{ctm[1]}),
        glm::length(glm::vec3{ctm[2]})
    };

    // ignore rotation to maintain axis alignment
    glm::mat4 T = glm::translate(glm::mat4{1.f}, center);
    glm::mat4 S = glm::scale(glm::mat4{1.f}, scale);
    
    m_box = Box{
        T * S * glm::vec4{m_objSpaceMin, 1.f},
        T * S * glm::vec4{m_objSpaceMax, 1.f}
    };
}


ProjectileManager::ProjectileManager() : m_rng(std::random_device{}()) {
}

void ProjectileManager::initialize(const std::vector<RenderShapeData>& fruitTemplates) {
    m_fruitTemplates = fruitTemplates;

    // for (auto& templateShape : m_fruitTemplates) {
    //     if (templateShape.vertexData.empty()) {
    //         RenderData tempRenderData;
    //         ModelParser::meshParse(tempRenderData, &templateShape.primitive, templateShape.ctm);
    //         // Replace templateShape with loaded shape from tempRenderData.shapes[0]
    //         // But this modifies the template - might need to store loaded versions separately
    //     }
    // }
    
    // available fruit types
    m_availableFruits.clear();
    for (const auto& templateShape : fruitTemplates) {
        const std::string& meshFile = templateShape.primitive.meshfile;
        
        // mesh files to fruit types based on main_scene.json
        if (meshFile.find("tomato") != std::string::npos) {
            m_availableFruits.push_back(FruitType::TOMATO);
        } else if (meshFile.find("cabbage") != std::string::npos) {
            m_availableFruits.push_back(FruitType::CABBAGE);
        } else if (meshFile.find("carrot") != std::string::npos) {
            m_availableFruits.push_back(FruitType::CARROT);
        } else if (meshFile.find("apple") != std::string::npos) {
            m_availableFruits.push_back(FruitType::APPLE);
        } else if (meshFile.find("onion") != std::string::npos) {
            m_availableFruits.push_back(FruitType::ONION);
        } else if (meshFile.find("potato") != std::string::npos) {
            m_availableFruits.push_back(FruitType::SWEET_POTATO);
        }
    }
}

void ProjectileManager::update(float dt, const glm::vec3& cameraPos, const glm::vec3& cameraLook) {
    //  active projectiles
    for (auto it = m_activeProjectiles.begin(); it != m_activeProjectiles.end();) {
        (*it)->update(dt);
        
        // remove if projectile says it should be removed
        if ((*it)->shouldRemove()) {
            it = m_activeProjectiles.erase(it);
        } else {
            ++it;
        }
    }
    
    // update projectile position to follow the camera
    if (m_isCharging && m_chargingProjectile) {
        glm::vec3 spawnPos = computeSpawnPosition(cameraPos, cameraLook);
        glm::mat4 ctm = glm::translate(glm::mat4(1.0f), spawnPos);
        ctm = glm::scale(ctm, glm::vec3(0.3f));
        // we recreate projectiles transform each frame during charging
        // but maybe we can update charging projectile's transform?
    }
}

void ProjectileManager::startCharging(const glm::vec3& cameraPos, const glm::vec3& cameraLook) {
    if (m_isCharging) return;
    
    m_isCharging = true;
    m_chargeStartPos = cameraPos;
    m_chargeLookDir = glm::normalize(cameraLook);
    m_chargeStartTime = 0.f;  // Will be set when thrown
    
    // spawn projectile at camera position (not thrown yet)
    glm::vec3 spawnPos = computeSpawnPosition(cameraPos, cameraLook);
    FruitType selectedFruit = selectRandomFruit();
    RenderShapeData templateShape = getTemplateForFruit(selectedFruit);
    
    // projectile with zero velocity (it's just being held)
    m_chargingProjectile = std::make_unique<Projectile>(
        selectedFruit,
        spawnPos,
        glm::vec3(0.f),
        templateShape
    );
}

void ProjectileManager::throwProjectile(const glm::vec3& cameraPos, const glm::vec3& cameraLook, float chargeTime) {
    if (!m_isCharging || !m_chargingProjectile) return;
    
    // throw velocity based on charge time and camera direction
    glm::vec3 throwVelocity = computeThrowVelocity(glm::normalize(cameraLook), chargeTime);
    
    // fruit type from charging projectile
    FruitType fruitType = m_chargingProjectile->getType();
    RenderShapeData templateShape = getTemplateForFruit(fruitType);
    
    // the actual projectile with velocity
    glm::vec3 spawnPos = computeSpawnPosition(cameraPos, cameraLook);
    
    if (m_activeProjectiles.size() >= MAX_PROJECTILES) {
        cleanupOldProjectiles();
    }
        auto newProjectile = std::make_unique<Projectile>(
        fruitType,
        spawnPos,
        throwVelocity,
        templateShape
    );
    
    m_activeProjectiles.push_back(std::move(newProjectile));
    m_chargingProjectile.reset();
    m_isCharging = false;
}

void ProjectileManager::cleanupOldProjectiles() {
    // remove the oldest projectile (first in vector)
    if (!m_activeProjectiles.empty()) {
        m_activeProjectiles.erase(m_activeProjectiles.begin());
    }
}

void ProjectileManager::spawnProjectile(const glm::vec3& pos, const glm::vec3& velocity) {
    // cleanup old projectiles if at max capacity
    if (m_activeProjectiles.size() >= MAX_PROJECTILES) {
        cleanupOldProjectiles();
    }
    
    FruitType selectedFruit = selectRandomFruit();
    RenderShapeData templateShape = getTemplateForFruit(selectedFruit);
    
    auto projectile = std::make_unique<Projectile>(
        selectedFruit,
        pos,
        velocity,
        templateShape
    );
    
    m_activeProjectiles.push_back(std::move(projectile));
}

FruitType ProjectileManager::selectRandomFruit() {
    if (m_availableFruits.empty()) {
        return FruitType::APPLE;
    }
    
    std::uniform_int_distribution<size_t> dist(0, m_availableFruits.size() - 1);
    return m_availableFruits[dist(m_rng)];
}

RenderShapeData ProjectileManager::getTemplateForFruit(FruitType type) const {
    // Find template that matches the fruit type
    for (const auto& templateShape : m_fruitTemplates) {
        const std::string& meshFile = templateShape.primitive.meshfile;
        std::string targetFile = Projectile::getMeshFileForType(type);
        
        if (meshFile.find(targetFile.substr(0, targetFile.find('.'))) != std::string::npos) {
            return templateShape;
        }
    }
    
    // Fallback to first template if not found
    return m_fruitTemplates.empty() ? RenderShapeData{} : m_fruitTemplates[0];
}

glm::vec3 ProjectileManager::computeSpawnPosition(const glm::vec3& cameraPos, const glm::vec3& cameraLook) const {
    // spawn a little below and in front of camera
    glm::vec3 forward = glm::normalize(cameraLook);
    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
    glm::vec3 up = glm::cross(right, forward);
    
    // camera pos + small forward offset + small down offset
    return cameraPos + forward * 0.5f + up * (-0.3f);
}

glm::vec3 ProjectileManager::computeThrowVelocity(const glm::vec3& cameraLook, float chargeTime) const {
    float baseSpeed = 8.0f;
    
    // charge multiplier (capped)
    float chargeMultiplier = std::min(1.0f + chargeTime * 2.0f, 3.0f);
    
    // the throw direction which is just the camera look direction with slight upward arc
    glm::vec3 forward = glm::normalize(cameraLook);
    glm::vec3 up = glm::vec3(0, 1, 0);
    
    // upward component for arc
    glm::vec3 throwDir = glm::normalize(forward + up * 0.3f);
    
    return throwDir * (baseSpeed * chargeMultiplier);
}
