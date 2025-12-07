#include "cube.h"

void Cube::updateParams(int param1, int param2) {
    m_vertexData.clear();
    m_param1 = param1;
    setVertexData();
}

void Cube::makeFace(const glm::vec3& topLeft,
                    const glm::vec3& topRight,
                    const glm::vec3& bottomLeft,
                    const glm::vec3& bottomRight)
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

            makePlaneTile(tL, tR, bL, bR);
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
