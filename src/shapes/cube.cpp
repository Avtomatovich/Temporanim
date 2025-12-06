#include "cube.h"

void Cube::updateParams(int param1) {
    m_vertexData.clear();
    m_param1 = std::max(1, param1);

    glm::vec3 v1 = glm::vec3(-0.5f, -0.5f, -0.5f);
    glm::vec3 v2 = glm::vec3( 0.5f, -0.5f, -0.5f);
    glm::vec3 v3 = glm::vec3( 0.5f,  0.5f, -0.5f);
    glm::vec3 v4 = glm::vec3(-0.5f,  0.5f, -0.5f);
    glm::vec3 v5 = glm::vec3(-0.5f, -0.5f,  0.5f);
    glm::vec3 v6 = glm::vec3( 0.5f, -0.5f,  0.5f);
    glm::vec3 v7 = glm::vec3( 0.5f,  0.5f,  0.5f);
    glm::vec3 v8 = glm::vec3(-0.5f,  0.5f,  0.5f);

    buildFace(v1, v2, v4, v3);
    buildFace(v6, v7, v2, v3);
    buildFace(v5, v8, v6, v7);
    buildFace(v1, v4, v5, v8);
    buildFace(v7, v8, v3, v4);
    buildFace(v5, v6, v1, v2);
}

void Cube::buildFace(const glm::vec3 &bl, const glm::vec3 &br,
                     const glm::vec3 &tl, const glm::vec3 &tr) {
    glm::vec3 right = br - bl;
    glm::vec3 up = tl - bl;
    float step = 1.0f / m_param1;

    for (int i = 0; i < m_param1; i++) {
        for (int j = 0; j < m_param1; j++) {
            glm::vec3 tile_bl = bl + (j * step) * right + (i * step) * up;
            glm::vec3 tile_br = bl + ((j + 1) * step) * right + (i * step) * up;
            glm::vec3 tile_tl = bl + (j * step) * right + ((i + 1) * step) * up;
            glm::vec3 tile_tr = bl + ((j + 1) * step) * right + ((i + 1) * step) * up;

            emitTriangle(tile_bl, tile_tl, tile_br);

            emitTriangle(tile_br, tile_tl, tile_tr);
        }
    }
}

void Cube::emitTriangle(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c) {
    glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));
    pushVertex(a, normal);
    pushVertex(b, normal);
    pushVertex(c, normal);
}

void Cube::pushVertex(const glm::vec3 &pos, const glm::vec3 &normal) {
    m_vertexData.push_back(pos.x);
    m_vertexData.push_back(pos.y);
    m_vertexData.push_back(pos.z);
    m_vertexData.push_back(normal.x);
    m_vertexData.push_back(normal.y);
    m_vertexData.push_back(normal.z);
}
