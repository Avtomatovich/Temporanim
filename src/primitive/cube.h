#ifndef CUBE_H
#define CUBE_H

#include "primitive.h"

class Cube : public Primitive
{
public:
    Cube(int param1) { init(param1, 0); }

    void updateParams(int param1, int param2) override;

    ~Cube() override = default;

private:
    void setVertexData() override;

    void makeFace(const glm::vec3& topLeft,
                  const glm::vec3& topRight,
                  const glm::vec3& bottomLeft,
                  const glm::vec3& bottomRight);
};

#endif // CUBE_H
