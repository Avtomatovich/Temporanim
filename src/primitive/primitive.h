 #ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <vector>
#include <glm/glm.hpp>

class Primitive
{
public:
    virtual void updateParams(int param1, int param2) = 0;

    inline const std::vector<float>& generateShape() { return m_vertexData; }

    virtual ~Primitive() = default;

protected:
    static constexpr float EPS = 1e-4f;

    std::vector<float> m_vertexData;

    int m_param1;
    int m_param2;
    float m_radius = 0.5;

    void init(int param1, int param2) {
        m_param1 = param1;
        m_param2 = param2;
        updateParams(param1, param2);
    }

    virtual void setVertexData() = 0;

    glm::vec2 calcPlaneUV(glm::vec3 pt, glm::vec3 n) {
        glm::vec2 uv{0.f};

        // rotate CCW
        if (n.x != 0.f) {
            // if on x-axis
            uv = n.x < 0.f ? glm::vec2{pt.z, pt.y} : glm::vec2{-pt.z, pt.y};
        } else if (n.y != 0.f) {
            // on y-axis
            uv = n.y < 0.f ? glm::vec2{pt.x, pt.z} : glm::vec2{pt.x, -pt.z};
        } else if (n.z != 0.f) {
            // if on z-axis
            uv = n.z < 0.f ? glm::vec2{-pt.x, pt.y} : glm::vec2{pt.x, pt.y};
        }

        // scale to [0, 1]
        return uv + 0.5f;
    }

    std::pair<glm::vec3, glm::vec3> calcTB(glm::vec3& p0, glm::vec3& p1, glm::vec3& p2,
                                           glm::vec2& uv0, glm::vec2& uv1, glm::vec2& uv2)
    {
        glm::vec3 e0 = p1 - p0;
        glm::vec3 e1 = p2 - p0;

        glm::vec2 duv0 = uv1 - uv0;
        glm::vec2 duv1 = uv2 - uv0;
        
        float det = duv0.x * duv1.y - duv1.x * duv0.y;

        // If det is close to 0, point tangent in x dir, bitangent in y dir
        if (fabs(det) < EPS) return {{1, 0, 0}, {0, 1, 0}};

        float coeff = 1.f / det;

        return {coeff * (e0 * duv1.y - e1 * duv0.y), coeff * (e1 * duv0.x - e0 * duv1.x)};
    }

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
