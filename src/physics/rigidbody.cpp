#include "rigidbody.h"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

RigidBody::RigidBody(PrimitiveType shapeType, float m, const glm::mat4& initCtm, const Box& box)
    : type(shapeType), mass(m), ctm(initCtm)
{
    // TODO: compute mass using constant density factor and computed volume
    reset();

    // inertia tensor based on shape type
    switch (type) {
        case PrimitiveType::PRIMITIVE_CUBE:
            Ibody = computeCubeInertia(mass, scale);
            break;
        case PrimitiveType::PRIMITIVE_SPHERE:
            Ibody = computeSphereInertia(mass, 0.5f * scale.x);
            break;
        case PrimitiveType::PRIMITIVE_CYLINDER:
            Ibody = computeCylinderInertia(mass, 0.5f * scale.x, scale.y);
            break;
        case PrimitiveType::PRIMITIVE_CONE:
            Ibody = computeConeInertia(mass, 0.5f * scale.x, scale.y);
            break;
        case PrimitiveType::PRIMITIVE_MESH:
            Ibody = computeCubeInertia(mass, box.max - box.min);
        default:
            Ibody = glm::mat3(1.f);
            break;
    }

    // Precompute inverse
    IbodyInv = glm::inverse(Ibody);

    // Compute initial auxiliary variables
    computeAuxiliaryVariables();
}

void RigidBody::reset() {
    // initial pos
    x_t = ctm[3];

    // store scale
    scale = {
        glm::length(glm::vec3{ctm[0]}),
        glm::length(glm::vec3{ctm[1]}),
        glm::length(glm::vec3{ctm[2]})
    };

    // to quaternion
    q_t = glm::quat_cast(glm::mat3{
                            glm::normalize(glm::vec3{ctm[0]}),
                            glm::normalize(glm::vec3{ctm[1]}),
                            glm::normalize(glm::vec3{ctm[2]})
                        });

    P_t = L_t = glm::vec3{0.f};
    clearForces();
    computeAuxiliaryVariables();
}

glm::mat4 RigidBody::getCtm() const {
    // Translation * Rotation * Scale
    glm::mat4 T = glm::translate(glm::mat4{1.f}, x_t);
    glm::mat4 S = glm::scale(glm::mat4{1.f}, scale);

    return T * R * S;
}

void RigidBody::computeAuxiliaryVariables() {
    // v(t) = P(t) / M
    v = P_t / mass;

    // quaternion to rotation matrix: R(t) = matrix(q(t))
    q_t = glm::normalize(q_t);
    R = glm::toMat4(q_t);

    glm::mat3 R3{R};

    // world space inverse inertia
    glm::mat3 Iinv = R3 * IbodyInv * glm::transpose(R3);

    omega = Iinv * L_t;
}

void RigidBody::integrate(float dt) {
    computeAuxiliaryVariables();

    glm::quat omega_quat{0.f, omega};
    glm::quat q_dot = 0.5f * (omega_quat * q_t);

    // euler integration
    x_t += v * dt;
    q_t += q_dot * dt;
    P_t += force * dt;
    L_t += torque * dt;

    q_t = glm::normalize(q_t);

    // damping
    P_t *= 0.99f;
    L_t *= 0.99f;
}

void RigidBody::clearForces() {
    force = torque = glm::vec3{0.f};
}

void RigidBody::applyForce() {
    force += mass * gravity;
}

void RigidBody::applyForceAtPoint(const glm::vec3& point) {
    applyForce();

    // torque
    torque += glm::cross(point - x_t, mass * gravity);
}

void RigidBody::applyImpulse(const glm::vec3& impulse) {
    // momentum directly
    P_t += impulse;
}

void RigidBody::handleForces() {
    for (int i = 0; i < 3; ++i) {
        if (v[i] < 0) {
            v[i] = -v[i] * restitution;

            // dampen velocity on impact
            v[i] *= 0.9f;

            // new linear momentum to match new velocity
            P_t[i] = mass * v[i];

            // reduce spin on impact
            L_t[i] *= 0.95f;
        }
    }
}

// Baraff equation (5-3)
glm::mat3 RigidBody::computeCubeInertia(float M, const glm::vec3& dim) {
    float w = dim.x, h = dim.y, d = dim.z;
    return glm::scale(glm::mat4{1.f}, {
                                          M / 12.f * (h*h + d*d),
                                          M / 12.f * (w*w + d*d),
                                          M / 12.f * (w*w + h*h)
                                      });
}

// Inertia tensor for a sphere
glm::mat3 RigidBody::computeSphereInertia(float M, float r) {
    float I = (2.f / 5.f) * M * r*r;

    return {
        I,   0.f, 0.f,
        0.f, I,   0.f,
        0.f, 0.f, I
    };
}

// Inertia tensor for a cylinder along Y axis
glm::mat3 RigidBody::computeCylinderInertia(float M, float r, float h) {
    float Ixx = M * (3 * r*r + h*h) / 12.f;
    float Iyy = M * r*r / 2.f;

    return {
        Ixx, 0.f, 0.f,
        0.f, Iyy, 0.f,
        0.f, 0.f, Ixx
    };
}

// Inertia tensor for a cone along Y axis
glm::mat3 RigidBody::computeConeInertia(float M, float r, float h) {
    float Ixx = M * (3 * r*r / 20.f + h*h / 10.f);
    float Iyy = 3 * M * r*r / 10.f;

    return {
        Ixx, 0.f, 0.f,
        0.f, Iyy, 0.f,
        0.f, 0.f, Ixx
    };
}
