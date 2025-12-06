#include "cone.h"
#include <glm/gtc/constants.hpp>

void Cone::updateParams(int p1, int p2) {
    m_vertexData.clear();
    m_p1 = std::max(1, p1);
    m_p2 = std::max(3, p2);
    buildSides();
    buildBase();
}

void Cone::buildSides() {
    float dTheta = glm::two_pi<float>() / m_p2;
    float dy = 1.f / m_p1;

    for (int i = 0; i < m_p2; i++) {
        float th1 = i * dTheta;
        float th2 = (i + 1) * dTheta;

        for (int j = 0; j < m_p1; j++) {
            float y1 = -0.5f + j * dy;
            float y2 = y1 + dy;
            float r1 = 0.5f * (1.f - (y1 + 0.5f));
            float r2 = 0.5f * (1.f - (y2 + 0.5f));

            glm::vec3 a(r2 * sin(th1), y2, r2 * cos(th1));
            glm::vec3 b(r2 * sin(th2), y2, r2 * cos(th2));
            glm::vec3 c(r1 * sin(th1), y1, r1 * cos(th1));
            glm::vec3 d(r1 * sin(th2), y1, r1 * cos(th2));


            auto normal = [](float theta) {
                glm::vec3 radial(sin(theta), 0, cos(theta));
                glm::vec3 n(radial.x, 0.5f, radial.z);
                return glm::normalize(n);
            };

            glm::vec3 na = normal(th1);
            glm::vec3 nb = normal(th2);
            glm::vec3 nc = normal(th1);
            glm::vec3 nd = normal(th2);

            pushVertex(a, na);
            pushVertex(c, nc);
            pushVertex(b, nb);

            pushVertex(b, nb);
            pushVertex(c, nc);
            pushVertex(d, nd);
        }
    }
}

void Cone::buildBase() {
    glm::vec3 center(0, -0.5f, 0);
    glm::vec3 n(0, -1, 0);
    float dTheta = glm::two_pi<float>() / m_p2;

    for (int i = 0; i < m_p2; i++) {
        float th1 = i * dTheta;
        float th2 = (i + 1) * dTheta;
        glm::vec3 a(0.5f * sin(th1), -0.5f, 0.5f * cos(th1));
        glm::vec3 b(0.5f * sin(th2), -0.5f, 0.5f * cos(th2));

        emitTri(center, b, a);
    }
}

void Cone::emitTri(const glm::vec3 &a,
                   const glm::vec3 &b,
                   const glm::vec3 &c)
{
    pushVertex(a, glm::vec3(0, -1, 0));
    pushVertex(b, glm::vec3(0, -1, 0));
    pushVertex(c, glm::vec3(0, -1, 0));
}

void Cone::pushVertex(const glm::vec3 &p, const glm::vec3 &n) {
    m_vertexData.push_back(p.x);
    m_vertexData.push_back(p.y);
    m_vertexData.push_back(p.z);
    m_vertexData.push_back(n.x);
    m_vertexData.push_back(n.y);
    m_vertexData.push_back(n.z);
}
