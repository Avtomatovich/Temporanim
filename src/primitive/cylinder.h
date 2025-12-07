#ifndef CYLINDER_H
#define CYLINDER_H

#include "primitive.h"

class Cylinder : public Primitive
{
public:
    Cylinder(int param1, int param2) { init(param1, param2); }

    void updateParams(int param1, int param2) override;

    ~Cylinder() override = default;

private:
    void setVertexData() override;

    // implicit normal func
    inline glm::vec3 calcNorm(const glm::vec3& pt) {
        return glm::normalize(glm::vec3{ 2 * pt.x, 0.f, 2 * pt.z });
    }

    // implicit UV func
    glm::vec2 calcUV(const glm::vec3& pt);

    // tile funcs
    void makeLateralTile(const glm::vec3& topLeft,
                         const glm::vec3& topRight,
                         const glm::vec3& bottomLeft,
                         const glm::vec3& bottomRight);

    // slice funcs
    void makeCapSlice(float y, float currentTheta, float nextTheta);
    void makeLateralSlice(float currentTheta, float nextTheta);

    // wedge funcs
    void makeWedge(float currentTheta, float nextTheta);
};

#endif // CYLINDER_H
