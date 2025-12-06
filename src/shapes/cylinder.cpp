#include "cylinder.h"
#include <glm/gtc/constants.hpp>

void Cylinder::updateParams(int p1, int p2) {
    m_vertexData.clear();
    m_p1 = std::max(1, p1);
    m_p2 = std::max(3, p2);
    buildSides();
    buildCap(true);  // top
    buildCap(false); // bottom
}

void Cylinder::buildSides() {
    float dTheta = glm::two_pi<float>() / m_p2;
    float dy = 1.0f / m_p1;

    for (int i = 0; i < m_p2; i++) {
        float th1 = i * dTheta;
        float th2 = (i+1) * dTheta;

        for (int j = 0; j < m_p1; j++) {
            float y1 = -0.5f + j * dy;
            float y2 = y1 + dy;

            glm::vec3 a(0.5f*sin(th1), y2, 0.5f*cos(th1));
            glm::vec3 b(0.5f*sin(th2), y2, 0.5f*cos(th2));
            glm::vec3 c(0.5f*sin(th1), y1, 0.5f*cos(th1));
            glm::vec3 d(0.5f*sin(th2), y1, 0.5f*cos(th2));

            glm::vec3 na = glm::normalize(glm::vec3(a.x,0,a.z));
            glm::vec3 nb = glm::normalize(glm::vec3(b.x,0,b.z));
            glm::vec3 nc = glm::normalize(glm::vec3(c.x,0,c.z));
            glm::vec3 nd = glm::normalize(glm::vec3(d.x,0,d.z));

            pushVertex(a,na);
            pushVertex(c,nc);
            pushVertex(b,nb);

            pushVertex(b,nb);
            pushVertex(c,nc);
            pushVertex(d,nd);
        }
    }
}

void Cylinder::buildCap(bool top) {
    float y = top ? 0.5f : -0.5f;
    glm::vec3 n = top ? glm::vec3(0,1,0) : glm::vec3(0,-1,0);
    float dTheta = glm::two_pi<float>() / m_p2;

    for (int i = 0; i < m_p2; i++) {
        float th1 = i * dTheta;
        float th2 = (i+1) * dTheta;

        glm::vec3 center(0,y,0);
        glm::vec3 e1(0.5f*sin(th1), y, 0.5f*cos(th1));
        glm::vec3 e2(0.5f*sin(th2), y, 0.5f*cos(th2));

        if (top)
            emitTri(center,e1,e2);
        else
            emitTri(center,e2,e1);
    }
}

void Cylinder::emitTri(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c) {
    pushVertex(a, a.y>0?glm::vec3(0,1,0):glm::vec3(0,-1,0));
    pushVertex(b, a.y>0?glm::vec3(0,1,0):glm::vec3(0,-1,0));
    pushVertex(c, a.y>0?glm::vec3(0,1,0):glm::vec3(0,-1,0));
}

void Cylinder::pushVertex(const glm::vec3 &p, const glm::vec3 &n) {
    m_vertexData.push_back(p.x);
    m_vertexData.push_back(p.y);
    m_vertexData.push_back(p.z);
    m_vertexData.push_back(n.x);
    m_vertexData.push_back(n.y);
    m_vertexData.push_back(n.z);
}
