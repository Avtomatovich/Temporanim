#pragma once
#include <glm/glm.hpp>
#include <vector>

class Cube {
public:
    Cube() {}
    void updateParams(int param1);
    const std::vector<float> &getVertexData() const {
        return m_vertexData;
    }

private:
    void buildFace(const glm::vec3 &bl, const glm::vec3 &br,const glm::vec3 &tl, const glm::vec3 &tr);
    void emitTriangle(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c);
    void pushVertex(const glm::vec3 &pos, const glm::vec3 &normal);

    std::vector<float> m_vertexData;
    int m_param1 = 1;
};
