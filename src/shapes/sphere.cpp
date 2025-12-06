#include "sphere.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>

void Sphere::updateParams(int p1, int p2) {
    m_vertexData.clear();
    m_p1 = std::max(2, p1);
    m_p2 = std::max(3, p2);
    tesselate();
}

void Sphere::tesselate() {
    float dPhi = glm::pi<float>() / m_p1;
    float dTheta = glm::two_pi<float>() / m_p2;

    for (int i = 0; i < m_p1; i++) {
        float phi1 = i * dPhi;
        float phi2 = (i+1) * dPhi;

        for (int j = 0; j < m_p2; j++) {
            float th1 = j * dTheta;
            float th2 = (j+1) * dTheta;

            glm::vec3 a = 0.5f * glm::vec3(
                              sin(phi1)*sin(th1),
                              cos(phi1),
                              sin(phi1)*cos(th1)
                              );

            glm::vec3 b = 0.5f * glm::vec3(
                              sin(phi1)*sin(th2),
                              cos(phi1),
                              sin(phi1)*cos(th2)
                              );

            glm::vec3 c = 0.5f * glm::vec3(
                              sin(phi2)*sin(th1),
                              cos(phi2),
                              sin(phi2)*cos(th1)
                              );

            glm::vec3 d = 0.5f * glm::vec3(
                              sin(phi2)*sin(th2),
                              cos(phi2),
                              sin(phi2)*cos(th2)
                              );

            emitQuad(a,b,c,d);
        }
    }
}

void Sphere::emitQuad(const glm::vec3 &a, const glm::vec3 &b,
                      const glm::vec3 &c, const glm::vec3 &d)
{
    emitTri(a,c,b);
    emitTri(b,c,d);
}

void Sphere::emitTri(const glm::vec3 &a,
                     const glm::vec3 &b,
                     const glm::vec3 &c)
{
    pushVertex(a, glm::normalize(a));
    pushVertex(b, glm::normalize(b));
    pushVertex(c, glm::normalize(c));
}

void Sphere::pushVertex(const glm::vec3 &p, const glm::vec3 &n) {
    m_vertexData.push_back(p.x);
    m_vertexData.push_back(p.y);
    m_vertexData.push_back(p.z);

    m_vertexData.push_back(n.x);
    m_vertexData.push_back(n.y);
    m_vertexData.push_back(n.z);
}
