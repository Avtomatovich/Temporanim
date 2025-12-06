#include "cylinder.h"

void Cylinder::updateParams(int param1, int param2) {
    m_vertexData.clear();
    m_param1 = param1;
    m_param2 = std::max(param2, 3);
    setVertexData();
}

glm::vec2 Cylinder::calcUV(glm::vec3& pt) {
    glm::vec2 uv{0.f};

    float theta = atan2(pt.z, pt.x);

    uv.x = theta < 0.f ?
           -theta / (2 * M_PI) :
            1.f - (theta / (2 * M_PI));

    uv.y = pt.y + 0.5f;

    return uv;
}

void Cylinder::makeCapTile(glm::vec3 topLeft,
                           glm::vec3 topRight,
                           glm::vec3 bottomLeft,
                           glm::vec3 bottomRight)
{
    glm::vec3 N = glm::normalize(glm::cross(bottomLeft - topLeft, bottomRight - topLeft));

    glm::vec2 tL_UV = calcPlaneUV(topLeft, N);
    glm::vec2 bR_UV = calcPlaneUV(bottomRight, N);
    glm::vec2 tR_UV = calcPlaneUV(topRight, N);
    glm::vec2 bL_UV = calcPlaneUV(bottomLeft, N);

    auto [T1, B1] = calcTB(topLeft, bottomRight, topRight, tL_UV, bR_UV, tR_UV);
    auto [T2, B2] = calcTB(topLeft, bottomLeft, bottomRight, tL_UV, bL_UV, bR_UV);

    // triangle 1
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, N);
    insertVec2(m_vertexData, tL_UV);
    insertVec3(m_vertexData, T1);
    insertVec3(m_vertexData, B1);
    
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, N);
    insertVec2(m_vertexData, bR_UV);
    insertVec3(m_vertexData, T1);
    insertVec3(m_vertexData, B1);

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, N);
    insertVec2(m_vertexData, tR_UV);
    insertVec3(m_vertexData, T1);
    insertVec3(m_vertexData, B1);


    // triangle 2
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, N);
    insertVec2(m_vertexData, tL_UV);
    insertVec3(m_vertexData, T2);
    insertVec3(m_vertexData, B2);

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, N);
    insertVec2(m_vertexData, bL_UV);
    insertVec3(m_vertexData, T2);
    insertVec3(m_vertexData, B2);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, N);
    insertVec2(m_vertexData, bR_UV);
    insertVec3(m_vertexData, T2);
    insertVec3(m_vertexData, B2);
}

void Cylinder::makeLateralTile(glm::vec3 topLeft,
                               glm::vec3 topRight,
                               glm::vec3 bottomLeft,
                               glm::vec3 bottomRight)
{
    glm::vec3 tL_N = calcNorm(topLeft);
    glm::vec3 bR_N = calcNorm(bottomRight);
    glm::vec3 tR_N = calcNorm(topRight);
    glm::vec3 bL_N = calcNorm(bottomLeft);

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

void Cylinder::makeCapSlice(float y, float currentTheta, float nextTheta){
    std::vector<float> radii;

    float d = 1.f / m_param1;

    for (int i = 0; i <= m_param1; ++i) {
        radii.push_back(0.f + i * d * m_radius);
    }

    for (int v = 0; v < m_param1; ++v) {
        glm::vec3 tL {
            radii[v] * glm::cos(currentTheta),
            y,
            radii[v] * glm::sin(currentTheta)
        };

        glm::vec3 tR {
            radii[v] * glm::cos(nextTheta),
            y,
            radii[v] * glm::sin(nextTheta)
        };

        glm::vec3 bL {
            radii[v + 1] * glm::cos(currentTheta),
            y,
            radii[v + 1] * glm::sin(currentTheta)
        };

        glm::vec3 bR {
            radii[v + 1] * glm::cos(nextTheta),
            y,
            radii[v + 1] * glm::sin(nextTheta)
        };

        makeCapTile(tL, tR, bL, bR);
    }
}

void Cylinder::makeLateralSlice(float currentTheta, float nextTheta){
    std::vector<float> height;

    float d = 1.f / m_param1;

    for (int i = 0; i <= m_param1; ++i) {
        height.push_back(m_radius + i * d * -1.f);
    }

    for (int v = 0; v < m_param1; ++v) {
        glm::vec3 tL {
            m_radius * glm::cos(currentTheta),
            height[v],
            m_radius * glm::sin(currentTheta)
        };

        glm::vec3 tR {
            m_radius * glm::cos(nextTheta),
            height[v],
            m_radius * glm::sin(nextTheta)
        };

        glm::vec3 bL {
            m_radius * glm::cos(currentTheta),
            height[v + 1],
            m_radius * glm::sin(currentTheta)
        };

        glm::vec3 bR {
            m_radius * glm::cos(nextTheta),
            height[v + 1],
            m_radius * glm::sin(nextTheta)
        };

        makeLateralTile(tL, tR, bL, bR);
    }
}

void Cylinder::makeWedge(float currentTheta, float nextTheta) {
    makeCapSlice(-m_radius, currentTheta, nextTheta);
    makeCapSlice( m_radius, nextTheta, currentTheta);
    makeLateralSlice(nextTheta, currentTheta);
}

void Cylinder::setVertexData() {
    std::vector<float> theta;

    float step = glm::radians(360.f / m_param2);

    for (int i = 0; i <= m_param2; ++i) {
        theta.push_back(i * step);
    }

    for (int c = 0; c < m_param2; ++c) {
        makeWedge(theta[c], theta[c + 1]);
    }
}
