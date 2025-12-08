#include "rigidbody.h"
#include "utils/transform.h"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

RigidBody::RigidBody(PrimitiveType shapeType, float m, const glm::mat4& initialTransform)
    : type(shapeType), mass(m), ctm(initialTransform)
{
    reset();

    // inertia tensor based on shape type
    switch (type) {
        case PrimitiveType::PRIMITIVE_CUBE:
            Ibody = Transform::computeCubeInertia(mass, scale);
            break;
        case PrimitiveType::PRIMITIVE_SPHERE:
            Ibody = Transform::computeSphereInertia(mass, 0.5f * scale.x);
            break;
        case PrimitiveType::PRIMITIVE_CYLINDER:
            Ibody = Transform::computeCylinderInertia(mass, 0.5f * scale.x, scale.y);
            break;
        case PrimitiveType::PRIMITIVE_CONE:
            Ibody = Transform::computeConeInertia(mass, 0.5f * scale.x, scale.y);
            break;
        default:
            //sphere
            Ibody = glm::mat3(1.0f);
            break;
    }

    // Precompute inverse
    IbodyInv = glm::inverse(Ibody);

    // Compute initial auxiliary variables
    computeAuxiliaryVariables();
}

void RigidBody::reset() {
    // initial pos
    x = ctm[3];

    // store scale
    scale = glm::vec3{
        glm::length(glm::vec3{ctm[0]}),
        glm::length(glm::vec3{ctm[1]}),
        glm::length(glm::vec3{ctm[2]})
    };

    // to quaternion
    q = glm::quat_cast(glm::mat3{
                            glm::normalize(glm::vec3{ctm[0]}),
                            glm::normalize(glm::vec3{ctm[1]}),
                            glm::normalize(glm::vec3{ctm[2]})
                        });

    P = L = glm::vec3{0.f};
    clearForces();
    computeAuxiliaryVariables();
}

glm::mat4 RigidBody::getTransformMatrix() const {
    // Translation * Rotation * Scale
    glm::mat4 T = glm::translate(glm::mat4{1.f}, x);
    glm::mat4 S = glm::scale(glm::mat4{1.f}, scale);

    return T * R * S;
}

void RigidBody::computeAuxiliaryVariables() {
    // v(t) = P(t) / M
    v = P / mass;

    // quaternion to rotation matrix: R(t) = matrix(q(t))
    q = glm::normalize(q);
    R = glm::toMat4(q);

    glm::mat3 R3{R};

    // world space inverse inertia
    glm::mat3 Iinv = R3 * IbodyInv * glm::transpose(R3);

    omega = Iinv * L;
}

void RigidBody::integrate(float dt) {
    computeAuxiliaryVariables();

    glm::quat omega_quat{0.f, omega};
    glm::quat q_dot = 0.5f * (omega_quat * q);

    // euler integration
    x += v * dt;
    P += force * dt;
    L += torque * dt;
    q += q_dot * dt;

    //damping
    P *= 0.99f;
    L *= 0.99f;
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
    torque += glm::cross(point - x, mass * gravity);
}

void RigidBody::applyImpulse(const glm::vec3& impulse) {
    // momentum directly
    P += impulse;
}

void RigidBody::bounceSphere(float groundY) {
    if (type != PrimitiveType::PRIMITIVE_SPHERE) return;

    // sphere radius
    float radius = scale.x * 0.5f;

    // if sphere hit ground
    float bottomY = x.y - radius;

    if (bottomY <= groundY) {
        x.y = groundY + radius;

        // bounciness
        float restitution = 0.7f;

        if (v.y < 0) {
            v.y = -v.y * restitution;

            // new linear momentum to match new velocity
            P = mass * v;

            // reduce horizontal velocity on impact
            v.x *= 0.9f;
            v.z *= 0.9f;
            P = mass * v;

            // reduce spin on impact
            L *= 0.95f;
        }
    }
}
