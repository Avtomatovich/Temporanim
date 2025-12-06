#pragma once
#include <vector>
#include <glm/glm.hpp>

class Cylinder {
public:
    Cylinder() {}
    void updateParams(int p1, int p2);
    const std::vector<float> &generateShape() const { return m_vertexData; }

private:
    void buildSides();
    void buildCap(bool top);
    void emitTri(const glm::vec3&, const glm::vec3&, const glm::vec3&);
    void pushVertex(const glm::vec3&, const glm::vec3&);

    std::vector<float> m_vertexData;
    int m_p1 = 1;
    int m_p2 = 3;
};
