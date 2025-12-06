#include "rigidbody.h"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

RigidBody::RigidBody() {
    x = glm::vec3(0);
    q = glm::quat(1, 0, 0, 0);
    P = glm::vec3(0);
    L = glm::vec3(0);
    mass = 1.0f;
    invMass = 1.0f;

    // derived state
    v = glm::vec3(0);
    omega = glm::vec3(0);

    force = glm::vec3(0);
    torque = glm::vec3(0);
    isStatic = false;
    enableGravity = true;
    Ibody = glm::mat3(1.0f);
    IbodyInv = glm::mat3(1.0f);
    Iinv = glm::mat3(1.0f);
    R = glm::mat4(1.0f);
    initialCTM = glm::mat4(1.0f);
    initialScale = glm::vec3(1.0f);
}

RigidBody::RigidBody(float m, const glm::mat4& ctm)
    : RigidBody()
{
    mass = m;
    invMass = 1.0f / m;
    initialCTM = ctm;

    // initial position
    x = glm::vec3(ctm[3]);

    // store scale to reapply later
    initialScale.x = glm::length(glm::vec3(ctm[0]));
    initialScale.y = glm::length(glm::vec3(ctm[1]));
    initialScale.z = glm::length(glm::vec3(ctm[2]));

    glm::mat3 rotMat;
    rotMat[0] = glm::vec3(ctm[0]) / initialScale.x;
    rotMat[1] = glm::vec3(ctm[1]) / initialScale.y;
    rotMat[2] = glm::vec3(ctm[2]) / initialScale.z;

    // to quaternion
    q = glm::quat_cast(rotMat);
}

void RigidBody::computeAuxiliaryVariables() {
    // v(t) = P(t)/M
    v = P * invMass;

    // quaternion to rotation matrix: R(t) = matrix(q(t))
    q = glm::normalize(q);
    R = glm::mat4_cast(q);

    glm::mat3 R3 = glm::mat3(R);
    Iinv = R3 * IbodyInv * glm::transpose(R3);

    omega = Iinv * L;
}

void RigidBody::clearForces() {
    force = glm::vec3(0);
    torque = glm::vec3(0);
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

glm::mat4 RigidBody::getTransformMatrix() const {
    // Translation * Rotation * Scale
    glm::mat4 T = glm::translate(glm::mat4(1.0f), x);
    glm::mat4 S = glm::scale(glm::mat4(1.0f), initialScale);

    return T * R * S;
}

// Baraff equation (5-3)
glm::mat3 RigidBody::computeBoxInertia(float M, const glm::vec3& dim) {
    float w = dim.x, h = dim.y, d = dim.z;
    return glm::mat3(
        M/12.0f * (h*h + d*d), 0, 0,
        0, M/12.0f * (w*w + d*d), 0,
        0, 0, M/12.0f * (w*w + h*h)
        );
}

// Inertia tensor for a sphere
glm::mat3 RigidBody::computeSphereInertia(float M, float r) {
    float I = (2.0f/5.0f) * M * r * r;
    return glm::mat3(I, 0, 0,
                     0, I, 0,
                     0, 0, I);
}

// i tensor for a cylinder along Y axis
glm::mat3 RigidBody::computeCylinderInertia(float M, float r, float h) {
    float Ixx = M * (3*r*r + h*h) / 12.0f;
    float Iyy = M * r * r / 2.0f;
    return glm::mat3(Ixx, 0, 0,
                     0, Iyy, 0,
                     0, 0, Ixx);
}

// i tensor for a cone along Y axis
glm::mat3 RigidBody::computeConeInertia(float M, float r, float h) {
    float Ixx = M * (3*r*r/20.0f + h*h/10.0f);
    float Iyy = 3*M*r*r / 10.0f;
    return glm::mat3(Ixx, 0, 0,
                     0, Iyy, 0,
                     0, 0, Ixx);
}
