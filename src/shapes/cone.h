#ifndef CONE_H
#define CONE_H

#include <glm/glm.hpp>
#include <vector>

class Cone {
public:
    Cone() {};
    void updateParams(int p1, int p2);
    const std::vector<float> &generateShape() const { return m_vertexData; }

private:
    void buildSides();
    void buildBase();
    void emitTri(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c);
    void pushVertex(const glm::vec3 &a, const glm::vec3 &b);

    std::vector<float> m_vertexData;
    int m_p1 = 1;
    int m_p2 = 3;
};

#endif // CONE_H
