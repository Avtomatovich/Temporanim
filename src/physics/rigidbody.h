#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "utils/scenedata.h"
#include "physics/box.h"

class RigidBody {
public:
    RigidBody(PrimitiveType shapeType, float m, const glm::mat4& initCtm);

    RigidBody(PrimitiveType shapeType, float m, const glm::mat4& initCtm, const Box& box);

    void reset();

    glm::mat4 getTransformMatrix() const;

    void clearForces();

    void applyForce();

    void integrate(float dt);

    void bounceSphere(float groundY);

private:
    glm::vec3 x_t{0.f};                   // position x(t) - center of mass
    glm::quat q_t{1.f, 0.f, 0.f, 0.f};    // orientation q(t) as quaternion
    glm::vec3 P_t{0.f};                   // linear momentum P(t)
    glm::vec3 L_t{0.f};                   // angular momentum L(t)
    float mass = 1.f;                     // m

    // derived state
    glm::vec3 v{0.f};                   // linear velocity v(t) = P(t) / M
    glm::vec3 omega{0.f};               // angular velocity

    glm::vec3 force{0.f};               // F(t)
    glm::vec3 torque{0.f};              // torque
    glm::mat3 Ibody{1.f};               // inertia tensor
    glm::mat3 IbodyInv{1.f};            // inverse of inertia tensor
    glm::mat4 R{1.f};                   // rotation matrix (from quaternion)
    glm::mat4 ctm{1.f};
    glm::vec3 scale{1.f};

    float restitution = 0.7f;           // bounciness factor

    PrimitiveType type;

    constexpr static glm::vec3 gravity{0.f, -9.8f, 0.f};

    void computeAuxiliaryVariables();

    void applyForceAtPoint(const glm::vec3& point);
    void applyImpulse(const glm::vec3& impulse);

    glm::mat3 computeCubeInertia(float M, const glm::vec3& dim);
    glm::mat3 computeSphereInertia(float M, float r);
    glm::mat3 computeCylinderInertia(float M, float r, float h);
    glm::mat3 computeConeInertia(float M, float r, float h);
};

#endif // RIGIDBODY_H
