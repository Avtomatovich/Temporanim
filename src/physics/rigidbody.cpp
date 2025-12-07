#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "rigidbody.h"

RigidBody::RigidBody(float m, const glm::mat4& ctm)
    : mass(m), invMass(1.f / m), initialCTM(ctm)
{
    // initial position
    x = ctm[3];

    // store scale to reapply later
    initialScale = {
        glm::length(ctm[0]),
        glm::length(ctm[1]),
        glm::length(ctm[2])
    };

    glm::mat3 rotMat{
        glm::normalize(ctm[0]),
        glm::normalize(ctm[1]),
        glm::normalize(ctm[2])
    };

    // to quaternion
    q = glm::quat_cast(rotMat);
}

glm::mat4 RigidBody::getTransformMatrix() const {
    // Translation * Rotation * Scale
    glm::mat4 T = glm::translate(glm::mat4{1.f}, x);
    glm::mat4 S = glm::scale(glm::mat4{1.f}, initialScale);

    return T * R * S;
}

void RigidBody::computeAuxiliaryVariables() {
    // v(t) = P(t) / M
    v = P * invMass;

    // quaternion to rotation matrix: R(t) = matrix(q(t))
    q = glm::normalize(q);
    R = glm::toMat4(q);

    glm::mat3 R3{R};
    Iinv = R3 * IbodyInv * glm::transpose(R3);

    omega = Iinv * L;
}

void RigidBody::clearForces() {
    force = torque = glm::vec3{0.f};
}

void RigidBody::applyForce(const glm::vec3& f) {
    force += f;
}

void RigidBody::applyForceAtPoint(const glm::vec3& f, const glm::vec3& worldPoint) {
    force += f;

    // torque
    glm::vec3 r = worldPoint - x;
    torque += glm::cross(r, f);
}

void RigidBody::applyImpulse(const glm::vec3& impulse) {
    // momentum directly
    P += impulse;
}

// Baraff equation (5-3)
glm::mat3 RigidBody::computeCubeInertia(float M, const glm::vec3& dim) {
    float w = dim.x, h = dim.y, d = dim.z;
    return glm::mat3{
        M / 12.f * (h*h + d*d), 0.f, 0.f,
        0.f, M / 12.f * (w*w + d*d), 0.f,
        0.f, 0.f, M / 12.f * (w*w + h*h)
    };
}

// Inertia tensor for a sphere
glm::mat3 RigidBody::computeSphereInertia(float M, float r) {
    return glm::scale(glm::mat4{1.f}, glm::vec3{(2.f / 5.f) * M * r*r});
}

// i tensor for a cylinder along Y axis
glm::mat3 RigidBody::computeCylinderInertia(float M, float r, float h) {
    float Ixx = M * (3 * r*r + h*h) / 12.f;
    float Iyy = M * r*r / 2.f;
    return glm::scale(glm::mat4{1.f}, {Ixx, Iyy, Ixx});
}

// i tensor for a cone along Y axis
glm::mat3 RigidBody::computeConeInertia(float M, float r, float h) {
    float Ixx = M * (3 * r*r / 20.f + h*h / 10.f);
    float Iyy = 3 * M * r*r / 10.f;
    return glm::scale(glm::mat4{1.f}, {Ixx, Iyy, Ixx});
}
