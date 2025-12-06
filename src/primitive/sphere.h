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
    glm::vec2 calcUV(glm::vec3& pt);

    void makeTile(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight);
    void makeWedge(float currTheta, float nextTheta);
    void makeSphere();
};

#endif // SPHERE_H
