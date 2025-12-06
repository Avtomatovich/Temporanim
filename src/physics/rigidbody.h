#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "utils/sceneparser.h"

class RigidBody {
public:
    glm::vec3 x;      // position x(t) - center of mass
    glm::quat q;      // orientation q(t) as quaternion
    glm::vec3 P;      // linear momentum P(t)
    glm::vec3 L;      // angular momentum L(t)
    float mass;       // m
    float invMass;    // 1/m
    glm::mat3 Ibody;     // inertia tensor
    glm::mat3 IbodyInv;  // inverse of Ibody
    glm::vec3 v;      // linear velocity v(t) = P(t)/M
    glm::vec3 omega;  // angular velocity
    glm::mat3 Iinv;   // world space inverse inertia
    glm::mat4 R;      // rotation matrix (from quaternion)
    glm::vec3 force;  // F(t)
    glm::vec3 torque; // torque
    bool isStatic;
    bool enableGravity;
    PrimitiveType shapeType;
    glm::mat4 initialCTM;
    glm::vec3 initialScale;

    RigidBody();
    RigidBody(float mass, const glm::mat4& initialTransform);
    void computeAuxiliaryVariables();
    void clearForces();
    void applyForce(const glm::vec3& f);
    void applyForceAtPoint(const glm::vec3& f, const glm::vec3& worldPoint);
    void applyImpulse(const glm::vec3& impulse);
    glm::mat4 getTransformMatrix() const;
    static glm::mat3 computeBoxInertia(float mass, const glm::vec3& dimensions);
    static glm::mat3 computeSphereInertia(float mass, float radius);
    static glm::mat3 computeCylinderInertia(float mass, float radius, float height);
    static glm::mat3 computeConeInertia(float mass, float radius, float height);
};
