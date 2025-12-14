#include "camera.h"
#include "utils/transform.h"

Camera::Camera(glm::vec3 pos, glm::vec3 look, glm::vec3 up,
               float aspectRatio,
               float heightAngle,
               float near, float far) :
    m_heightAngle(heightAngle),
    m_near(near),
    m_far(far)
{
    // init view matrix
    setView(pos, look, up);

    // init aspect ratio, height angle, projection matrix
    setAspectRatio(aspectRatio);
}

void Camera::setView(const glm::vec3& pos,
                     const glm::vec3& look,
                     const glm::vec3& up)
{
    m_pos = pos;

    glm::vec3 w = m_look = glm::normalize(-look);
    glm::vec3 v = glm::normalize(up - glm::dot(up, w) * w);
    glm::vec3 u = glm::cross(v, w);

    glm::mat4 rotation {
        glm::vec4(u.x, v.x, w.x, 0.f),
        glm::vec4(u.y, v.y, w.y, 0.f),
        glm::vec4(u.z, v.z, w.z, 0.f),
        glm::vec4(0.f, 0.f, 0.f, 1.f)
    };

    m_view = rotation * Transform::translate(-pos);
}

void Camera::setAspectRatio(float aspectRatio) {
    m_aspectRatio = aspectRatio;
    m_widthAngle = 2.f * atan(m_aspectRatio * tan(m_heightAngle / 2.f));
    perspective(m_near, m_far);
}

const glm::mat4& Camera::getView() const {
    return m_view;
}

const glm::mat4& Camera::getProj() const {
    return m_proj;
}

const glm::vec3& Camera::getPos() const {
    return m_pos;
}

const glm::vec3& Camera::getLook() const {
    return m_look;
}

void Camera::perspective(float near, float far) {
    m_near = near, m_far = far;

    float c = -near / far;

    glm::mat4 scalingMat {
        glm::vec4(1.f / (far * tan(m_widthAngle / 2.f)), 0.f, 0.f, 0.f),
        glm::vec4(0.f, 1.f / (far * tan(m_heightAngle / 2.f)), 0.f, 0.f),
        glm::vec4(0.f, 0.f, 1.f / far, 0.f),
        glm::vec4(0.f, 0.f, 0.f, 1.f)
    };

    glm::mat4 unhingingMat {
        glm::vec4(1.f, 0.f, 0.f, 0.f),
        glm::vec4(0.f, 1.f, 0.f, 0.f),
        glm::vec4(0.f, 0.f, 1.f / (1.f + c), -1.f),
        glm::vec4(0.f, 0.f, -c / (1.f + c), 0.f)
    };

    glm::mat4 remappingMat {
        glm::vec4(1.f, 0.f, 0.f, 0.f),
        glm::vec4(0.f, 1.f, 0.f, 0.f),
        glm::vec4(0.f, 0.f, -2.f, 0.f),
        glm::vec4(0.f, 0.f, -1.f, 1.f)
    };

    m_proj = remappingMat * unhingingMat * scalingMat;
}
