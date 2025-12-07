#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <vector>
#include <glm/glm.hpp>

class Primitive
{
public:
    const std::vector<float>& generateShape();

    virtual void updateParams(int param1, int param2) = 0;

    virtual ~Primitive() = default;

protected:
    // member vars
    std::vector<float> m_vertexData;

    int m_param1;
    int m_param2;
    float m_radius = 0.5;

    static constexpr float EPS = 1e-4f;

    // protected funcs
    virtual void setVertexData() = 0;

    void init(int param1, int param2);

    glm::vec2 calcPlaneUV(const glm::vec3& pt,
                          const glm::vec3& n);

    std::pair<glm::vec3, glm::vec3> calcTB(const glm::vec3& p0,
                                           const glm::vec3& p1,
                                           const glm::vec3& p2,
                                           const glm::vec2& uv0,
                                           const glm::vec2& uv1,
                                           const glm::vec2& uv2);

    void makePlaneTile(const glm::vec3& topLeft,
                       const glm::vec3& topRight,
                       const glm::vec3& bottomLeft,
                       const glm::vec3& bottomRight);

    inline void insertVec2(std::vector<float>& data, glm::vec2 v) {
        data.push_back(v.x);
        data.push_back(v.y);
    }

    inline void insertVec3(std::vector<float>& data, glm::vec3 v) {
        data.push_back(v.x);
        data.push_back(v.y);
        data.push_back(v.z);
    }
};

#endif // PRIMITIVE_H
