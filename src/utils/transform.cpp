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

}
