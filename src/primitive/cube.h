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

    void makeTile(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight);
    void makeFace(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight);
};

#endif // CUBE_H
