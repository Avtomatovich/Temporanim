#include "primitive.h"

void Primitive::init(int param1, int param2) {
    m_param1 = param1;
    m_param2 = param2;
    updateParams(param1, param2);
}

const std::vector<float>& Primitive::generateShape() {
    return m_vertexData;
}

void Primitive::makePlaneTile(const glm::vec3& topLeft,
                              const glm::vec3& topRight,
                              const glm::vec3& bottomLeft,
                              const glm::vec3& bottomRight)
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

glm::vec2 Primitive::calcPlaneUV(const glm::vec3& pt,
                                 const glm::vec3& n)
{
    glm::vec2 uv{0.f};

    // rotate CCW
    if (n.x != 0.f) {
        // if on x-axis
        uv = n.x < 0.f ? glm::vec2{pt.z, pt.y} : glm::vec2{-pt.z, pt.y};
    } else if (n.y != 0.f) {
        // on y-axis
        uv = n.y < 0.f ? glm::vec2{pt.x, pt.z} : glm::vec2{pt.x, -pt.z};
    } else if (n.z != 0.f) {
        // if on z-axis
        uv = n.z < 0.f ? glm::vec2{-pt.x, pt.y} : glm::vec2{pt.x, pt.y};
    }

    // scale to [0, 1]
    return uv + 0.5f;
}

std::pair<glm::vec3, glm::vec3> Primitive::calcTB(const glm::vec3& p0,
                                                  const glm::vec3& p1,
                                                  const glm::vec3& p2,
                                                  const glm::vec2& uv0,
                                                  const glm::vec2& uv1,
                                                  const glm::vec2& uv2)
{
    glm::vec3 e0 = p1 - p0;
    glm::vec3 e1 = p2 - p0;

    glm::vec2 duv0 = uv1 - uv0;
    glm::vec2 duv1 = uv2 - uv0;

    float det = duv0.x * duv1.y - duv1.x * duv0.y;

    // If det is close to 0, point tangent in x dir, bitangent in y dir
    if (fabs(det) < EPS) return {{1, 0, 0}, {0, 1, 0}};

    float coeff = 1.f / det;

    return {coeff * (e0 * duv1.y - e1 * duv0.y), coeff * (-e0 * duv1.x + e1 * duv0.x)};
}
