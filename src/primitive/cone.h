#ifndef CONE_H
#define CONE_H

#include "primitive.h"

class Cone : public Primitive
{
public:
    Cone(int param1, int param2) { init(param1, param2); }

    void updateParams(int param1, int param2) override;

    ~Cone() override = default;

private:
    void setVertexData() override;

    // implicit normal func
    inline glm::vec3 calcNorm(glm::vec3& pt) {
        return glm::normalize(glm::vec3{ 2 * pt.x,
                                        -0.25f * (2.f * pt.y - 1.f),
                                         2 * pt.z });
    }

    // implicit UV func
    glm::vec2 calcUV(glm::vec3& pt);

    // tile funcs
    void makeCapTile(glm::vec3 topLeft,
                     glm::vec3 topRight,
                     glm::vec3 bottomLeft,
                     glm::vec3 bottomRight);
    void makeSlopeTile(glm::vec3 topLeft,
                       glm::vec3 topRight,
                       glm::vec3 bottomLeft,
                       glm::vec3 bottomRight);

    // slice funcs
    void makeCapSlice(float currentTheta, float nextTheta);
    void makeSlopeSlice(float currentTheta, float nextTheta);

    // wedge func
    void makeWedge(float currentTheta, float nextTheta);
};

#endif // CONE_H
