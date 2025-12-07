#include "sphere.h"

void Sphere::updateParams(int param1, int param2) {
    m_vertexData.clear();
    m_param1 = std::max(param1, 2);
    m_param2 = std::max(param2, 3);
    setVertexData();
}

glm::vec2 Sphere::calcUV(const glm::vec3& pt) {
    glm::vec2 uv{0.f};

    float theta = atan2(pt.z, pt.x);

    uv.x = theta < 0.f ?
           -theta / (2 * M_PI) :
            1.f - (theta / (2 * M_PI));

    uv.y = ( asin(pt.y / 0.5f) / M_PI ) + 0.5f;

    // if singularity, set u to 0.5
    if (fabs(uv.y) < EPS || fabs(uv.y - 1.f) < EPS) uv.x = 0.5f;

    return uv;
}

void Sphere::makeTile(const glm::vec3& topLeft,
                      const glm::vec3& topRight,
                      const glm::vec3& bottomLeft,
                      const glm::vec3& bottomRight)
{
    glm::vec3 tL_N = glm::normalize(topLeft);
    glm::vec3 bR_N = glm::normalize(bottomRight);
    glm::vec3 tR_N = glm::normalize(topRight);
    glm::vec3 bL_N = glm::normalize(bottomLeft);

    glm::vec2 tL_UV = calcUV(topLeft);
    glm::vec2 bR_UV = calcUV(bottomRight);
    glm::vec2 tR_UV = calcUV(topRight);
    glm::vec2 bL_UV = calcUV(bottomLeft);

    auto [T1, B1] = calcTB(topLeft, bottomRight, topRight, tL_UV, bR_UV, tR_UV);
    auto [T2, B2] = calcTB(topLeft, bottomLeft, bottomRight, tL_UV, bL_UV, bR_UV);

    // triangle 1
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, tL_N);
    insertVec2(m_vertexData, tL_UV);
    insertVec3(m_vertexData, T1);
    insertVec3(m_vertexData, B1);
    
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, bR_N);
    insertVec2(m_vertexData, bR_UV);
    insertVec3(m_vertexData, T1);
    insertVec3(m_vertexData, B1);

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, tR_N);
    insertVec2(m_vertexData, tR_UV);
    insertVec3(m_vertexData, T1);
    insertVec3(m_vertexData, B1);


    // triangle 2
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, tL_N);
    insertVec2(m_vertexData, tL_UV);
    insertVec3(m_vertexData, T2);
    insertVec3(m_vertexData, B2);

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, bL_N);
    insertVec2(m_vertexData, bL_UV);
    insertVec3(m_vertexData, T2);
    insertVec3(m_vertexData, B2);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, bR_N);
    insertVec2(m_vertexData, bR_UV);
    insertVec3(m_vertexData, T2);
    insertVec3(m_vertexData, B2);
}

void Sphere::makeWedge(float currentTheta, float nextTheta) {
    std::vector<float> phi;

    float d = 1.f / m_param1;

    for (int i = 0; i <= m_param1; ++i) {
        phi.push_back(0.f + i * d * glm::radians(180.f));
    }

    for (int r = 0; r < m_param1; ++r) {
        glm::vec3 tL {
             m_radius * glm::sin(phi[r]) * glm::cos(currentTheta),
             m_radius * glm::cos(phi[r]),
            -m_radius * glm::sin(phi[r]) * glm::sin(currentTheta)
        };

        glm::vec3 tR {
             m_radius * glm::sin(phi[r]) * glm::cos(nextTheta),
             m_radius * glm::cos(phi[r]),
            -m_radius * glm::sin(phi[r]) * glm::sin(nextTheta)
        };

        glm::vec3 bL {
             m_radius * glm::sin(phi[r + 1]) * glm::cos(currentTheta),
             m_radius * glm::cos(phi[r + 1]),
            -m_radius * glm::sin(phi[r + 1]) * glm::sin(currentTheta)
        };

        glm::vec3 bR {
             m_radius * glm::sin(phi[r + 1]) * glm::cos(nextTheta),
             m_radius * glm::cos(phi[r + 1]),
            -m_radius * glm::sin(phi[r + 1]) * glm::sin(nextTheta)
        };

        makeTile(tL, tR, bL, bR);
    }
}

void Sphere::setVertexData() {
    float step = glm::radians(360.f / m_param2);

    for (int i = 0; i < m_param2; ++i) {
        makeWedge(i * step, (i + 1) * step);
    }
}
