#include "transform.h"
#include "glm/ext/matrix_transform.hpp"

namespace Transform
{

    glm::mat4 translate(const glm::vec3& v) {
        return glm::mat4 {
            glm::vec4(1.f, 0.f, 0.f, 0.f),
            glm::vec4(0.f, 1.f, 0.f, 0.f),
            glm::vec4(0.f, 0.f, 1.f, 0.f),
            glm::vec4(v.x, v.y, v.z, 1.f)
        };
    }

    glm::mat4 rotate(float t, const glm::vec3& v) {
        glm::vec3 u = glm::normalize(v);
        float cost = cos(t), sint = sin(t), cost_1 = 1.f - cost;
        return glm::mat4 {

            glm::vec4(cost  +  u.x * u.x * cost_1,
                      u.x * u.y * cost_1  +  u.z * sint,
                      u.x * u.z * cost_1  -  u.y * sint,
                      0.f),

            glm::vec4(u.x * u.y * cost_1  -  u.z * sint,
                      cost  +  u.y * u.y * cost_1,
                      u.y * u.z * cost_1  +  u.x * sint,
                      0.f),

            glm::vec4(u.x * u.z * cost_1  +  u.y * sint,
                      u.y * u.z * cost_1  -  u.x * sint,
                      cost  +  u.z * u.z * cost_1,
                      0.f),

            glm::vec4(0.f,
                      0.f,
                      0.f,
                      1.f)

        };
    }

    glm::mat4 toGlmMat(const aiMatrix4x4& mat) {
        return glm::mat4{
            mat.a1, mat.b1, mat.c1, mat.d1,
            mat.a2, mat.b2, mat.c2, mat.d2,
            mat.a3, mat.b3, mat.c3, mat.d3,
            mat.a4, mat.b4, mat.c4, mat.d4
        };
    }

    // Baraff equation (5-3)
    glm::mat3 computeCubeInertia(float M, const glm::vec3& dim) {
        float w = dim.x, h = dim.y, d = dim.z;
        return glm::scale(glm::mat4{1.f}, {
                                              M / 12.f * (h*h + d*d),
                                              M / 12.f * (w*w + d*d),
                                              M / 12.f * (w*w + h*h)
                                          });
    }

    // Inertia tensor for a sphere
    glm::mat3 computeSphereInertia(float M, float r) {
        float I = (2.f / 5.f) * M * r*r;

        return glm::mat3{
            I, 0.f, 0.f,
            0.f, I, 0.f,
            0.f, 0.f, I
        };
    }

    // Inertia tensor for a cylinder along Y axis
    glm::mat3 computeCylinderInertia(float M, float r, float h) {
        float Ixx = M * (3 * r*r + h*h) / 12.f;
        float Iyy = M * r*r / 2.f;

        return glm::mat3{
            Ixx, 0.f, 0.f,
            0.f, Iyy, 0.f,
            0.f, 0.f, Ixx
        };
    }

    // Inertia tensor for a cone along Y axis
    glm::mat3 computeConeInertia(float M, float r, float h) {
        float Ixx = M * (3 * r*r / 20.f + h*h / 10.f);
        float Iyy = 3 * M * r*r / 10.f;

        return glm::mat3{
            Ixx, 0.f, 0.f,
            0.f, Iyy, 0.f,
            0.f, 0.f, Ixx
        };
    }

}
