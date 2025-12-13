#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

class Camera
{
public:
    Camera() {}

    Camera(glm::vec3 pos, glm::vec3 look, glm::vec3 up,
           float aspectRatio,
           float heightAngle,
           float near, float far);

    void setView(const glm::vec3& pos,
                 const glm::vec3& look,
                 const glm::vec3& up);

    void setAspectRatio(float aspectRatio);

    const glm::mat4& getView() const;

    const glm::mat4& getProj() const;

    const glm::vec3& getPos() const;

    const glm::vec3& getLook() const;

    void perspective(float near, float far);    

private:
    glm::mat4 m_view;
    glm::mat4 m_proj;

    float m_aspectRatio;
    float m_heightAngle;
    float m_widthAngle;

    float m_near;
    float m_far;

    glm::vec3 m_pos;
    glm::vec3 m_look;
};

#endif // CAMERA_H
