#include "transform.h"

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

    aiMatrix4x4 getAiMat(const glm::mat4& mat) {
        aiMatrix4x4 res;
        res.a1 = mat[0][0], res.a2 = mat[1][0], res.a3 = mat[2][0], res.a4 = mat[3][0];
        res.b1 = mat[0][1], res.b2 = mat[1][1], res.b3 = mat[2][1], res.b4 = mat[3][1];
        res.c1 = mat[0][2], res.c2 = mat[1][2], res.c3 = mat[2][2], res.c4 = mat[3][2];
        res.d1 = mat[0][3], res.d2 = mat[1][3], res.d3 = mat[2][3], res.d4 = mat[3][3];
        return res;
    }

    glm::mat4 getGlmMat(const aiMatrix4x4& mat) {
        return glm::mat4{
            mat.a1, mat.b1, mat.c1, mat.d1,
            mat.a2, mat.b2, mat.c2, mat.d2,
            mat.a3, mat.b3, mat.c3, mat.d3,
            mat.a4, mat.b4, mat.c4, mat.d4
        };
    }

}
