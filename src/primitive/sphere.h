#ifndef SPHERE_H
#define SPHERE_H

#include "primitive.h"

class Sphere : public Primitive
{
public:
    Sphere(int param1, int param2) { init(param1, param2); }

    void updateParams(int param1, int param2) override;

    ~Sphere() override = default;

private:
    void setVertexData() override;

    // implicit UV func
    glm::vec2 calcUV(const glm::vec3& pt);

    void makeTile(const glm::vec3& topLeft,
                  const glm::vec3& topRight,
                  const glm::vec3& bottomLeft,
                  const glm::vec3& bottomRight);

    void makeWedge(float currTheta, float nextTheta);
};

#endif // SPHERE_H
