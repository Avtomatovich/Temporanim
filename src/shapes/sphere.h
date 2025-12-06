#pragma once
#include <vector>
#include <glm/glm.hpp>

class Sphere {
public:
    Sphere() {}
    void updateParams(int p1, int p2);
    const std::vector<float> &generateShape() const { return m_vertexData; }

private:
    void tesselate();
    void emitQuad(const glm::vec3 &a, const glm::vec3 &b,
                  const glm::vec3 &c, const glm::vec3 &d);
    void emitTri(const glm::vec3&, const glm::vec3&, const glm::vec3&);
    void pushVertex(const glm::vec3&, const glm::vec3&);

    std::vector<float> m_vertexData;
    int m_p1 = 10;
    int m_p2 = 10;
};
