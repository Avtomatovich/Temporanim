#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "utils/scenedata.h"

struct Force {
    float impulse;
    glm::vec3 v;

    static constexpr float g = 9.8f;
};

class RigidBody {
public:
    RigidBody() {}

    RigidBody(float mass, const glm::mat4& initialTransform);

    glm::mat4 getTransformMatrix() const;

private:
    glm::vec3 x{0.f};                   // position x(t) - center of mass
    glm::quat q{1.f, 0.f, 0.f, 0.f};    // orientation q(t) as quaternion
    glm::vec3 P{0.f};                   // linear momentum P(t)
    glm::vec3 L{0.f};                   // angular momentum L(t)
    float mass = 1.f;                   // m
    float invMass = 1.f;                // 1/m

    // derived state
    glm::vec3 v{0.f};                   // linear velocity v(t) = P(t) / M
    glm::vec3 omega{0.f};               // angular velocity

    glm::vec3 force{0.f};               // F(t)
    glm::vec3 torque{0.f};              // torque
    glm::mat3 Ibody{1.f};               // inertia tensor
    glm::mat3 IbodyInv{1.f};            // inverse of inertia tensor
    glm::mat3 Iinv{1.f};                // world space inverse inertia
    glm::mat4 R{1.f};                   // rotation matrix (from quaternion)
    glm::mat4 initialCTM{1.f};
    glm::vec3 initialScale{1.f};

    bool isStatic = false;
    bool enableGravity = true;
    PrimitiveType shapeType;

    void computeAuxiliaryVariables();
    void clearForces();
    void applyForce(const glm::vec3& f);
    void applyForceAtPoint(const glm::vec3& f, const glm::vec3& worldPoint);
    void applyImpulse(const glm::vec3& impulse);

    static glm::mat3 computeCubeInertia(float M, const glm::vec3& dim);
    static glm::mat3 computeSphereInertia(float M, float r);
    static glm::mat3 computeCylinderInertia(float M, float r, float h);
    static glm::mat3 computeConeInertia(float M, float r, float h);
};

#endif // RIGIDBODY_H
