#include "projectile.h"
#include <glm/gtc/matrix_transform.hpp>

Projectile::Projectile(FruitType type,
                       const glm::vec3& startPos,
                       const glm::vec3& initialVelocity,
                       const RenderShapeData& templateShape)
    : m_type(type), m_renderData(templateShape), m_collision(templateShape)
{
    FruitData data = getFruitData(type);

    // Compute object-space box from template's vertex data (like Collision does)
    // We can't use m_collision.getBox() because it's already transformed to world space
    Box objSpaceBox;
    if (!templateShape.vertexData.empty()) {
        auto x_cmp = [](const Vertex& a, const Vertex& b) { return a.pos.x < b.pos.x; };
        auto y_cmp = [](const Vertex& a, const Vertex& b) { return a.pos.y < b.pos.y; };
        auto z_cmp = [](const Vertex& a, const Vertex& b) { return a.pos.z < b.pos.z; };
        
        const auto& vertices = templateShape.vertexData;
        objSpaceBox.min = {
            std::min_element(vertices.begin(), vertices.end(), x_cmp)->pos.x,
            std::min_element(vertices.begin(), vertices.end(), y_cmp)->pos.y,
            std::min_element(vertices.begin(), vertices.end(), z_cmp)->pos.z
        };
        objSpaceBox.max = {
            std::max_element(vertices.begin(), vertices.end(), x_cmp)->pos.x,
            std::max_element(vertices.begin(), vertices.end(), y_cmp)->pos.y,
            std::max_element(vertices.begin(), vertices.end(), z_cmp)->pos.z
        };
    }

    // Compute mass from box volume and density
    glm::vec3 dims = objSpaceBox.max - objSpaceBox.min;
    float volume = dims.x * dims.y * dims.z;
    float mass = data.density * volume;

    // Transform at spawn position with scale
    glm::mat4 ctm = glm::translate(glm::mat4(1.0f), startPos);
    ctm = glm::scale(ctm, glm::vec3(0.3f));

    // Create rigid body with object-space box
    m_rigidBody = RigidBody(PrimitiveType::PRIMITIVE_MESH, mass, ctm, objSpaceBox);
    m_rigidBody.setVelocity(initialVelocity);

    // Update render data and collision
    m_renderData.ctm = ctm;
    m_collision.updateBox(ctm);
}

void Projectile::updateRenderData() {
    // Update render data transform from RigidBody
    glm::mat4 ctm = m_rigidBody.getCtm();
    m_renderData.ctm = ctm;
    m_collision.updateBox(ctm);
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
    // Remove if nearly stopped
    glm::vec3 vel = m_rigidBody.getVelocity();
    return glm::length(vel) < 0.01f;
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
    // Default fallback
    return {"models/potato/scene.gltf", 950.0f};
}
