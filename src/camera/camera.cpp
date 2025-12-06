#include "camera.h"
#include "settings.h"
#include <glm/gtc/matrix_inverse.hpp>
#include <cmath>

Camera::Camera(const SceneCameraData &data, int width, int height)
{
    m_pos = data.pos;
    m_look = data.look;
    m_up = data.up;

    m_heightAngle = data.heightAngle;
    m_aspect = float(width) / float(height);

    updateBasis();
}

void Camera::updateBasis() {
    glm::vec3 look3 = glm::vec3(m_look);
    glm::vec3 up3   = glm::vec3(m_up);

    m_w = glm::normalize(-look3);                 // backwards
    m_u = glm::normalize(glm::cross(up3, m_w));   // right
    m_v = glm::cross(m_w, m_u);                   // up
}


glm::mat4 Camera::getViewMatrix() const {

    glm::mat4 R(1.f);
    R[0] = glm::vec4(m_u, 0);
    R[1] = glm::vec4(m_v, 0);
    R[2] = glm::vec4(m_w, 0);

    glm::mat4 T = glm::mat4(1.f);
    T[3][0] = -m_pos.x;
    T[3][1] = -m_pos.y;
    T[3][2] = -m_pos.z;

    return glm::transpose(R) * T;
}


glm::mat4 Camera::getProjMatrix() const {
    float n = settings.nearPlane;
    float f = settings.farPlane;
    float tanH = tan(m_heightAngle / 2.f);
    float tanW = tanH * m_aspect;

    // OpenGL perspective projection matrix
    glm::mat4 proj(0.f);

    proj[0][0] = 1.f / tanW;
    proj[1][1] = 1.f / tanH;
    proj[2][2] = -((f + n) / (f - n));
    proj[2][3] = -1.f;
    proj[3][2] = -((2.f * f * n) / (f - n));

    return proj;
}



void Camera::moveForward(float amt) {
    m_pos += glm::vec4(glm::normalize(glm::vec3(m_look)) * amt, 0);
}

void Camera::moveRight(float amt) {
    m_pos += glm::vec4(m_u * amt, 0);
}

void Camera::moveUp(float amt) {
    m_pos += glm::vec4(0, amt, 0, 0);
}


void Camera::rotate(float dx, float dy) {

    float sensitivity = 0.005f;

    float yaw = -dx * sensitivity;
    float pitch = -dy * sensitivity;

    glm::mat4 yawMat = glm::mat4(1.f);
    yawMat[0][0] =  cos(yaw); yawMat[0][2] = sin(yaw);
    yawMat[2][0] = -sin(yaw); yawMat[2][2] = cos(yaw);

    glm::mat4 pitchMat = glm::mat4(1.f);
    pitchMat[1][1] = cos(pitch);  pitchMat[1][2] = -sin(pitch);
    pitchMat[2][1] = sin(pitch);  pitchMat[2][2] = cos(pitch);

    glm::vec4 newLook = yawMat * pitchMat * m_look;

    glm::vec3 look3 = glm::normalize(glm::vec3(newLook));
    m_look = glm::vec4(look3, 0.f);

    updateBasis();

}
