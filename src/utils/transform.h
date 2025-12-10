#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <assimp/matrix4x4.inl>
#include <glm/glm.hpp>

namespace Transform
{
    glm::mat4 translate(const glm::vec3& v);
    glm::mat4 rotate(float t, const glm::vec3& v);
}

#endif // TRANSFORM_H
