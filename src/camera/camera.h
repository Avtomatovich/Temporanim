#pragma once

#include <glm/glm.hpp>
#include "utils/scenedata.h"

class Camera {
public:
    Camera() = default;
    Camera(const SceneCameraData &data, int width, int height);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjMatrix() const;

    glm::vec3 getPosition() const { return glm::vec3(m_pos); }
    void setAspect(float a) { m_aspect = a; }

    // movement
    void moveForward(float amt);   // along look
    void moveRight(float amt);     // along right = cross(look, up)
    void moveUp(float amt);        // along world up (0,1,0)

    // rotation
    void rotate(float dx, float dy);

private:
    glm::vec4 m_pos;
    glm::vec4 m_look;
    glm::vec4 m_up;

    float m_heightAngle;
    float m_aspect;

    // orthonormal basis
    glm::vec3 m_u, m_v, m_w;

    void updateBasis();
};
