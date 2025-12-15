#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "physics/collision.h"
#include "utils/scenedata.h"
#include "physics/box.h"

class RigidBody {
public:
    RigidBody() {}

    RigidBody(PrimitiveType shapeType, const glm::mat4& initCtm, const Box& box);

    void reset();

    glm::mat4 getCtm() const;

    void clearForces();

    void applyForce();

    void applyTorque(const glm::vec3& axis);

    void applyImpulse(const glm::vec3& impulse);

    void integrate(float dt);

    void applyReaction(const Contact& contact);

    bool atRest();

private:
    PrimitiveType type;

    // constant vars
    float M = 1.f;                        // mass
    glm::mat3 Ibody{1.f};                 // inertia tensor
    glm::mat3 IbodyInv{1.f};              // inverse of inertia tensor

    // state vars
    glm::vec3 x_t{0.f};                   // position x(t) - center of mass
    glm::quat q_t{1.f, 0.f, 0.f, 0.f};    // orientation q(t) as quaternion
    glm::vec3 P_t{0.f};                   // linear momentum P(t)
    glm::vec3 L_t{0.f};                   // angular momentum L(t)

    // derived (auxiliary) vars
    glm::mat4 R{1.f};                     // rotation matrix (from quaternion)
    glm::vec3 v{0.f};                     // linear velocity v(t) = P(t) / M
    glm::vec3 omega{0.f};                 // angular velocity

    // computed vars
    glm::vec3 force{0.f};                 // F(t)
    glm::vec3 torque{0.f};                // torque

    glm::mat4 ctm{1.f};
    glm::vec3 scale{0.f};

    void computeAuxiliaryVariables();

    glm::mat3 computeCubeInertia(const glm::vec3& dim);
    glm::mat3 computeSphereInertia(float r);
    glm::mat3 computeCylinderInertia(float r, float h);
    glm::mat3 computeConeInertia(float r, float h);

    constexpr static float restitution = 0.6f;
    constexpr static float impulse_mag = 20.f;
    constexpr static float torque_mag = 100.f;
    constexpr static float rho = 0.7f;
    constexpr static glm::vec3 g{0.f, -9.8f, 0.f};
    constexpr static float EPS = 1e-6f;
};

#endif // RIGIDBODY_H
