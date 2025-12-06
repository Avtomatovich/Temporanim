#include "cube.h"

void Cube::updateParams(int param1, int param2) {
    m_vertexData.clear();
    m_param1 = param1;
    setVertexData();
}

void Cube::makeTile(glm::vec3 topLeft,
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

void Cube::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight)
{
    float d = 1.f / m_param1;

    std::vector<glm::vec3> xlerp, ylerp;

    for (int i = 0; i <= m_param1; ++i) {
        xlerp.push_back(i * d * (topRight - topLeft));
        ylerp.push_back(i * d * (bottomLeft - topLeft));
    }

    for (int r = 0; r < m_param1; ++r) {
        for (int c = 0; c < m_param1; ++c) {
            auto tL = topLeft + ylerp[r    ] + xlerp[c    ];
            auto tR = topLeft + ylerp[r    ] + xlerp[c + 1];
            auto bL = topLeft + ylerp[r + 1] + xlerp[c    ];
            auto bR = topLeft + ylerp[r + 1] + xlerp[c + 1];

            makeTile(tL, tR, bL, bR);
        }
    }

}

void Cube::setVertexData() {
    // face 1
    makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3( 0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, 0.5f));

    // face 2
    makeFace(glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f));

    // face 3
    makeFace(glm::vec3( 0.5f,  0.5f,  0.5f),
             glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f,  0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f));

    // face 4
    makeFace(glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f,  0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f,  0.5f));

    // face 5
    makeFace(glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f,  0.5f),
             glm::vec3( 0.5f,  0.5f,  0.5f));

    // face 6
    makeFace(glm::vec3(-0.5f, -0.5f,  0.5f),
             glm::vec3( 0.5f, -0.5f,  0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f));
}
